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

/* Task includes */
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"

/* Test includes. */
#include "unity.h"
#include "unity_memory.h"

#include "queue.h"
#include "timers.h"
#include "event_groups.h"
#include "stream_buffer.h"

#include "mock_fake_port.h"
#include "portmacro.h"
#include "privilege_helper.h"
#include "mock_task.h"
#include "mock_local_portable.h"

#include "mpu_prototypes.h"

/* ===============================  CONSTANTS =============================== */
static const char * pcTaskListBuffer = "taskList";
static const char * pcTaskStatusTaskName = "taskName";
static const TickType_t xSingleTick = 1U;
static const UBaseType_t uxTaskStatusTaskNumber = 1U;
static const UBaseType_t uxTaskStatusTaskPriority = 1U;
static const configRUN_TIME_COUNTER_TYPE ulTaskStatusRunTimeCounter = 999U;
static const StackType_t * pxTaskStatusStackBase = ( StackType_t * ) 10U;
static const configSTACK_DEPTH_TYPE usTaskStatusHighWaterMark = ( configSTACK_DEPTH_TYPE ) 100U;
static const configSTACK_DEPTH_TYPE usHigherHighWaterMark = ( configSTACK_DEPTH_TYPE ) 1234U;
static const BaseType_t firstIndex = 1U;

/* ============================  GLOBAL VARIABLES =========================== */
static BaseType_t xResult;
static TickType_t xPreviousWakeTime = 0U;
static char * pcBuffer = NULL;
static uint32_t ulNotificationValue = 0U;
static TimeOut_t * pxTimeOut = NULL;
static TaskHandle_t xHandle = NULL;
static TaskStatus_t xTaskStatus;
static TaskStatus_t * pxTaskStatus = &xTaskStatus;
static void * pvParameter = NULL;

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
static void initTaskStatus( TaskStatus_t * pxTaskStatus )
{
    pxTaskStatus->xHandle = xHandle;
    pxTaskStatus->pcTaskName = pcTaskStatusTaskName;
    pxTaskStatus->xTaskNumber = uxTaskStatusTaskNumber;
    pxTaskStatus->eCurrentState = eRunning;
    pxTaskStatus->uxCurrentPriority = uxTaskStatusTaskPriority;
    pxTaskStatus->uxBasePriority = uxTaskStatusTaskPriority;
    pxTaskStatus->ulRunTimeCounter = ulTaskStatusRunTimeCounter;
    pxTaskStatus->pxStackBase = ( StackType_t * ) pxTaskStatusStackBase;
    pxTaskStatus->usStackHighWaterMark = usTaskStatusHighWaterMark;
}

static void assertTaskStatusValues( TaskStatus_t * pxTaskStatus )
{
    TEST_ASSERT_EQUAL_PTR( xHandle, pxTaskStatus->xHandle );
    TEST_ASSERT_EQUAL_STRING( pcTaskStatusTaskName, pxTaskStatus->pcTaskName );
    TEST_ASSERT_EQUAL( uxTaskStatusTaskNumber, pxTaskStatus->xTaskNumber );
    TEST_ASSERT_EQUAL( uxTaskStatusTaskPriority, pxTaskStatus->uxCurrentPriority );
    TEST_ASSERT_EQUAL( uxTaskStatusTaskPriority, pxTaskStatus->uxBasePriority );
    TEST_ASSERT_EQUAL( ulTaskStatusRunTimeCounter, pxTaskStatus->ulRunTimeCounter );
    TEST_ASSERT_EQUAL( pxTaskStatusStackBase, pxTaskStatus->pxStackBase );
}

static void assertTaskStatusIsInit( TaskStatus_t * pxTaskStatus )
{
    assertTaskStatusValues( pxTaskStatus );
    TEST_ASSERT_EQUAL( eRunning, pxTaskStatus->eCurrentState );
    TEST_ASSERT_EQUAL( usTaskStatusHighWaterMark, pxTaskStatus->usStackHighWaterMark );
}

