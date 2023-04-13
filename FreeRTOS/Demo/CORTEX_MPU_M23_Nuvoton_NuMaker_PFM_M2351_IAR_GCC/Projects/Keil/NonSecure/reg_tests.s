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

    .extern ulRegTest1LoopCounter
    .extern ulRegTest2LoopCounter
    .global vRegTest1Implementation
    .global vRegTest2Implementation
    .type vRegTest1Implementation "function"
    .type vRegTest2Implementation "function"

/*-----------------------------------------------------------*/

vRegTest1Implementation:
   /* Fill the core registers with known values. */
   movs r0, #10
   movs r1, #11
   movs r2, #12
   movs r3, #13
   movs r4, #14
   movs r5, #15
   movs r6, #16
   movs r7, #17
   mov r8,  #18
   mov r9,  #19
   mov r10, #20
   mov r11, #21
   mov r12, #22

reg1_loop:
   cmp r0, #10
   bne  reg1_error_loop
   cmp r1, #11
   bne  reg1_error_loop
   cmp r2, #12
   bne  reg1_error_loop
   cmp r3, #13
   bne  reg1_error_loop
   cmp r4, #14
   bne  reg1_error_loop
   cmp r5, #15
   bne  reg1_error_loop
   cmp r6, #16
   bne  reg1_error_loop
   cmp r7, #17
   bne  reg1_error_loop
   cmp r8, #18
   bne  reg1_error_loop
   cmp r9, #19
   bne  reg1_error_loop
   cmp r10, #20
   bne  reg1_error_loop
   cmp r11, #21
   bne  reg1_error_loop
   cmp r12, #22
   bne  reg1_error_loop

   /* Everything passed, increment the loop counter. */
   push { r0, r1 }
   ldr r0, ulRegTest1LoopCounter
   ldr r1, [r0]
   adds r1, r1, #1
   str r1, [r0]

   /* Yield to increase code coverage. */
   movs r0, #1
   ldr r1, =0xe000ed04
   lsls r0, #28
   str r0, [r1]
   dsb
   /* Restore r0 and r1 from stack. */
   pop  { r0, r1 }

   /* Start again. */
   b reg1_loop

reg1_error_loop:
   /* Busy loop which holds the task. */
   nop
   b reg1_error_loop

vRegTest2Implementation:
   movs r0, #99                                          
   movs r1, #98                                          
   movs r2, #97                                          
   movs r3, #96                                          
   movs r4, #95                                          
   movs r5, #94                                          
   movs r6, #93                                          
   movs r7, #92                                          
   mov r8,  #91                                          
   mov r9,  #90                                          
   mov r10, #89                                          
   mov r11, #88                                          
   mov r12, #87                                          
                                                        
reg2_loop:                                               
   cmp r0, #99                                           
   bne  reg2_error_loop                                  
   cmp r1, #98                                           
   bne  reg2_error_loop                                  
   cmp r2, #97                                           
   bne  reg2_error_loop                                  
   cmp r3, #96                                           
   bne  reg2_error_loop                                  
   cmp r4, #95                                           
   bne  reg2_error_loop                                  
   cmp r5, #94                                           
   bne  reg2_error_loop                                  
   cmp r6, #93                                           
   bne  reg2_error_loop                                  
   cmp r7, #92                                           
   bne  reg2_error_loop                                  
   cmp r8, #91                                           
   bne  reg2_error_loop                                  
   cmp r9, #90                                           
   bne  reg2_error_loop                                  
   cmp r10, #89                                          
   bne  reg2_error_loop                                  
   cmp r11, #88                                         
   bne  reg2_error_loop                                  
   cmp r12, #87                                          
   bne  reg2_error_loop                                  
                                                        
   /* Everything passed, increment the loop counter. */
   push { r0, r1 }                                       
   ldr r0, ulRegTest2LoopCounter                   
   ldr r1, [r0]                                          
   adds r1, r1, #1                                       
   str r1, [r0]                                          
                                                        
   /* Yield to increase code coverage. */                
   movs r0, #1                                            
   ldr r1, =0xe000ed04                                   
   lsls r0, #28                                          
   str r0, [r1]                                          
   dsb                                                   
   /* Restore r0 and r1 from stack. */                   
   pop  { r0, r1 }                                       
                                                        
   /* Start again. */                                    
   b reg2_loop                                           
                                                        
reg2_error_loop:                                         
   /* Busy loop which holds the task. */                 
   nop
   b reg2_error_loop
