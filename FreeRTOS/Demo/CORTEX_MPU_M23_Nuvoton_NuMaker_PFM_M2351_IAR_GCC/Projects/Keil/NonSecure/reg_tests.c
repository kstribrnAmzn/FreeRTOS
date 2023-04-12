/*
 * FreeRTOS V202212.00
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */

#include "reg_tests.h"

/*-----------------------------------------------------------*/

#define mainREG_TEST_STACK_SIZE_WORDS       90

/* Parameters that are passed into the register check tasks solely for the
purpose of ensuring parameters are passed into tasks correctly. */
#define mainREG_TEST_TASK_1_PARAMETER       ( ( void * ) 0x12345678 )
#define mainREG_TEST_TASK_2_PARAMETER       ( ( void * ) 0x87654321 )

/*-----------------------------------------------------------*/

static void prvRegTest1Task( void* pvParameters ) /* __attribute__(( naked )) */
{
    __asm volatile
    (
		".extern ulRegTest1LoopCounter                              \n"
		".syntax unified                                            \n"
		"                                                           \n"
		"   /* Fill the core registers with known values. */        \n"
		"   movs r0, #10                                            \n"
		"   movs r1, #11                                            \n"
		"   movs r2, #12                                            \n"
		"   movs r3, #13                                            \n"
		"   movs r4, #14                                            \n"
		"   movs r5, #15                                            \n"
		"   movs r6, #16                                            \n"
		"   movs r7, #17                                            \n"
		"   mov r8,  #18                                            \n"
		"   mov r9,  #19                                            \n"
		"   mov r10, #20                                            \n"
		"   mov r11, #21                                            \n"
		"   mov r12, #22                                            \n"
		"                                                           \n"
		"reg1_loop:                                                 \n"
		"   cmp r0, #10                                             \n"
		"   bne  reg1_error_loop                                    \n"
		"   cmp r1, #11                                             \n"
		"   bne  reg1_error_loop                                    \n"
		"   cmp r2, #12                                             \n"
		"   bne  reg1_error_loop                                    \n"
		"   cmp r3, #13                                             \n"
		"   bne  reg1_error_loop                                    \n"
		"   cmp r4, #14                                             \n"
		"   bne  reg1_error_loop                                    \n"
		"   cmp r5, #15                                             \n"
		"   bne  reg1_error_loop                                    \n"
		"   cmp r6, #16                                             \n"
		"   bne  reg1_error_loop                                    \n"
		"   cmp r7, #17                                             \n"
		"   bne  reg1_error_loop                                    \n"
		"   cmp r8, #18                                             \n"
		"   bne  reg1_error_loop                                    \n"
		"   cmp r9, #19                                             \n"
		"   bne  reg1_error_loop                                    \n"
		"   cmp r10, #20                                            \n"
		"   bne  reg1_error_loop                                    \n"
		"   cmp r11, #21                                            \n"
		"   bne  reg1_error_loop                                    \n"
		"   cmp r12, #22                                            \n"
		"   bne  reg1_error_loop                                    \n"
		"                                                           \n"
		"   /* Everything passed, increment the loop counter. */    \n"
		"   push { r0, r1 }                                         \n"
		"   ldr r0, ulRegTest1LoopCounterConst                      \n"
		"   ldr r1, [r0]                                            \n"
		"   adds r1, r1, #1                                         \n"
		"   str r1, [r0]                                            \n"
		"                                                           \n"
		"   /* Yield to increase code coverage. */                  \n"
		"   mov r0, #1                                              \n"
		"   ldr r1, =0xe000ed04                                     \n"
		"   lsls r0, #28                                            \n"
		"   str r0, [r1]                                            \n"
		"   dsb                                                     \n"
		"   /* Restore r0 and r1 from stack. */                     \n"
		"   pop  { r0, r1 }                                         \n"
		"                                                           \n"
		"   /* Start again. */                                      \n"
		"   b reg1_loop                                             \n"
		"                                                           \n"
		"reg1_error_loop:                                           \n"
		"   /* Busy loop which holds the task. */                   \n"
		"   b reg1_error_loop                                       \n"
		".ltorg                                                     \n"
    );
}
/*-----------------------------------------------------------*/