static void assertTaskStatusIsModified( TaskStatus_t * pxTaskStatus,
                                        eTaskState eState,
                                        bool bGetFreeSpace )
{
    configSTACK_DEPTH_TYPE waterMark = usTaskStatusHighWaterMark;

    if( bGetFreeSpace )
    {
        waterMark = usHigherHighWaterMark;
    }

    TEST_ASSERT_EQUAL( eState, pxTaskStatus->eCurrentState );
    TEST_ASSERT_EQUAL( waterMark, pxTaskStatus->usStackHighWaterMark );

    assertTaskStatusValues( pxTaskStatus );
}

static void taskGetInfoStub( TaskHandle_t xHandle,
                             TaskStatus_t * pxTaskStatus,
                             BaseType_t xGetFreeStackSpace,
                             eTaskState eState,
                             int call_count )
{
    assertTaskStatusIsInit( pxTaskStatus );

    if( eState != eInvalid )
    {
        pxTaskStatus->eCurrentState = eState;
    }
    else
    {
        pxTaskStatus->eCurrentState = eSuspended;
    }

    if( xGetFreeStackSpace != pdFALSE )
    {
        pxTaskStatus->usStackHighWaterMark = usHigherHighWaterMark;
    }
}

/* =============================  Test Cases ============================== */

void test_vTaskDelayUntil_unprivileged_accessiblePreviousWakeTime( void )
{
    unprivilegedTask_raisesAndLowersPrivilege();
    xPortIsAuthorizedToAccessBuffer_ExpectAndReturn( &xPreviousWakeTime, sizeof( TickType_t ), tskMPU_WRITE_PERMISSION, pdTRUE );

    xTaskDelayUntil_ExpectAndReturn( &xPreviousWakeTime, xSingleTick, pdTRUE );

    xResult = MPU_xTaskDelayUntil( &xPreviousWakeTime, xSingleTick );
    TEST_ASSERT_EQUAL( pdTRUE, xResult );
}

void test_vTaskDelayUntil_unprivileged_inaccessiblePreviousWakeTime( void )
{
    unprivilegedTask_raisesAndLowersPrivilege();
    xPortIsAuthorizedToAccessBuffer_ExpectAndReturn( &xPreviousWakeTime, sizeof( TickType_t ), tskMPU_WRITE_PERMISSION, pdFALSE );

    xResult = MPU_xTaskDelayUntil( &xPreviousWakeTime, xSingleTick );
    TEST_ASSERT_EQUAL( pdFALSE, xResult );
}

void test_vTaskDelayUntil_privileged( void )
{
    privilegedTask_retainsPrivilege();
    xTaskDelayUntil_ExpectAndReturn( &xPreviousWakeTime, xSingleTick, pdTRUE );

    xResult = MPU_xTaskDelayUntil( &xPreviousWakeTime, xSingleTick );
    TEST_ASSERT_EQUAL( pdTRUE, xResult );
}

void test_vTaskGetInfo_unprivileged_accessibleTaskStatus( void )
{
    initTaskStatus( pxTaskStatus );
    unprivilegedTask_raisesAndLowersPrivilege();
    xPortIsAuthorizedToAccessBuffer_ExpectAndReturn( pxTaskStatus, sizeof( TaskStatus_t ), tskMPU_WRITE_PERMISSION, pdTRUE );
    vTaskGetInfo_StubWithCallback( taskGetInfoStub );

    MPU_vTaskGetInfo( xHandle, pxTaskStatus, pdTRUE, eBlocked );
    assertTaskStatusIsModified( pxTaskStatus, eBlocked, true );
}

void test_vTaskGetInfo_unprivileged_inaccessibleTaskStatus( void )
{
    unprivilegedTask_raisesAndLowersPrivilege();
    xPortIsAuthorizedToAccessBuffer_ExpectAndReturn( pxTaskStatus, sizeof( TaskStatus_t ), tskMPU_WRITE_PERMISSION, pdFALSE );

    MPU_vTaskGetInfo( xHandle, pxTaskStatus, pdFALSE, eInvalid );
}

