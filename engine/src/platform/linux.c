/**
 * @author Matthew Weissel (null@mattweissel.info)
 * @file linux.c
 * @brief Implementation of the platform header for GNU/Linux-based
 * operating systems.
 * (see platform.h for additional details)
 */
#include "platform/platform.h"

#include "core/event.h"
#include "core/logger.h"

#include "container/array.h"

// Begin platform layer.
#if PLATFORM_LINUX == 1

#include <sys/time.h>

#if _POSIX_C_SOURCE >= 199309L
#include <time.h>   // nanosleep
#else
#include <unistd.h> // usleep
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Defines reserved console color codes.
#define PLATFORM_CONSOLE_COLORS \
    {                           \
      "0;41"                    \
    , "0;91"                    \
    , "0;93"                    \
    , "0;32"                    \
    , "0;33"                    \
    , "0;35"                    \
                                \
    , "0;97"                    \
    , "0;37"                    \
    , "0;90"                    \
    , "0;35"                    \
                                \
    , "101;97"                  \
    , "101;30"                  \
    , "41;97"                   \
    , "41;30"                   \
    }

// Type definition for platform subsystem state.
typedef struct 
{
    bool tmp;
}
state_t;

// Global subsystem state.
static state_t* state;

/**
 * @brief Primary implementation of platform_console_write and
 * platform_console_write_error.
 * @param mesg The message to write.
 * @param color The message color code index.
 * @param file The system file to write to.
 */
void
_platform_console_write
(   const char* mesg
,   u8          color
,   FILE*       file
);

bool
platform_startup
(   u64*        memory_requirement
,   void*       state_
)
{
    *memory_requirement = sizeof ( state_t );

    if ( !state_ )
    {
        return true;
    }

    state = state_;
    platform_memory_clear ( state , sizeof ( state_t ) );
    
    return true;
}

void
platform_shutdown
(   void* state_
)
{
    if ( !state )
    {
        return;
    }

    state = 0;
}

void*
platform_memory_allocate
(   u64         size
,   const bool  aligned
)
{
    return malloc ( size );
}

void
platform_memory_free
(   void*       blk
,   const bool  aligned
)
{
    free ( blk );
}

void*
platform_memory_clear
(   void*   blk
,   u64     size
)
{
    return memset ( blk , 0 , size );
}

void*
platform_memory_set
(   void*       blk
,   const i32   value
,   u64         size
)
{
    return memset ( blk , value , size );
}

void*
platform_memory_copy
(   void*       dst
,   const void* src
,   u64         size
)
{
    return memcpy ( dst , src , size );
}

void*
platform_memory_move
(   void*       dst
,   const void* src
,   u64         size
)
{
    return memmove ( dst , src , size );
}

void
platform_console_write
(   const char* mesg
,   u8          color
)
{
    _platform_console_write ( mesg , color , stdout );
}

void
platform_console_write_error
(   const char* mesg
,   u8          color
)
{
    _platform_console_write ( mesg , color , stderr );
}

f64
platform_get_absolute_time
( void )
{
    struct timespec time;
    clock_gettime ( CLOCK_MONOTONIC , &time );
    return time.tv_sec + time.tv_nsec * 0.000000001;
}

void
platform_sleep
(   u64 ms
)
{
#if _POSIX_C_SOURCE >= 199309L
    struct timespec time;
    time.tv_sec = ms / 1000;
    time.tv_nsec = ( ms % 1000 ) * 1000 * 1000;
    nanosleep ( &time , 0 );
#else
    if ( ms >= 1000 )
    {
        sleep ( ms / 1000 );
    }
    usleep ( ( ms % 1000 ) * 1000 );
#endif
}

void
_platform_console_write
(   const char* mesg
,   u8          color
,   FILE*       file
)
{
    const char* colors[] = PLATFORM_CONSOLE_COLORS;
    fprintf ( file , "\033[%sm%s\033[0m" , colors[ color ] , mesg );
}

#endif  // End platform layer.
