#include "microros_allocators.h"
#include "FreeRTOS.h"
#include <string.h>

static uint8_t heap[configTOTAL_HEAP_SIZE];
static size_t current_pointer = 0;

void free_all_heap()
{
    current_pointer = 0;
}

void assert_position()
{
    if (current_pointer >= sizeof(heap)) {
        // Handle memory error
        while(1){};
    }
}

#define SYSTEM_ALIGNMENT 4

size_t align_size(size_t size)
{
    if (size % SYSTEM_ALIGNMENT != 0) {
        size += SYSTEM_ALIGNMENT - (size % SYSTEM_ALIGNMENT);
    }
    return size;
}

void * custom_allocate(size_t size, void * state)
{
    size = align_size(size);
    size_t p = current_pointer;
    current_pointer += size;
    assert_position();
    return (void *) &heap[p];
}

void custom_deallocate(void * pointer, void * state)
{
    (void) state;
    (void) pointer;
}

void * custom_reallocate(void * pointer, size_t size, void * state)
{
    size = align_size(size);
    size_t p = current_pointer;
    current_pointer += size;
    // Careful! pointer may have less than size memory, garbage can be copied!
    memcpy(&heap[p], pointer, size);
    assert_position();
    return (void *) &heap[p];
}

void * custom_zero_allocate(size_t number_of_elements, size_t size_of_element, void * state)
{
    size_t size = number_of_elements * size_of_element;
    size = align_size(size);
    size_t p = current_pointer;
    current_pointer += size;
    memset(&heap[p], 0, size);
    assert_position();
    return (void *) &heap[p];
}
