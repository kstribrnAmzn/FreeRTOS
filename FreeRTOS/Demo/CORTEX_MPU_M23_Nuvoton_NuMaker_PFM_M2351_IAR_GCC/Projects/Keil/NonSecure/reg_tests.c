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

extern void vRegTest1Implementation( void );
extern void vRegTest2Implementation( void );

/*-----------------------------------------------------------*/

static void prvRegTest1Task( void* pvParameters ) /* __attribute__(( naked )) */
{
    (void) pvParameters;
    vRegTest1Implementation();
}
/*-----------------------------------------------------------*/

static void prvRegTest2Task( void* pvParameters ) /* __attribute__(( naked )) */
{
    (void) pvParameters;
    vRegTest2Implementation();
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