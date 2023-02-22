/*
 * Defines the memory ranges allocated to the task when an MPU is used.
 */

/*
 * typedef struct xMEMORY_REGION
 * {
 *  void * pvBaseAddress;
 *  uint32_t ulLengthInBytes;
 *  uint32_t ulParameters;
 * } MemoryRegion_t;
 */
#include "task.h"

struct xMEMORY_REGION;
void vPortStoreTaskMPUSettings( xMPU_SETTINGS * xMPUSettings,
                                const struct xMEMORY_REGION * const xRegions,
                                StackType_t * pxBottomOfStack,
                                uint32_t ulStackDepth );

BaseType_t xPortStartScheduler( void );

void vPortEndScheduler( void );


/*
 * StackType_t * pxPortInitialiseStack( StackType_t * pxTopOfStack,
 *                                   TaskFunction_t pxCode,
 *                                   void * pvParameters,
 *                                   BaseType_t xRunPrivileged );
 */

StackType_t * pxPortInitialiseStack( StackType_t * pxTopOfStack,
                                     void ( * pxCode )( void * ),
                                     void * xRunPrivileged );

BaseType_t xPortIsAuthorizedToAccessBuffer( const void * pvBuffer,
                                            uint32_t ulBufferLength,
                                            uint32_t ulAccessRequested );
