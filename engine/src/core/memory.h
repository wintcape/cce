/**
 * @file memory.h
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Engine subsystem for memory allocation and management.
 */
#ifndef MEMORY_H
#define MEMORY_H

#include "common.h"

// Type definition for memory tag.
typedef enum
{
    MEMORY_TAG_UNKNOWN
,   MEMORY_TAG_ARRAY
,   MEMORY_TAG_DYNAMIC_ARRAY
,   MEMORY_TAG_LINEAR_ALLOCATOR
,   MEMORY_TAG_STRING
,   MEMORY_TAG_ENGINE
,   MEMORY_TAG_APPLICATION
,   MEMORY_TAG_COUNT
}
MEMORY_TAG;

/**
 * @brief Initializes the memory subsystem.
 * @param cap The total amount of memory to request from the host platform.
 * @return true if initialization successful, false otherwise.
 */
bool
memory_startup
(   u64 cap
);

/**
 * @brief Terminates the memory subsystem.
 */
void
memory_shutdown
( void );

/**
 * @brief Allocates a block of memory.
 * @param size The number of bytes to allocate.
 * @param tag The block tag.
 * @return The allocated block.
 */
void*
memory_allocate
(   u64         size
,   MEMORY_TAG  tag
);

/**
 * @brief Allocates a block of memory.
 * @param size The number of bytes to allocate.
 * @param alignment .
 * @param tag The block tag.
 * @return The allocated block.
 */
void*
memory_allocate_aligned
(   u64         size
,   u16         alignment
,   MEMORY_TAG  tag
);

/**
 * @brief Frees a block of memory.
 * @param memory The block to free.
 * @param size The block size in bytes.
 * @param tag The block tag.
 */
void
memory_free
(   void*       memory
,   u64         size
,   MEMORY_TAG  tag
);

/**
 * @brief Frees a block of memory.
 * @param memory The block to free.
 * @param size The block size in bytes.
 * @param alignment .
 * @param tag The block tag.
 */
void
memory_free_aligned
(   void*       memory
,   u64         size
,   u16         alignment
,   MEMORY_TAG  tag
);

/**
 * @brief Clears a block of memory.
 * @param memory The block to clear.
 * @param size The block size in bytes.
 * @return memory.
 */
void*
memory_clear
(   void*   memory
,   u64     size
);

/**
 * @brief Sets a block of memory.
 * @param memory The block to set.
 * @param size The block size in bytes.
 * @return memory.
 */
void*
memory_set
(   void*   dst
,   i32     value
,   u64     size
);

/**
 * @brief Copies a specified number of bytes of memory from a source block to a
 * destination block. The size of both blocks should be adequate for the
 * specified size parameter.
 * @param dst The destination block.
 * @param src The source block.
 * @param size The number of bytes to copy.
 * @return dst.
 */
void*
memory_copy
(   void*       dst
,   const void* src
,   u64         size
);

/**
 * @brief Stringify utility. Uses memory allocator; call string_free to free.
 * @return A string representation of memory usage statistics.
 */
char*
memory_stat
( void );

/**
 * @brief Queries the global allocation count.
 * @return The global allocation count.
 */
u64
memory_allocation_count
( void );

/**
 * @brief Queries the global free count.
 * @return The global free count.
 */
u64
memory_free_count
( void );

#endif  // MEMORY_H