void test_vTaskGetInfo_privileged( void )
{
    initTaskStatus( pxTaskStatus );
    privilegedTask_retainsPrivilege();
    vTaskGetInfo_StubWithCallback( taskGetInfoStub );

    MPU_vTaskGetInfo( xHandle, pxTaskStatus, pdFALSE, eInvalid );
    assertTaskStatusIsModified( pxTaskStatus, eSuspended, false );
}

void test_vTaskList_unprivileged_accessibleBuffer( void )
{
    unprivilegedTask_raisesAndLowersPrivilege();
    xPortIsAuthorizedToAccessBuffer_ExpectAndReturn( pcBuffer, sizeof( char ), tskMPU_WRITE_PERMISSION, pdTRUE );
    vTaskList_Ignore();

    MPU_vTaskList( pcBuffer );
}

void test_vTaskList_unprivileged_inaccessibleBuffer( void )
{
    unprivilegedTask_raisesAndLowersPrivilege();
    xPortIsAuthorizedToAccessBuffer_ExpectAndReturn( pcBuffer, sizeof( char ), tskMPU_WRITE_PERMISSION, pdFALSE );

    MPU_vTaskList( pcBuffer );
}

void test_vTaskList_privileged( void )
{
    privilegedTask_retainsPrivilege();
    vTaskList_Ignore();

    MPU_vTaskList( pcBuffer );
}

void test_vTaskGetRunTimeStats_unprivileged_accessibleBuffer( void )
{
    unprivilegedTask_raisesAndLowersPrivilege();
    xPortIsAuthorizedToAccessBuffer_ExpectAndReturn( pcBuffer, sizeof( char ), tskMPU_WRITE_PERMISSION, pdTRUE );
    vTaskGetRunTimeStats_Ignore();

    MPU_vTaskGetRunTimeStats( pcBuffer );
}

void test_vTaskGetRunTimeStats_unprivileged_inaccessibleBuffer( void )
{
    unprivilegedTask_raisesAndLowersPrivilege();
    xPortIsAuthorizedToAccessBuffer_ExpectAndReturn( pcBuffer, sizeof( char ), tskMPU_WRITE_PERMISSION, pdFALSE );

    MPU_vTaskGetRunTimeStats( pcBuffer );
}

void test_vTaskGetRunTimeStats_privileged( void )
{
    privilegedTask_retainsPrivilege();
    vTaskGetRunTimeStats_Ignore();

    MPU_vTaskGetRunTimeStats( pcBuffer );
}

void test_xTaskCallApplicationTaskHook_unprivileged_accessibleParameters( void )
{
    unprivilegedTask_raisesAndLowersPrivilege();
    xPortIsAuthorizedToAccessBuffer_ExpectAndReturn( pvParameter, 1U, tskMPU_WRITE_PERMISSION | tskMPU_READ_PERMISSION, pdTRUE );
    xTaskCallApplicationTaskHook_ExpectAndReturn( xHandle, pvParameter, pdTRUE );

    MPU_xTaskCallApplicationTaskHook( xHandle, pvParameter );
}

void test_xTaskCallApplicationTaskHook_unprivileged_inaccessibleParameters( void )
{
    unprivilegedTask_raisesAndLowersPrivilege();
    xPortIsAuthorizedToAccessBuffer_ExpectAndReturn( pvParameter, 1U, tskMPU_WRITE_PERMISSION | tskMPU_READ_PERMISSION, pdFALSE );

    MPU_xTaskCallApplicationTaskHook( xHandle, pvParameter );
}

void test_xTaskCallApplicationTaskHook_privileged( void )
{
    privilegedTask_retainsPrivilege();
    xTaskCallApplicationTaskHook_ExpectAndReturn( xHandle, pvParameter, pdTRUE );

    MPU_xTaskCallApplicationTaskHook( xHandle, pvParameter );
}

