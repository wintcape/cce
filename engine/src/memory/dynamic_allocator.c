/**
 * @author Matthew Weissel (null@mattweissel.info)
 * @file dynamic_allocator.c
 * @brief Implementation of the dynamic allocator header.
 * (see dynamic_allocator.h for additional details)
 */

#include "memory/dynamic_allocator.h"

#include "core/assert.h"
#include "core/logger.h"
#include "core/memory.h"
#include "core/string.h"

#include "container/freelist.h"

// Type definition for internal state.
typedef struct
{
    u64         cap;
    freelist_t  freelist;
    void*       freelist_start;
    void*       memory_start;
}
state_t;

// Type definition for a memory allocation header.
typedef struct
{
    void*   start;
    u16     alignment;
}
header_t;

// Storage size (in bytes) of a node's user memory block.
#define SIZE_STORAGE                ( sizeof ( u32 ) )

// Maximum single allocation size.
#define MAX_SINGLE_ALLOCATION_SIZE  ( GIBIBYTES ( 4 ) )

bool
dynamic_allocator_startup
(   u64                     cap
,   u64*                    memory_requirement
,   void*                   memory
,   dynamic_allocator_t*    allocator
)
{
    if ( cap < 1 )
    {
        LOGERROR ( "dynamic_allocator_startup: Attempted to initialize allocator with size 0." );
        return false;
    }

    if ( !memory_requirement )
    {
        LOGERROR ( "dynamic_allocator_startup: Missing argument: memory_requirement." );
        return false;
    }

    u64 freelist_memory_requirement = 0;
    freelist_init ( cap , &freelist_memory_requirement , 0 , 0 );
    
    *memory_requirement = freelist_memory_requirement
                        + sizeof ( state_t )
                        + cap
                        ;

    if ( !memory )
    {
        return true;
    }

    ( *allocator ).memory = memory;
    state_t* state = ( *allocator ).memory;
    ( *state ).cap = cap;
    ( *state ).freelist_start = ( *allocator ).memory + sizeof ( state_t );
    ( *state ).memory_start = ( *state ).freelist_start
                            + freelist_memory_requirement
                            ;

    freelist_init ( cap
                  , &freelist_memory_requirement
                  , ( *state ).freelist_start
                  , &( *state ).freelist
                  );

    memory_clear ( ( *state ).memory_start , cap );

    return true;
}

bool
dynamic_allocator_shutdown
(   dynamic_allocator_t* allocator
)
{
    if ( !allocator )
    {
        LOGWARN ( "dynamic_allocator_shutdown: The provided allocator is uninitialized (0x%p)."
                , allocator
                );
        return false;
    }

    state_t* state = ( *allocator ).memory;
    freelist_free ( &( *state ).freelist );
    memory_clear ( ( *state ).memory_start , ( *state ).cap );
    ( *state ).cap = 0;
    ( *allocator ).memory = 0;

    return true;
}

void*
dynamic_allocator_allocate
(   dynamic_allocator_t*    allocator
,   u64                     size
)
{
    return dynamic_allocator_allocate_aligned ( allocator , size , 1 );
}

void*
dynamic_allocator_allocate_aligned
(   dynamic_allocator_t*    allocator
,   u64                     size
,   u16                     alignment
)
{
    if ( !allocator || !size || !alignment )
    {
        if ( !allocator )
        {
            LOGERROR ( "dynamic_allocator_allocate: The provided allocator is uninitialized (0x%p)."
                     , allocator
                     );
        }
        if ( !size )
        {
            LOGERROR ( "dynamic_allocator_allocate: Cannot allocate block of size 0." );
        }
        if ( !alignment )
        {
            LOGERROR ( "dynamic_allocator_allocate: Cannot allocate block with alignment 0." );
        }

        return 0;
    }

    state_t* state = ( *allocator ).memory;

    const u64 header_size = sizeof ( header_t );
    const u64 storage_size = SIZE_STORAGE;
    const u64 required_size = alignment + header_size + storage_size + size;

    ASSERTM ( required_size < MAX_SINGLE_ALLOCATION_SIZE
            , "dynamic_allocator_allocate: Requested block size larger than MAX_SINGLE_ALLOCATION_SIZE."
            );

    u64 base_offs = 0;
    if ( !freelist_allocate_block ( &( *state ).freelist
                                  , required_size
                                  , &base_offs
                                  ))
    {
        f32 req_amt;
        f32 rem_amt;
        const char* req_unit = string_bytesize ( size , &req_amt );
        const char* rem_unit = string_bytesize ( freelist_query_free ( &( *state ).freelist )
                                               , &rem_amt
                                               );
        LOGERROR ( "dynamic_allocator_allocate: No blocks of memory large enough to allocate from."
                   "\n\tRequested size: %.2f %s (Available: %.2f %s)"
                 , req_amt , req_unit
                 , rem_amt , rem_unit
                 );
        return 0;
    }

    void* blk = ( void* )( ( u64 )( ( *state ).memory_start ) + base_offs );
    u64 blk_offs = get_aligned ( ( u64 ) blk + SIZE_STORAGE , alignment );
    u32* blk_size = ( u32* )( blk_offs - SIZE_STORAGE );
    *blk_size = ( u32 ) size;
    header_t* header = ( header_t* )( blk_offs + size );
    ( *header ).start = blk;
    ( *header ).alignment = alignment;
    return ( void* ) blk_offs;
}

bool
dynamic_allocator_free
(   dynamic_allocator_t*    allocator
,   void*                   memory
)
{
    return dynamic_allocator_free_aligned ( allocator , memory );
}

bool
dynamic_allocator_free_aligned
(   dynamic_allocator_t*    allocator
,   void*                   memory
)
{
    if ( !allocator || !memory )
    {
        if ( !allocator )
        {
            LOGERROR ( "dynamic_allocator_free: The provided allocator is uninitialized (0x%p)."
                     , allocator
                     );
        }
        if ( !memory )
        {
            LOGERROR ( "dynamic_allocator_free: Missing argument: memory." );
        }
        return false;
    }
    
    state_t* state = ( *allocator ).memory;
    const void* const memory_end = ( *state ).memory_start + ( *state ).cap;

    if (   memory < ( *state ).memory_start
        || memory > memory_end
       )
    {
        LOGWARN ( "dynamic_allocator_free: Trying to release block [0x%p] outside of allocator range [0x%p .. 0x%p]."
                , memory
                , ( *state ).memory_start
                , memory_end
                );
        return false;
    }

    u32* size = ( u32* )( ( u64 ) memory - SIZE_STORAGE );
    header_t* header = ( header_t* )( ( u64 ) memory + *size );
    u64 offs = ( u64 ) ( *header ).start - ( u64 ) ( *state ).memory_start;
    u64 required_size = ( *header ).alignment
                      + sizeof ( header_t )
                      + SIZE_STORAGE
                      + *size
                      ;

    if ( !freelist_free_block ( &( *state ).freelist
                              , required_size
                              , offs
                              ))
    {
        LOGERROR ( "dynamic_allocator_free: Failed to free memory." );
        return false;
    }

    return true;
}
