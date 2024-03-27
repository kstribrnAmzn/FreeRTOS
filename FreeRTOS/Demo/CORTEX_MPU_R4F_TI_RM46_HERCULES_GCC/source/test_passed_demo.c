/*
 * FreeRTOS Kernel <DEVELOPMENT BRANCH>
 * Copyright (C) 2024 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */

/* Standard includes. */
#include <stdint.h>

/* FreeRTOS includes. */
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "portmacro.h"

/* HalCoGen includes. */
#include "sci.h"

#include "demo_tasks.h"

/** @brief TCB used by the Notification Test Task */
static StaticTask_t xTestPassedTaskTCB;

/** @brief MPU Region Aligned Stack used by the Notification Test Task */
static StackType_t uxTestPassedTaskStack[ configMINIMAL_STACK_SIZE ]
    __attribute__( ( aligned( configMINIMAL_STACK_SIZE * 0x4UL ) ) );

static void testPassedTask(void * pvParameters)
{
    while(1)
    {
        sci_print( "Test Passed from task.\r\n" );
        vTaskDelay(pdMS_TO_TICKS(250));
    }
}

BaseType_t xCreateTestPassedTask()
{
    BaseType_t xReturn = pdFAIL;
    xReturn = xTaskCreateStatic(testPassedTask, "testPassedTask", configMINIMAL_STACK_SIZE, NULL, demoTEST_PASSED_TASK_PRIORITY | portPRIVILEGE_BIT, uxTestPassedTaskStack, &xTestPassedTaskTCB);
    return xReturn;
}