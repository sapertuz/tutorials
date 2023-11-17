#ifndef __MICROROS_ALLOCATORS__
#define __MICROROS_ALLOCATORS__

#include <stdlib.h>
#include <stdio.h>
#include "FreeRTOS.h"

#define SYSTEM_ALIGNMENT 4

void free_all_heap();
void assert_position();
size_t align_size(size_t size);

void * custom_allocate(size_t size, void * state);
void custom_deallocate(void * pointer, void * state);
void * custom_reallocate(void * pointer, size_t size, void * state);
void * custom_zero_allocate(size_t number_of_elements, size_t size_of_element, void * state);

#endif
