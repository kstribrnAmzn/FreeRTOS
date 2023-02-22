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
/*! @file queue_utest.c */

/* C runtime includes. */
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"

/* Test includes. */
#include "unity.h"
#include "unity_memory.h"

#include "task.h"
#include "timers.h"
#include "event_groups.h"
#include "stream_buffer.h"

#include "mock_fake_port.h"
#include "portmacro.h"
#include "privilege_helper.h"
#include "mock_queue.h"
#include "queue.h"
#include "mock_local_portable.h"

#include "mpu_prototypes.h"

/* ===============================  CONSTANTS =============================== */
static const BaseType_t xFrontCopyPosition = 0;
static const TickType_t xNoTickWait = 0U;
static const QueueHandle_t xQueue = ( QueueHandle_t ) 3U; /* Point to any address*/
static void * const pvQueueItem = NULL;

/* ============================  GLOBAL VARIABLES =========================== */
BaseType_t xResult;

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

static void unprivileged_queueAccess_allowed( QueueHandle_t xQueue,
                                              void * const pvQueueItem,
                                              uint16_t usPermission,
                                              BaseType_t xGranted )
{
    unprivilegedTask_raisesAndLowersPrivilege();

    uxQueueGetQueueItemSize_ExpectAndReturn( xQueue, 1U );
    xPortIsAuthorizedToAccessBuffer_ExpectAndReturn( pvQueueItem, 1U, usPermission, xGranted );
}

/* =============================  Test Cases ============================== */

/**
 * @brief Test xQueuePeek with a buffer which is accessible
 */
void test_xQueuePeek_unprivileged_acessibleBuffer( void )
{
    unprivileged_queueAccess_allowed( xQueue, pvQueueItem, tskMPU_WRITE_PERMISSION, pdTRUE );
    xQueuePeek_ExpectAndReturn( xQueue, pvQueueItem, xNoTickWait, pdTRUE );

    xResult = MPU_xQueuePeek( xQueue, pvQueueItem, xNoTickWait );
    TEST_ASSERT_EQUAL( pdTRUE, xResult );
}

/**
 * @brief Test xQueuePeek with a buffer which is inaccessible
 */
void test_xQueuePeek_unprivileged_inacessibleBuffer( void )
{
    unprivileged_queueAccess_allowed( xQueue, pvQueueItem, tskMPU_WRITE_PERMISSION, pdFALSE );

    xResult = MPU_xQueuePeek( xQueue, pvQueueItem, xNoTickWait );
    TEST_ASSERT_EQUAL( pdFALSE, xResult );
}

/**
 * @brief Test xQueuePeek from a privileged task
 */
void test_xQueuePeek_privileged( void )
{
    privilegedTask_retainsPrivilege();
    xQueuePeek_ExpectAndReturn( xQueue, pvQueueItem, xNoTickWait, pdTRUE );

    xResult = MPU_xQueuePeek( xQueue, pvQueueItem, xNoTickWait );
    TEST_ASSERT_EQUAL( pdTRUE, xResult );
}

/**
 * @brief Test xQueueReceive with a buffer which is accessible
 */
void test_xQueueReceive_unprivileged_acessibleBuffer( void )
{
    unprivileged_queueAccess_allowed( xQueue, pvQueueItem, tskMPU_WRITE_PERMISSION, pdTRUE );
    xQueueReceive_ExpectAndReturn( xQueue, pvQueueItem, xNoTickWait, pdTRUE );

    xResult = MPU_xQueueReceive( xQueue, pvQueueItem, xNoTickWait );
    TEST_ASSERT_EQUAL( pdTRUE, xResult );
}

/**
 * @brief Test xQueueReceive with a buffer which is inaccessible
 */
void test_xQueueReceive_unprivileged_inacessibleBuffer( void )
{
    unprivileged_queueAccess_allowed( xQueue, pvQueueItem, tskMPU_WRITE_PERMISSION, pdFALSE );

    xResult = MPU_xQueueReceive( xQueue, pvQueueItem, xNoTickWait );
    TEST_ASSERT_EQUAL( pdFALSE, xResult );
}

/**
 * @brief Test xQueueReceive from a privileged task
 */
void test_xQueueReceive_privileged( void )
{
    privilegedTask_retainsPrivilege();
    xQueueReceive_ExpectAndReturn( xQueue, pvQueueItem, xNoTickWait, pdTRUE );

    xResult = MPU_xQueueReceive( xQueue, pvQueueItem, xNoTickWait );
    TEST_ASSERT_EQUAL( pdTRUE, xResult );
}

/**
 * @brief Test xQueueSend with a buffer which is inaccessible
 */
void test_xQueueSend_unprivileged_acessibleBuffer( void )
{
    unprivileged_queueAccess_allowed( xQueue, pvQueueItem, tskMPU_READ_PERMISSION, pdTRUE );
    xQueueGenericSend_ExpectAndReturn( xQueue, pvQueueItem, xNoTickWait, xFrontCopyPosition, pdTRUE );

    xResult = MPU_xQueueGenericSend( xQueue, pvQueueItem, xNoTickWait, xFrontCopyPosition );
    TEST_ASSERT_EQUAL( pdTRUE, xResult );
}

/**
 * @brief Test xQueueSend with a buffer which is inaccessible
 */
void test_xQueueSend_unprivileged_inacessibleBuffer( void )
{
    unprivileged_queueAccess_allowed( xQueue, pvQueueItem, tskMPU_READ_PERMISSION, pdFALSE );

    xResult = MPU_xQueueGenericSend( xQueue, pvQueueItem, xNoTickWait, xFrontCopyPosition );
    TEST_ASSERT_EQUAL( pdFALSE, xResult );
}

/**
 * @brief Test xQueueSend with a buffer which is inaccessible
 */
void test_xQueueSend_privileged( void )
{
    privilegedTask_retainsPrivilege();
    xQueueGenericSend_ExpectAndReturn( xQueue, pvQueueItem, xNoTickWait, xFrontCopyPosition, pdTRUE );

    xResult = MPU_xQueueGenericSend( xQueue, pvQueueItem, xNoTickWait, xFrontCopyPosition );
    TEST_ASSERT_EQUAL( pdTRUE, xResult );
}
