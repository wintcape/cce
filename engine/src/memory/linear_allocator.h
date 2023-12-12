/**
 * @file linear_allocator.h
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Linear memory allocation subsystem.
 */
#ifndef MEMORY_LINEAR_ALLOCATOR_H
#define MEMORY_LINEAR_ALLOCATOR_H

#include "common.h"

// Type definition.
typedef struct
{
    u64     cap;
    u64     allocated;
    void*   memory;
    bool    owns_memory;
}
linear_allocator_t;

/**
 * @brief Initializes a linear allocator.
 * @param cap The capacity in bytes.
 * @param memory The starting memory index (optional).
 * @param allocator Output buffer.
 */
void
linear_allocator_create
(   u64                 cap
,   void*               memory
,   linear_allocator_t* allocator
);

/**
 * @brief Terminates a dynamic allocator.
 * @param allocator The allocator to terminate.
 */
void
linear_allocator_destroy
(   linear_allocator_t* allocator
);

/**
 * @brief Allocates memory using a linear allocator.
 * @param allocator The allocator.
 * @param size The number of bytes to allocate.
 * @return The address of the allocated memory, on success.
 *         0, on error.
 */
void*
linear_allocator_allocate
(   linear_allocator_t* allocator
,   u64                 size
);

/**
 * @brief Clears all memory managed by a linear allocator.
 * @param allocator The allocator.
 */
void
linear_allocator_free
(   linear_allocator_t* allocator
);

#endif  // MEMORY_LINEAR_ALLOCATOR_H
