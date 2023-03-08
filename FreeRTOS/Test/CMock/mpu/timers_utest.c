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
static BaseType_t const xPriorityWoken = ( BaseType_t ) 4U;
static BaseType_t * const pxPriorityWoken = ( BaseType_t * const ) &xPriorityWoken;
static void * pvTimerID = NULL;

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

void suiteSetUp( void )
{
}

int suiteTearDown( int numFailures )
{
    return 0;
}

/* ==========================  Helper functions =========================== */

/* =============================  Test Cases ============================== */

void test_vTimerSetTimerID_unprivileged_accessibleID( void )
{
    unprivilegedTask_raisesAndLowersPrivilege();
    xPortIsAuthorizedToAccessBuffer_ExpectAndReturn( pvTimerID, 1U, tskMPU_READ_PERMISSION, pdTRUE );
    /* This validates the ID pointer passed in */
    vTimerSetTimerID_Expect( xHandle, pvTimerID );

    MPU_vTimerSetTimerID( xHandle, pvTimerID );
}

void test_vTimerSetTimerID_unprivileged_inaccessibleID( void )
{
    unprivilegedTask_raisesAndLowersPrivilege();
    xPortIsAuthorizedToAccessBuffer_ExpectAndReturn( pvTimerID, 1U, tskMPU_READ_PERMISSION, pdFALSE );

    MPU_vTimerSetTimerID( xHandle, pvTimerID );
}

void test_vTimerSetTimerID_privileged( void )
{
    privilegedTask_retainsPrivilege();
    /* This validates the ID pointer passed in */
    vTimerSetTimerID_Expect( xHandle, pvTimerID );

    MPU_vTimerSetTimerID( xHandle, pvTimerID );
}

void test_xTimerGenericCommand_unprivileged_accessibleWokenBuffer( void )
{
    unprivilegedTask_raisesAndLowersPrivilege();
    xPortIsAuthorizedToAccessBuffer_ExpectAndReturn( pxPriorityWoken, sizeof( BaseType_t ), tskMPU_WRITE_PERMISSION, pdTRUE );
    xTimerGenericCommand_ExpectAndReturn( xHandle, tmrCOMMAND_START, xSingleTickIncrement, pxPriorityWoken, xSingleTickIncrement, pdTRUE );

    xResult = MPU_xTimerGenericCommand( xHandle, tmrCOMMAND_START, xSingleTickIncrement, pxPriorityWoken, xSingleTickIncrement );
    TEST_ASSERT_EQUAL( pdTRUE, xResult );
}

void test_xTimerGenericCommand_unprivileged_nullHigherPriorityWoken( void )
{
    BaseType_t * const nullPriorityWoken = NULL;

    unprivilegedTask_raisesAndLowersPrivilege();
    xTimerGenericCommand_ExpectAndReturn( xHandle, tmrCOMMAND_START, xSingleTickIncrement, nullPriorityWoken, xSingleTickIncrement, pdTRUE );

    xResult = MPU_xTimerGenericCommand( xHandle, tmrCOMMAND_START, xSingleTickIncrement, nullPriorityWoken, xSingleTickIncrement );
    TEST_ASSERT_EQUAL( pdTRUE, xResult );
}

void test_xTimerGenericCommand_unprivileged_inaccessibleWokenBuffer( void )
{
    unprivilegedTask_raisesAndLowersPrivilege();
    xPortIsAuthorizedToAccessBuffer_ExpectAndReturn( pxPriorityWoken, sizeof( BaseType_t ), tskMPU_WRITE_PERMISSION, pdFALSE );

    xResult = MPU_xTimerGenericCommand( xHandle, tmrCOMMAND_START, xSingleTickIncrement, pxPriorityWoken, xSingleTickIncrement );

    TEST_ASSERT_EQUAL( pdFALSE, xResult );
}

void test_xTimerGenericCommand_privileged( void )
{
    privilegedTask_retainsPrivilege();
    xTimerGenericCommand_ExpectAndReturn( xHandle, tmrCOMMAND_START, xSingleTickIncrement, pxPriorityWoken, xSingleTickIncrement, pdTRUE );

    xResult = MPU_xTimerGenericCommand( xHandle, tmrCOMMAND_START, xSingleTickIncrement, pxPriorityWoken, xSingleTickIncrement );

    TEST_ASSERT_EQUAL( pdTRUE, xResult );
}