/* Note - these tests only cover when runtime stats are enabled */
void test_uxTaskGetSystemState_unprivileged_accessibleStatusArrayAndRuntime( void )
{
    UBaseType_t arraySize = 10U;
    uint32_t arrayByteSize = arraySize * sizeof( TaskStatus_t );

    unprivilegedTask_raisesAndLowersPrivilege();
    xPortIsAuthorizedToAccessBuffer_ExpectAndReturn( pxTaskStatus, arrayByteSize, tskMPU_WRITE_PERMISSION, pdTRUE );
    xPortIsAuthorizedToAccessBuffer_ExpectAndReturn( ( configRUN_TIME_COUNTER_TYPE * ) &ulTaskStatusRunTimeCounter, sizeof( configRUN_TIME_COUNTER_TYPE ), tskMPU_WRITE_PERMISSION, pdTRUE );
    uxTaskGetSystemState_ExpectAndReturn( pxTaskStatus, arraySize, ( configRUN_TIME_COUNTER_TYPE * ) &ulTaskStatusRunTimeCounter, pdTRUE );

    xResult = MPU_uxTaskGetSystemState( pxTaskStatus, arraySize, ( configRUN_TIME_COUNTER_TYPE * ) &ulTaskStatusRunTimeCounter );
    TEST_ASSERT_EQUAL( pdTRUE, xResult );
}

void test_uxTaskGetSystemState_unprivileged_accessibleStatusArrayAndNullRuntime( void )
{
    UBaseType_t arraySize = 9U;
    uint32_t arrayByteSize = arraySize * sizeof( TaskStatus_t );

    unprivilegedTask_raisesAndLowersPrivilege();
    xPortIsAuthorizedToAccessBuffer_ExpectAndReturn( pxTaskStatus, arrayByteSize, tskMPU_WRITE_PERMISSION, pdTRUE );
    uxTaskGetSystemState_ExpectAndReturn( pxTaskStatus, arraySize, ( configRUN_TIME_COUNTER_TYPE * ) NULL, pdTRUE );

    xResult = MPU_uxTaskGetSystemState( pxTaskStatus, arraySize, ( configRUN_TIME_COUNTER_TYPE * ) NULL );
    TEST_ASSERT_EQUAL( pdTRUE, xResult );
}

void test_uxTaskGetSystemState_unprivileged_inaccessibleStatusArray( void )
{
    UBaseType_t arraySize = 8U;
    uint32_t arrayByteSize = arraySize * sizeof( TaskStatus_t );

    unprivilegedTask_raisesAndLowersPrivilege();
    xPortIsAuthorizedToAccessBuffer_ExpectAndReturn( pxTaskStatus, arrayByteSize, tskMPU_WRITE_PERMISSION, pdFALSE );
    xPortIsAuthorizedToAccessBuffer_ExpectAndReturn( ( configRUN_TIME_COUNTER_TYPE * ) &ulTaskStatusRunTimeCounter, sizeof( configRUN_TIME_COUNTER_TYPE ), tskMPU_WRITE_PERMISSION, pdTRUE );

    xResult = MPU_uxTaskGetSystemState( pxTaskStatus, arraySize, ( configRUN_TIME_COUNTER_TYPE * ) &ulTaskStatusRunTimeCounter );
    TEST_ASSERT_EQUAL( pdFALSE, xResult );
}

void test_uxTaskGetSystemState_unprivileged_inaccessibleStatusArrayAndNullRuntime( void )
{
    UBaseType_t arraySize = 7U;
    uint32_t arrayByteSize = arraySize * sizeof( TaskStatus_t );

    unprivilegedTask_raisesAndLowersPrivilege();
    xPortIsAuthorizedToAccessBuffer_ExpectAndReturn( pxTaskStatus, arrayByteSize, tskMPU_WRITE_PERMISSION, pdFALSE );

    xResult = MPU_uxTaskGetSystemState( pxTaskStatus, arraySize, NULL );
    TEST_ASSERT_EQUAL( pdFALSE, xResult );
}

