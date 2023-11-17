#ifndef __MICROROS_TIME_H__
#define __MICROROS_TIME_H__

#include <unistd.h>
#include <time.h>
#include "FreeRTOS.h"
#include "task.h"

#define MICROSECONDS_PER_SECOND    ( 1000000LL )                                   /**< Microseconds per second. */
#define NANOSECONDS_PER_SECOND     ( 1000000000LL )                                /**< Nanoseconds per second. */
#define NANOSECONDS_PER_TICK       ( NANOSECONDS_PER_SECOND / configTICK_RATE_HZ ) /**< Nanoseconds per FreeRTOS tick. */

void UTILS_NanosecondsToTimespec( int64_t llSource,
                                  struct timespec * const pxDestination );

int clock_gettime( int clock_id,
                   struct timespec * tp );

#endif