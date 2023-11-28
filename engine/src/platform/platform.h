/**
 * @file platform.h
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Platform interface.
 */
#ifndef PLATFORM_H
#define PLATFORM_H

#include "common.h"

/**
 * @brief Initializes the platform subsystem. Call once to read the memory
 * requirement. Call again passing in a state pointer.
 * @param memory_requirement Output buffer to read memory requirement.
 * @param state Pass 0 to read memory requirement. Otherwise, pass a buffer.
 * @param wm_title The title to assign the application window.
 * @param wm_class The class name to assign the application window.
 * @param wm_x The x coordinate to assign the application window.
 * @param wm_y The y coordinate to assign the application window.
 * @param wm_w The width to assign the application window.
 * @param wm_h The height to assign the application window.
 * @return false on error, true otherwise.
 */
bool
platform_startup
(   u64*        memory_requirement
,   void*       state
);

/**
 * @brief Terminates the platform subsystem.
 * @param state The platform state to mutate.
 */
void
platform_shutdown
(   void* state
);

/**
 * @brief Platform-independent block allocation function.
 * @param size The number of bytes to allocate.
 * @param aligned tbd.
 */
void*
platform_memory_allocate
(   u64         size
,   const bool  aligned
);

/**
 * @brief Platform-independent block free function.
 * @param blk The block to free.
 * @param aligned tbd.
 */
void
platform_memory_free
(   void*       blk
,   const bool  aligned
);

/**
 * @brief Platform-independent block clear function.
 * @param blk The block to initialize.
 * @param size The number of bytes to set.
 * @return blk.
 */
void*
platform_memory_clear
(   void*   blk
,   u64     size
);

/**
 * @brief Platform-independent block set function.
 * @param blk The block to set.
 * @param value The value to set.
 * @param size The number of bytes to set.
 * @return blk.
 */
void*
platform_memory_set
(   void*       blk
,   const i32   value
,   u64         size
);

/**
 * @brief Platform-independent block copy function.
 * @param dst The destination block.
 * @param src The source block.
 * @param size The number of bytes to copy.
 * @return dst.
 */
void*
platform_memory_copy
(   void*       dst
,   const void* src
,   u64         size
);

/**
 * @brief Platform-independent block move function.
 * @param dst The destination block.
 * @param src The source block.
 * @param size The number of bytes to move.
 * @return dst.
 */
void*
platform_memory_move
(   void*       dst
,   const void* src
,   u64         size
);

/**
 * @brief Platform-independent console write function.
 * @param mesg The message to write.
 * @param color The message color code index.
 */
void
platform_console_write
(   const char* mesg
,   u8          color
);

/**
 * @brief Platform-independent console write error function.
 * @param mesg The error message to write.
 * @param color The message color code index.
 */
void
platform_console_write_error
(   const char* mesg
,   u8          color
);

/**
 * @brief Platform-independent get absolute time function.
 */
f64
platform_get_absolute_time
( void );

/**
 * @brief Platform-independent sleep function.
 * @param ms The number of ms to sleep on the current thread for.
 */
void
platform_sleep
(   u64 ms
);

#endif  // PLATFORM_H
