/**
 * @author Matthew Weissel (null@mattweissel.info)
 * @file memory.c
 * @brief Implementation of the memory header.
 * (see memory.h for additional details)
 */
#include "core/memory.h"

#include "core/logger.h"
#include "core/string.h"

#include "memory/dynamic_allocator.h"

#include "platform/platform.h"

// Defaults.
#define MEMORY_STAT_STRING_MAX_LENGTH 1024

// Defines an aligned string representation of each memory tag.
static const char* memory_tags[ MEMORY_TAG_COUNT ] = { "UNKNOWN          "
                                                     , "ARRAY            "
                                                     , "DYNAMIC ARRAY    "
                                                     , "LINEAR ALLOCATOR "
                                                     , "STRING           "
                                                     , "ENGINE           "
                                                     , "APPLICATION      "
                                                     };

// Type definition for a container to hold global statistics.
typedef struct
{
    u64     allocation_count;
    u64     free_count;

    u64     allocated;
    u64     tagged_allocations[ MEMORY_TAG_COUNT ];

    char    string[ MEMORY_STAT_STRING_MAX_LENGTH ];
    u64     string_length;
}
stat_t;

// Type definition for memory subsystem state.
typedef struct
{
    stat_t              stat;
    
    u64                 allocator_cap;
    dynamic_allocator_t allocator;
    void*               allocator_start;

    u64                 cap;
    void*               memory;
}
state_t;

// Memory subsystem state.
static state_t* state;

bool
memory_startup
(   u64 cap
)
{
    u64 state_memory_requirement = sizeof ( state_t );

    u64 allocator_memory_requirement = 0;
    dynamic_allocator_startup ( cap , &allocator_memory_requirement , 0 , 0 );

    void* memory = platform_memory_allocate (   state_memory_requirement
                                              + allocator_memory_requirement
                                            , true
                                            );
    if ( !memory )
    {
        LOGFATAL ( "The host platform failed to allocate memory to run the application." );
        return false;
    }

    state = memory;
    ( *state ).allocator_cap = allocator_memory_requirement;
    ( *state ).allocator_start = memory + state_memory_requirement;
    ( *state ).cap = cap;
    platform_memory_clear ( &( *state ).stat , sizeof ( stat_t ) );

    if ( !dynamic_allocator_startup ( cap
                                    , &( *state ).allocator_cap
                                    , ( *state ).allocator_start
                                    , &( *state ).allocator
                                    ))
    {
        LOGFATAL ( "Memory subsystem failed to initialize internal allocator." );
        return false;
    }

    f32 amt = 0.0f;
    const char* unit = string_bytesize ( cap , &amt );
    LOGDEBUG ( "Successfully allocated %.2f %s of host platform memory to run the application."
             , amt , unit
             );

    return true;
}

void
memory_shutdown
( void )
{
    if ( !state )
    {
        return;
    }

    dynamic_allocator_shutdown ( &( *state ).allocator );

    if ( ( *state ).stat.allocation_count != ( *state ).stat.free_count )
    {
        LOGWARN ( "Noticed allocation count (%llu) != free count (%llu) when shutting down memory subsystem."
                , ( *state ).stat.allocation_count
                , ( *state ).stat.free_count
                );
    }
    
    platform_memory_free ( state , ( *state ).allocator_cap + sizeof ( state_t ) );

    state = 0;
}

void*
memory_allocate
(   u64         size
,   MEMORY_TAG  tag
)
{
    return memory_allocate_aligned ( size , 1 , tag );
}

void*
memory_allocate_aligned
(   u64         size
,   u16         alignment
,   MEMORY_TAG  tag
)
{
    // Warn if tag unknown.
    if ( tag == MEMORY_TAG_UNKNOWN )
    {
        LOGWARN ( "memory_allocate: called with MEMORY_TAG_UNKNOWN." );
    }

    if ( !state )
    {
        LOGTRACE ( "memory_allocate: called before the memory subsystem was initialized." );
        return 0;
    }

    // Perform the memory allocation.
    void* memory = dynamic_allocator_allocate_aligned ( &( *state ).allocator
                                                      , size
                                                      , alignment
                                                      );
    if ( !memory )
    {
        LOGFATAL ( "memory_allocate: Failed to allocate memory." );
        return 0;
    }
    
    platform_memory_clear ( memory , size );

    // Update statistics.
    ( *state ).stat.allocated += size;
    ( *state ).stat.tagged_allocations[ tag ] += size;
    ( *state ).stat.allocation_count += 1;

    return memory;
}

void
memory_free
(   void*       memory
,   u64         size
,   MEMORY_TAG  tag
)
{
    memory_free_aligned ( memory , size , 1 , tag );
}

void
memory_free_aligned
(   void*       memory
,   u64         size
,   u16         alignment
,   MEMORY_TAG  tag
)
{
    // Warn if tag unknown.
    if ( tag == MEMORY_TAG_UNKNOWN )
    {
        LOGWARN ( "memory_free: called with MEMORY_TAG_UNKNOWN." );
    }

    if ( !state )
    {
        LOGTRACE ( "memory_free: called before the memory subsystem was initialized." );
        return;
    }    

    // Perform the memory free.
    if ( dynamic_allocator_free_aligned ( &( *state ).allocator
                                        , memory
                                        ))
    {
        // Update statistics.
        ( *state ).stat.allocated -= size;
        ( *state ).stat.tagged_allocations[ tag ] -= size;
        ( *state ).stat.free_count += 1;
    }
    else
    {
        /**
         * If the call failed, try freeing the memory on the platform level as
         * a failsafe.
         */
        platform_memory_free ( memory , false );
    }
    
    //. . .if that didn't work either, you're fucked.
}

void*
memory_clear
(   void*   memory
,   u64     size
)
{
    return platform_memory_clear ( memory , size );
}

void*
memory_set
(   void*   memory
,   i32     value
,   u64     size
)
{
    return platform_memory_set ( memory , value , size );
}

void*
memory_copy
(   void*       dst
,   const void* src
,   u64         size
)
{
    return platform_memory_copy ( dst , src , size );    
}

char*
memory_stat
( void )
{
    if ( !state )
    {
        return 0;
    }

    const char* unit;
    f32 amt;
    u64 offs = string_format ( ( *state ).stat.string
                             , "System memory usage:\n"
                             );

    for ( u32 i = 0; i < MEMORY_TAG_COUNT; ++i )
    {
        unit = string_bytesize ( ( *state ).stat.tagged_allocations[ i ]
                               , &amt
                               );
        offs += string_format ( ( *state ).stat.string + offs
                              , "\t  %s: %.2f %s\n"
                              , memory_tags[ i ] , amt , unit
                              );
    }

    unit = string_bytesize ( ( *state ).stat.allocated
                           , &amt
                           );
    offs += string_format ( ( *state ).stat.string + offs
                          , "\t  ------------------------------\n"
                            "\t  TOTAL            : %.2f %s\n"
                          , amt , unit
                          );
    unit = string_bytesize ( ( *state ).cap
                           , &amt
                           );
    offs += string_format ( ( *state ).stat.string + offs
                          , "\t                    (%.2f %s reserved)"
                          , amt , unit
                          );
   
    ( *state ).stat.string_length = offs;
    return ( *state ).stat.string;
}

u64
memory_allocation_count
( void )
{
    if ( state )
    {
        return ( *state ).stat.allocation_count;
    }
    return 0;
}

u64
memory_free_count
( void )
{
    if ( state )
    {
        return ( *state ).stat.free_count;
    }
    return 0;
}
