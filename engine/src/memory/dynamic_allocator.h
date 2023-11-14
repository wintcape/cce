/**
 * @file dynamic_allocator.h
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Linear memory allocation subsystem.
 */
#ifndef MEMORY_DYNAMIC_ALLOCATOR_H
#define MEMORY_DYNAMIC_ALLOCATOR_H

#include "common.h"

// Type definition.
typedef struct
{
    void* memory;
}
dynamic_allocator_t;

/**
 * @brief Initializes a dynamic allocator. Call once to get the memory
 * requirement, call a second time passing in a valid memory buffer.
 * @param cap The capacity in bytes.
 * @param memory_requirement The number of bytes required to operate a dynamic
 * allocator.
 * @param memory The starting memory index (optional).
 * @param allocator Output buffer.
 * @return true on success, false otherwise.
 */
bool
dynamic_allocator_startup
(   u64                     cap
,   u64*                    memory_requirement
,   void*                   memory
,   dynamic_allocator_t*    allocator
);

/**
 * @brief Terminates a dynamic allocator.
 * @param allocator The allocator to terminate.
 * @return true on success, false otherwise.
 */
bool
dynamic_allocator_shutdown
(   dynamic_allocator_t* allocator
);

/**
 * @brief Allocates memory using a dynamic allocator.
 * @param allocator The allocator.
 * @param size The number of bytes to allocate.
 * @return The address of the allocated memory, on success.
 *         0, on error.
 */
void*
dynamic_allocator_allocate
(   dynamic_allocator_t*    allocator
,   u64                     size
);

/**
 * @brief Allocates memory using a dynamic allocator.
 * @param allocator The allocator.
 * @param size The number of bytes to allocate.
 * @return The address of the allocated memory, on success.
 *         0, on error.
 */
void*
dynamic_allocator_allocate_aligned
(   dynamic_allocator_t*    allocator
,   u64                     size
,   u16                     alignment
);

/**
 * @brief Frees a single block of memory previously allocated by a dynamic
 * allocator.
 * @param allocator The allocator.
 * @param blk The memory block to free.
 * @return true on success, false otherwise.
 */
bool
dynamic_allocator_free
(   dynamic_allocator_t*    allocator
,   void*                   blk
);

/**
 * @brief Frees a single block of memory previously allocated by a dynamic
 * allocator.
 * @param allocator The allocator.
 * @param blk The memory block to free.
 * @return true on success, false otherwise.
 */
bool
dynamic_allocator_free_aligned
(   dynamic_allocator_t*    allocator
,   void*                   blk
);

#endif  // MEMORY_DYNAMIC_ALLOCATOR_H
