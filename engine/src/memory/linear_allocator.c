/*
 * @author Matthew Weissel (null@mattweissel.info)
 * @file linear_allocator.c
 * @brief Implementation of the linear allocator header.
 * (see linear_allocator.h for additional details)
 */

#include "memory/linear_allocator.h"

#include "core/logger.h"
#include "core/memory.h"
#include "core/string.h"

void
linear_allocator_startup
(   u64                 cap
,   void*               memory
,   linear_allocator_t* allocator
)
{
    if ( !allocator )
    {
        LOGERROR ( "linear_allocator_init: Missing argument: allocator."
                 , allocator
                 );
        return;
    }

    const bool owns_memory = !memory;

    ( *allocator ).cap = cap;
    ( *allocator ).allocated = 0;
    ( *allocator ).owns_memory = owns_memory;
    
    if ( memory )
    {
        ( *allocator ).memory = memory;
    }
    else
    {
        ( *allocator ).memory = memory_allocate ( cap
                                                , MEMORY_TAG_LINEAR_ALLOCATOR
                                                );
    }
}

void
linear_allocator_shutdown
(   linear_allocator_t* allocator
)
{
    if ( !allocator )
    {
        return;
    }

    if ( ( *allocator ).owns_memory && ( *allocator ).memory )
    {
        memory_free ( ( *allocator ).memory
                    , ( *allocator ).cap
                    , MEMORY_TAG_LINEAR_ALLOCATOR
                    );
    }

    ( *allocator ).memory = 0;
    ( *allocator ).cap = 0;
    ( *allocator ).allocated = 0;
    ( *allocator ).owns_memory = false;
}

void*
linear_allocator_allocate
(   linear_allocator_t* allocator
,   u64                 size
)
{
    if ( !allocator || !( *allocator ).memory )
    {
        LOGERROR ( "linear_allocator_allocate: The provided allocator is uninitialized." );
        return 0;
    }

    if ( ( *allocator ).allocated + size > ( *allocator ).cap )
    {
        f32 req_amt;
        f32 rem_amt;
        const char* req_unit = string_bytesize ( ( *allocator ).cap - ( *allocator ).allocated
                                               , &req_amt
                                               );
        const char* rem_unit = string_bytesize ( ( *allocator ).cap - ( *allocator ).allocated
                                               , &rem_amt
                                               );
        LOGERROR ( "linear_allocator_allocate: Cannot allocate %.2f %s, only %.2f %s remaining."
                 , req_amt , req_unit
                 , rem_amt , rem_unit
                 );
        return 0;
    }

    void* blk = ( *allocator ).memory + ( *allocator ).allocated;
    ( *allocator ).allocated += size;
    return blk;
}

void
linear_allocator_free
(   linear_allocator_t* allocator
)
{
    if ( !allocator || !( *allocator ).memory )
    {
        LOGERROR ( "linear_allocator_reset: The provided allocator is uninitialized." );
        return;
    }
    
    ( *allocator ).allocated = 0;
    memory_clear ( ( *allocator ).memory , ( *allocator ).cap );
}
