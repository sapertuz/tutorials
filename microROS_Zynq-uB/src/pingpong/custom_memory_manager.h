#ifndef __CUSTOM_MEMORY_MNGR__
#define __CUSTOM_MEMORY_MNGR__

/*
 * A custom implementation of pvPortMallocMicroROS() and vPortFreeMicroROS() with realloc and
 * calloc features based on FreeRTOS heap4.c.
 */

#include <stdlib.h>
#include <string.h>

/* Defining MPU_WRAPPERS_INCLUDED_FROM_API_FILE prevents task.h from redefining
all the API functions to use the MPU wrappers.  That should only be done when
task.h is included from an application file. */
#define MPU_WRAPPERS_INCLUDED_FROM_API_FILE

#include "FreeRTOS.h"
#include "task.h"

#undef MPU_WRAPPERS_INCLUDED_FROM_API_FILE

#if( configSUPPORT_DYNAMIC_ALLOCATION == 0 )
	#error This file must not be used if configSUPPORT_DYNAMIC_ALLOCATION is 0
#endif

/* Block sizes must not get too small. */
#define heapMINIMUM_BLOCK_SIZE	( ( size_t ) ( xHeapStructSize << 1 ) )

/* Assumes 8bit bytes! */
#define heapBITS_PER_BYTE		( ( size_t ) 8 )

/* Allocate the memory for the heap. */
static uint8_t ucHeap[ configTOTAL_HEAP_SIZE ];

/* Define the linked list structure.  This is used to link free blocks in order
of their memory address. */
typedef struct A_BLOCK_LINK
{
	struct A_BLOCK_LINK *pxNextFreeBlock;	/*<< The next free block in the list. */
	size_t xBlockSize;						/*<< The size of the free block. */
} BlockLink_t;

/*-----------------------------------------------------------*/

void *pvPortMallocMicroROS( size_t xWantedSize );
/*-----------------------------------------------------------*/

void vPortFreeMicroROS( void *pv );
/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/
size_t getBlockSize( void *pv );
/*-----------------------------------------------------------*/

void *pvPortReallocMicroROS( void *pv, size_t xWantedSize );
/*-----------------------------------------------------------*/

void *pvPortCallocMicroROS( size_t num, size_t xWantedSize );
/*-----------------------------------------------------------*/

size_t xPortGetFreeHeapSizeMicroROS( void );
/*-----------------------------------------------------------*/

size_t xPortGetMinimumEverFreeHeapSizeMicroROS( void );
/*-----------------------------------------------------------*/

void vPortInitialiseBlocksMicroROS( void );
/*-----------------------------------------------------------*/

static void prvHeapInit( void );
/*-----------------------------------------------------------*/

static void prvInsertBlockIntoFreeList( BlockLink_t *pxBlockToInsert );

#endif