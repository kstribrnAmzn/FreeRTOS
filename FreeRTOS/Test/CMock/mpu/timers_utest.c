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
/*! @file timers_utest.c */

/* C runtime includes. */
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/* Timers includes */
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"

/* Test includes. */
#include "unity.h"
#include "unity_memory.h"

#include "event_groups.h"
#include "queue.h"
#include "stream_buffer.h"

#include "mock_fake_port.h"
#include "portmacro.h"
#include "privilege_helper.h"
#include "mock_timers.h"
#include "mock_local_portable.h"

#include "mpu_prototypes.h"

/* ===============================  CONSTANTS =============================== */
static const TimerHandle_t xHandle = NULL;
static const TickType_t xSingleTickIncrement = 1U;
static BaseType_t * const xPriorityWoken;
/* ============================  GLOBAL VARIABLES =========================== */
static BaseType_t xResult;
/* ==========================  CALLBACK FUNCTIONS =========================== */
void vApplicationGetIdleTaskMemory( StaticTask_t ** x,
                                    StackType_t ** y,
                                    uint32_t * z )
{
}

void vApplicationTickHook( void )
{
}

void vApplicationStackOverflowHook( struct tskTaskControlBlock * x,
                                    char * y )
{
}

void vApplicationIdleHook( void )
{
}

void vApplicationGetTimerTaskMemory( StaticTask_t ** ppxTimerTaskTCBBuffer,
                                     StackType_t ** ppxTimerTaskStackBuffer,
                                     uint32_t * pulTimerTaskStackSize )
{
}

void vApplicationDaemonTaskStartupHook( void )
{
}

/* ============================= Unity Fixtures ============================= */

void setUp( void )
{
}

void tearDown( void )
{
}

void suiteSetUp()
{
}

int suiteTearDown( int numFailures )
{
    return 0;
}

/* ==========================  Helper functions =========================== */


/* =============================  Test Cases ============================== */

void test_xTimerGenericCommand_unprivileged_accessibleWokenBuffer()
{
    unprivilegedTask_raisesAndLowersPrivilege();
    xPortIsAuthorizedToAccessBuffer_ExpectAndReturn( xPriorityWoken, sizeof( BaseType_t ), tskMPU_WRITE_PERMISSION, pdTRUE );
    xTimerGenericCommand_ExpectAndReturn( xHandle, tmrCOMMAND_START, xSingleTickIncrement, xPriorityWoken, xSingleTickIncrement, pdTRUE );

    xResult = MPU_xTimerGenericCommand( xHandle, tmrCOMMAND_START, xSingleTickIncrement, xPriorityWoken, xSingleTickIncrement );
    TEST_ASSERT_EQUAL( pdTRUE, xResult );
}

void test_xTimerGenericCommand_unprivileged_inaccessibleWokenBuffer()
{
    unprivilegedTask_raisesAndLowersPrivilege();
    xPortIsAuthorizedToAccessBuffer_ExpectAndReturn( xPriorityWoken, sizeof( BaseType_t ), tskMPU_WRITE_PERMISSION, pdFALSE );

    xResult = MPU_xTimerGenericCommand( xHandle, tmrCOMMAND_START, xSingleTickIncrement, xPriorityWoken, xSingleTickIncrement );

    TEST_ASSERT_EQUAL( pdFALSE, xResult );
}

void test_xTimerGenericCommand_privileged()
{
    privilegedTask_retainsPrivilege();
    xTimerGenericCommand_ExpectAndReturn( xHandle, tmrCOMMAND_START, xSingleTickIncrement, xPriorityWoken, xSingleTickIncrement, pdTRUE );

    xResult = MPU_xTimerGenericCommand( xHandle, tmrCOMMAND_START, xSingleTickIncrement, xPriorityWoken, xSingleTickIncrement );

    TEST_ASSERT_EQUAL( pdTRUE, xResult );
}