void test_uxTaskGetSystemState_unprivileged_inaccessibleRuntime( void )
{
    UBaseType_t arraySize = 6U;
    uint32_t arrayByteSize = arraySize * sizeof( TaskStatus_t );

    unprivilegedTask_raisesAndLowersPrivilege();
    xPortIsAuthorizedToAccessBuffer_ExpectAndReturn( pxTaskStatus, arrayByteSize, tskMPU_WRITE_PERMISSION, pdTRUE );
    xPortIsAuthorizedToAccessBuffer_ExpectAndReturn( ( configRUN_TIME_COUNTER_TYPE * ) &ulTaskStatusRunTimeCounter, sizeof( configRUN_TIME_COUNTER_TYPE ), tskMPU_WRITE_PERMISSION, pdFALSE );

    xResult = MPU_uxTaskGetSystemState( pxTaskStatus, arraySize, ( configRUN_TIME_COUNTER_TYPE * ) &ulTaskStatusRunTimeCounter );
    TEST_ASSERT_EQUAL( pdFALSE, xResult );
}

void test_uxTaskGetSystemState_unprivileged_inaccessibleStatusArrayAndRuntime( void )
{
    UBaseType_t arraySize = 5U;
    uint32_t arrayByteSize = arraySize * sizeof( TaskStatus_t );

    unprivilegedTask_raisesAndLowersPrivilege();
    xPortIsAuthorizedToAccessBuffer_ExpectAndReturn( pxTaskStatus, arrayByteSize, tskMPU_WRITE_PERMISSION, pdFALSE );
    xPortIsAuthorizedToAccessBuffer_ExpectAndReturn( ( configRUN_TIME_COUNTER_TYPE * ) &ulTaskStatusRunTimeCounter, sizeof( configRUN_TIME_COUNTER_TYPE ), tskMPU_WRITE_PERMISSION, pdFALSE );

    xResult = MPU_uxTaskGetSystemState( pxTaskStatus, arraySize, ( configRUN_TIME_COUNTER_TYPE * ) &ulTaskStatusRunTimeCounter );
    TEST_ASSERT_EQUAL( pdFALSE, xResult );
}

void test_uxTaskGetSystemState_privileged( void )
{
    UBaseType_t arraySize = 4U;

    privilegedTask_retainsPrivilege();

    uxTaskGetSystemState_ExpectAndReturn( pxTaskStatus, arraySize, ( configRUN_TIME_COUNTER_TYPE * ) NULL, pdTRUE );

    xResult = MPU_uxTaskGetSystemState( pxTaskStatus, arraySize, ( configRUN_TIME_COUNTER_TYPE * ) NULL );
    TEST_ASSERT_EQUAL( pdTRUE, xResult );
}

void test_vTaskSetTimeOutState_unprivileged_accessibleBuffer( void )
{
    unprivilegedTask_raisesAndLowersPrivilege();
    xPortIsAuthorizedToAccessBuffer_ExpectAndReturn( pxTimeOut, sizeof( TimeOut_t ), tskMPU_WRITE_PERMISSION, pdTRUE );
    vTaskSetTimeOutState_Ignore();

    MPU_vTaskSetTimeOutState( pxTimeOut );
}

void test_vTaskSetTimeOutState_unprivileged_inaccessibleBuffer( void )
{
    unprivilegedTask_raisesAndLowersPrivilege();
    xPortIsAuthorizedToAccessBuffer_ExpectAndReturn( pxTimeOut, sizeof( TimeOut_t ), tskMPU_WRITE_PERMISSION, pdFALSE );

    MPU_vTaskSetTimeOutState( pxTimeOut );
}

void test_vTaskSetTimeOutState_privileged( void )
{
    privilegedTask_retainsPrivilege();
    vTaskSetTimeOutState_Ignore();

    MPU_vTaskSetTimeOutState( pxTimeOut );
}