static void prvRegTest2Task( void* pvParameters ) /* __attribute__(( naked )) */
{
    __asm volatile
    (
		".extern ulRegTest2LoopCounter                          		\n"
		".syntax unified                                            \n"
		"                                                           \n"
		"   /* Fill the core registers with known values. */        \n"
		"   movs r0, #99                                            \n"
		"   movs r1, #98                                            \n"
		"   movs r2, #97                                            \n"
		"   movs r3, #96                                            \n"
		"   movs r4, #95                                            \n"
		"   movs r5, #94                                            \n"
		"   movs r6, #93                                            \n"
		"   movs r7, #92                                            \n"
		"   mov r8,  #91                                            \n"
		"   mov r9,  #90                                            \n"
		"   mov r10, #89                                            \n"
		"   mov r11, #88                                            \n"
		"   mov r12, #87                                            \n"
		"                                                           \n"
		"reg2_loop:                                                 \n"
		"   cmp r0, #10                                             \n"
		"   bne  reg2_error_loop                                    \n"
		"   cmp r1, #11                                             \n"
		"   bne  reg2_error_loop                                    \n"
		"   cmp r2, #12                                             \n"
		"   bne  reg2_error_loop                                    \n"
		"   cmp r3, #13                                             \n"
		"   bne  reg2_error_loop                                    \n"
		"   cmp r4, #14                                             \n"
		"   bne  reg2_error_loop                                    \n"
		"   cmp r5, #15                                             \n"
		"   bne  reg2_error_loop                                    \n"
		"   cmp r6, #16                                             \n"
		"   bne  reg2_error_loop                                    \n"
		"   cmp r7, #17                                             \n"
		"   bne  reg2_error_loop                                    \n"
		"   cmp r8, #18                                             \n"
		"   bne  reg2_error_loop                                    \n"
		"   cmp r9, #19                                             \n"
		"   bne  reg2_error_loop                                    \n"
		"   cmp r10, #20                                            \n"
		"   bne  reg2_error_loop                                    \n"
		"   cmp r11, #21                                            \n"
		"   bne  reg2_error_loop                                    \n"
		"   cmp r12, #22                                            \n"
		"   bne  reg2_error_loop                                    \n"
		"                                                           \n"
		"		/* Everything passed, increment the loop counter. */    \n"
		"   push { r0, r1 }                                         \n"
		"   ldr r0, ulRegTest1LoopCounterConst                      \n"
		"   ldr r1, [r0]                                            \n"
		"   adds r1, r1, #1                                         \n"
		"   str r1, [r0]                                            \n"
		"                                                           \n"
		"   /* Yield to increase code coverage. */                  \n"
		"   mov r0, #1                                              \n"
		"   ldr r1, =0xe000ed04                                     \n"
		"   lsls r0, #28                                            \n"
		"   str r0, [r1]                                            \n"
		"   dsb                                                     \n"
		"   /* Restore r0 and r1 from stack. */                     \n"
		"   pop  { r0, r1 }                                         \n"
		"                                                           \n"
		"   /* Start again. */                                      \n"
		"   b reg2_loop                                             \n"
		"                                                           \n"
		"reg2_error_loop:                                           \n"
		"   /* Busy loop which holds the task. */                   \n"
		"   b reg2_error_loop                                       \n"
		".ltorg                                                     \n"
    );
}
/*-----------------------------------------------------------*/

void vStartRegTestDemo( void )
{
        xTaskCreate( prvRegTest1Task,           /* The function that implements the task. */
                "RegTest1",                         /* The name of the task. */
                 mainREG_TEST_STACK_SIZE_WORDS, /* Size of stack to allocate for the task - in words not bytes!. */
                 mainREG_TEST_TASK_1_PARAMETER, /* Parameter passed into the task. */
                 tskIDLE_PRIORITY,              /* Priority of the task. */
                 NULL );                        /* Can be used to pass out a handle to the created task. */

        xTaskCreate( prvRegTest2Task,           /* The function that implements the task. */
                "RegTest2",                         /* The name of the task. */
                 mainREG_TEST_STACK_SIZE_WORDS, /* Size of stack to allocate for the task - in words not bytes!. */
                 mainREG_TEST_TASK_2_PARAMETER, /* Parameter passed into the task. */
                 tskIDLE_PRIORITY,              /* Priority of the task. */
                 NULL );                        /* Can be used to pass out a handle to the created task. */
}