void test_xTaskCheckForTimeOut_unprivileged_accessibleTickAndTimeOut( void )
{
    unprivilegedTask_raisesAndLowersPrivilege();
    xPortIsAuthorizedToAccessBuffer_ExpectAndReturn( pxTimeOut, sizeof( TimeOut_t ), tskMPU_WRITE_PERMISSION, pdTRUE );
    xPortIsAuthorizedToAccessBuffer_ExpectAndReturn( ( TickType_t * const ) &xSingleTick, sizeof( TickType_t ), tskMPU_WRITE_PERMISSION, pdTRUE );
    xTaskCheckForTimeOut_ExpectAndReturn( pxTimeOut, ( TickType_t * const ) &xSingleTick, pdTRUE );

    xResult = MPU_xTaskCheckForTimeOut( pxTimeOut, ( TickType_t * const ) &xSingleTick );
    TEST_ASSERT_EQUAL( pdTRUE, xResult );
}

void test_xTaskCheckForTimeOut_unprivileged_inaccessibleTick( void )
{
    unprivilegedTask_raisesAndLowersPrivilege();
    xPortIsAuthorizedToAccessBuffer_ExpectAndReturn( pxTimeOut, sizeof( TimeOut_t ), tskMPU_WRITE_PERMISSION, pdTRUE );
    xPortIsAuthorizedToAccessBuffer_ExpectAndReturn( ( TickType_t * const ) &xSingleTick, sizeof( TickType_t ), tskMPU_WRITE_PERMISSION, pdFALSE );

    xResult = MPU_xTaskCheckForTimeOut( pxTimeOut, ( TickType_t * const ) &xSingleTick );
    TEST_ASSERT_EQUAL( pdFALSE, xResult );
}

void test_xTaskCheckForTimeOut_unprivileged_inaccessibleTimeOut( void )
{
    unprivilegedTask_raisesAndLowersPrivilege();
    xPortIsAuthorizedToAccessBuffer_ExpectAndReturn( pxTimeOut, sizeof( TimeOut_t ), tskMPU_WRITE_PERMISSION, pdFALSE );
    xPortIsAuthorizedToAccessBuffer_ExpectAndReturn( ( TickType_t * const ) &xSingleTick, sizeof( TickType_t ), tskMPU_WRITE_PERMISSION, pdTRUE );

    xResult = MPU_xTaskCheckForTimeOut( pxTimeOut, ( TickType_t * const ) &xSingleTick );
    TEST_ASSERT_EQUAL( pdFALSE, xResult );
}

void test_xTaskCheckForTimeOut_unprivileged_inaccessibleTickAndTimeOut( void )
{
    unprivilegedTask_raisesAndLowersPrivilege();
    xPortIsAuthorizedToAccessBuffer_ExpectAndReturn( pxTimeOut, sizeof( TimeOut_t ), tskMPU_WRITE_PERMISSION, pdFALSE );
    xPortIsAuthorizedToAccessBuffer_ExpectAndReturn( ( TickType_t * const ) &xSingleTick, sizeof( TickType_t ), tskMPU_WRITE_PERMISSION, pdFALSE );

    xResult = MPU_xTaskCheckForTimeOut( pxTimeOut, ( TickType_t * const ) &xSingleTick );
    TEST_ASSERT_EQUAL( pdFALSE, xResult );
}

void test_xTaskCheckForTimeOut_privileged( void )
{
    privilegedTask_retainsPrivilege();
    xTaskCheckForTimeOut_ExpectAndReturn( pxTimeOut, ( TickType_t * const ) &xSingleTick, pdTRUE );

    xResult = MPU_xTaskCheckForTimeOut( pxTimeOut, ( TickType_t * const ) &xSingleTick );
    TEST_ASSERT_EQUAL( pdTRUE, xResult );
}

void test_xTaskGenericNotify_unprivileged_accessibleBuffer( void )
{
    unprivilegedTask_raisesAndLowersPrivilege();
    xPortIsAuthorizedToAccessBuffer_ExpectAndReturn( &ulNotificationValue, sizeof( uint32_t ), tskMPU_WRITE_PERMISSION, pdTRUE );
    xTaskGenericNotify_ExpectAndReturn( xHandle, 1U, 1U, eSetBits, &ulNotificationValue, pdTRUE );

    xResult = MPU_xTaskGenericNotify( xHandle, 1U, 1U, eSetBits, &ulNotificationValue );
    TEST_ASSERT_EQUAL( pdTRUE, xResult );
}

void test_xTaskGenericNotify_unprivileged_nullNotificationValue( void )
{
    uint32_t * pulNotificationValue = NULL;

    unprivilegedTask_raisesAndLowersPrivilege();
    xTaskGenericNotify_ExpectAndReturn( xHandle, 1U, 1U, eSetBits, pulNotificationValue, pdTRUE );

    xResult = MPU_xTaskGenericNotify( xHandle, 1U, 1U, eSetBits, pulNotificationValue );
    TEST_ASSERT_EQUAL( pdTRUE, xResult );
}

void test_xTaskGenericNotify_unprivileged_inaccessibleBuffer( void )
{
    unprivilegedTask_raisesAndLowersPrivilege();
    xPortIsAuthorizedToAccessBuffer_ExpectAndReturn( &ulNotificationValue, sizeof( uint32_t ), tskMPU_WRITE_PERMISSION, pdFALSE );

    xResult = MPU_xTaskGenericNotify( xHandle, 1U, 1U, eSetBits, &ulNotificationValue );
    TEST_ASSERT_EQUAL( pdFALSE, xResult );
}

void test_xTaskGenericNotify_privileged( void )
{
    privilegedTask_retainsPrivilege();
    xTaskGenericNotify_ExpectAndReturn( xHandle, 2U, 2U, eSetValueWithoutOverwrite, &ulNotificationValue, pdTRUE );

    xResult = MPU_xTaskGenericNotify( xHandle, 2U, 2U, eSetValueWithoutOverwrite, &ulNotificationValue );
    TEST_ASSERT_EQUAL( pdTRUE, xResult );
}

void test_xTaskGenericNotifyWait_unprivileged_accessibleBuffer( void )
{
    unprivilegedTask_raisesAndLowersPrivilege();
    xPortIsAuthorizedToAccessBuffer_ExpectAndReturn( &ulNotificationValue, sizeof( uint32_t ), tskMPU_WRITE_PERMISSION, pdTRUE );
    xTaskGenericNotifyWait_ExpectAndReturn( 1U, 2U, 3U, &ulNotificationValue, xSingleTick, pdTRUE );

    xResult = MPU_xTaskGenericNotifyWait( 1U, 2U, 3U, &ulNotificationValue, xSingleTick );
    TEST_ASSERT_EQUAL( pdTRUE, xResult );
}

void test_xTaskGenericNotifyWait_unprivileged_nullNotificationValue( void )
{
    uint32_t * pulNotificationValue = NULL;

    unprivilegedTask_raisesAndLowersPrivilege();
    xTaskGenericNotifyWait_ExpectAndReturn( 1U, 2U, 3U, pulNotificationValue, xSingleTick, pdTRUE );

    xResult = MPU_xTaskGenericNotifyWait( 1U, 2U, 3U, pulNotificationValue, xSingleTick );
    TEST_ASSERT_EQUAL( pdTRUE, xResult );
}

void test_xTaskGenericNotifyWait_unprivileged_inaccessibleBuffer( void )
{
    unprivilegedTask_raisesAndLowersPrivilege();
    xPortIsAuthorizedToAccessBuffer_ExpectAndReturn( &ulNotificationValue, sizeof( uint32_t ), tskMPU_WRITE_PERMISSION, pdFALSE );

    xResult = MPU_xTaskGenericNotifyWait( 1U, 2U, 3U, &ulNotificationValue, xSingleTick );
    TEST_ASSERT_EQUAL( pdFALSE, xResult );
}

void test_xTaskGenericNotifyWait_privileged( void )
{
    privilegedTask_retainsPrivilege();
    xTaskGenericNotifyWait_ExpectAndReturn( 1U, 2U, 3U, &ulNotificationValue, xSingleTick, pdTRUE );

    xResult = MPU_xTaskGenericNotifyWait( 1U, 2U, 3U, &ulNotificationValue, xSingleTick );
    TEST_ASSERT_EQUAL( pdTRUE, xResult );
}
