/**
 * @file test_dynamic_allocator.c
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Tests the dynamic allocator.
 *        (see test.h for additional details)
 */
#include "memory/test_dynamic_allocator.h"
#include "memory/dynamic_allocator.h"

#include "common.h"

#include "test/expect.h"
#include "test/test.h"

#include "core/memory.h"
#include "core/string.h"

// Type definition for a container to hold allocation information.
typedef struct
{
    void*   block;
    u16     alignment;
    u64     size;
}
alloc_t;

u8
test_dynamic_allocator_create_and_destroy
( void )
{
    dynamic_allocator_t allocator;

    u64 memory_requirement = 0;
    EXPECT ( dynamic_allocator_create ( 1024
                                      , &memory_requirement
                                      , 0
                                      , 0
                                      ));
    
    void* memory = memory_allocate ( memory_requirement
                                   , MEMORY_TAG_APPLICATION
                                   );
    EXPECT ( dynamic_allocator_create ( 1024
                                      , &memory_requirement
                                      , memory
                                      , &allocator
                                      ));
    EXPECT_NEQ ( 0 , allocator.memory );
    
    u64 free = dynamic_allocator_query_free ( &allocator );
    EXPECT_EQ ( 1024 , free );

    dynamic_allocator_destroy ( &allocator );
    EXPECT_EQ ( 0 , allocator.memory );
    
    memory_free ( memory
                , memory_requirement
                , MEMORY_TAG_APPLICATION
                );

    return true;
}

u8
test_dynamic_allocator_single_allocation_all_space
( void )
{
    dynamic_allocator_t allocator;

    u64 memory_requirement = 0;
    const u64 allocator_size = 1024;
    const u64 alignment = 1;
    const u64 total_allocator_size = allocator_size
                                   + dynamic_allocator_header_size ()
                                   + alignment;

    EXPECT ( dynamic_allocator_create ( total_allocator_size
                                      , &memory_requirement
                                      , 0
                                      , 0
                                      ));
    
    void* memory = memory_allocate ( memory_requirement
                                   , MEMORY_TAG_APPLICATION
                                   );

    EXPECT ( dynamic_allocator_create ( total_allocator_size
                                      , &memory_requirement
                                      , memory
                                      , &allocator
                                      ));
    EXPECT_NEQ ( 0 , allocator.memory );
    
    u64 free = dynamic_allocator_query_free ( &allocator );
    EXPECT_EQ ( total_allocator_size , free );

    void* blk = dynamic_allocator_allocate ( &allocator
                                           , 1024
                                           );
    EXPECT_NEQ ( 0 , blk );
    free = dynamic_allocator_query_free ( &allocator );
    EXPECT_EQ ( 0 , free );

    dynamic_allocator_free ( &allocator
                           , blk
                           );
    free = dynamic_allocator_query_free ( &allocator );
    EXPECT_EQ ( total_allocator_size , free );

    dynamic_allocator_destroy ( &allocator );
    EXPECT_EQ ( 0 , allocator.memory );
    
    memory_free ( memory
                , memory_requirement
                , MEMORY_TAG_APPLICATION
                );

    return true;
}

u8
test_dynamic_allocator_multi_allocation_all_space
( void )
{
    dynamic_allocator_t allocator;
    
    u64 memory_requirement = 0;
    const u64 allocator_size = 1024;
    const u64 alignment = 1;
    u64 header_size = dynamic_allocator_header_size () + alignment;
    const u64 total_allocator_size = allocator_size + 3 * header_size;

    EXPECT ( dynamic_allocator_create ( total_allocator_size
                                      , &memory_requirement
                                      , 0
                                      , 0
                                      ));
    
    void* memory = memory_allocate ( memory_requirement
                                   , MEMORY_TAG_APPLICATION
                                   );
    
    EXPECT ( dynamic_allocator_create ( total_allocator_size
                                      , &memory_requirement
                                      , memory
                                      , &allocator
                                      ));
    EXPECT_NEQ ( 0 , allocator.memory );
    
    u64 free = dynamic_allocator_query_free ( &allocator );
    EXPECT_EQ ( total_allocator_size , free );

    void* blk0 = dynamic_allocator_allocate ( &allocator , 256 );
    EXPECT_NEQ ( 0 , blk0 );
    free = dynamic_allocator_query_free ( &allocator );
    EXPECT_EQ ( 768 + 2 * header_size , free );

    void* blk1 = dynamic_allocator_allocate ( &allocator , 512 );
    EXPECT_NEQ ( 0 , blk1 );
    free = dynamic_allocator_query_free ( &allocator );
    EXPECT_EQ ( 256 + header_size , free );

    void* blk2 = dynamic_allocator_allocate( &allocator , 256 );
    EXPECT_NEQ ( 0 , blk2 );
    free = dynamic_allocator_query_free ( &allocator );
    EXPECT_EQ ( 0 , free );

    dynamic_allocator_free ( &allocator , blk2 );
    free = dynamic_allocator_query_free ( &allocator );
    EXPECT_EQ ( 256 + header_size , free );

    dynamic_allocator_free ( &allocator , blk0 );
    free = dynamic_allocator_query_free ( &allocator );
    EXPECT_EQ ( 512 + 2 * header_size , free );

    dynamic_allocator_free ( &allocator , blk1 );
    free = dynamic_allocator_query_free ( &allocator );
    EXPECT_EQ ( total_allocator_size , free );

    dynamic_allocator_destroy ( &allocator );
    EXPECT_EQ ( 0 , allocator.memory );
    
    memory_free ( memory
                , memory_requirement
                , MEMORY_TAG_APPLICATION
                );

    return true;
}

u8
test_dynamic_allocator_multi_allocation_over_allocate
( void )
{
    dynamic_allocator_t allocator;
    
    u64 memory_requirement = 0;
    const u64 allocator_size = 1024;
    const u64 alignment = 1;
    u64 header_size = dynamic_allocator_header_size () + alignment;
    const u64 total_allocator_size = allocator_size + 3 * header_size;

    EXPECT ( dynamic_allocator_create ( total_allocator_size
                                      , &memory_requirement
                                      , 0
                                      , 0
                                      ));
    
    void* memory = memory_allocate ( memory_requirement
                                   , MEMORY_TAG_APPLICATION
                                   );
    
    EXPECT ( dynamic_allocator_create ( total_allocator_size
                                      , &memory_requirement
                                      , memory
                                      , &allocator
                                      ));
    EXPECT_NEQ ( 0 , allocator.memory );
    
    u64 free = dynamic_allocator_query_free ( &allocator );
    EXPECT_EQ ( total_allocator_size , free );

    void* blk0 = dynamic_allocator_allocate ( &allocator , 256 );
    EXPECT_NEQ ( 0 , blk0 );
    free = dynamic_allocator_query_free ( &allocator );
    EXPECT_EQ ( 768 + 2 * header_size , free );

    void* blk1 = dynamic_allocator_allocate ( &allocator , 512 );
    EXPECT_NEQ ( 0 , blk1 );
    free = dynamic_allocator_query_free ( &allocator );
    EXPECT_EQ ( 256 + header_size , free );

    void* blk2 = dynamic_allocator_allocate( &allocator , 256 );
    EXPECT_NEQ ( 0 , blk2 );
    free = dynamic_allocator_query_free ( &allocator );
    EXPECT_EQ ( 0 , free );

    LOGDEBUG ( "The following warning and error are intentionally triggered by a test:" );
    
    void* blk_fail = dynamic_allocator_allocate ( &allocator , 256 );
    EXPECT_EQ ( 0 , blk_fail );
    free = dynamic_allocator_query_free ( &allocator );
    EXPECT_EQ ( 0 , free );

    dynamic_allocator_destroy ( &allocator );
    EXPECT_EQ ( 0 , allocator.memory );
    
    memory_free ( memory
                , memory_requirement
                , MEMORY_TAG_APPLICATION
                );

    return true; 
}

u8
test_dynamic_allocator_multi_allocation_most_space_request_too_big
( void )
{
    dynamic_allocator_t allocator;
    
    u64 memory_requirement = 0;
    const u64 allocator_size = 1024;
    const u64 alignment = 1;
    u64 header_size = dynamic_allocator_header_size () + alignment;
    const u64 total_allocator_size = allocator_size + 3 * header_size;

    EXPECT ( dynamic_allocator_create ( total_allocator_size
                                      , &memory_requirement
                                      , 0
                                      , 0
                                      ));
    
    void* memory = memory_allocate ( memory_requirement
                                   , MEMORY_TAG_APPLICATION
                                   );
    
    EXPECT ( dynamic_allocator_create ( total_allocator_size
                                      , &memory_requirement
                                      , memory
                                      , &allocator
                                      ));
    EXPECT_NEQ ( 0 , allocator.memory );
    
    u64 free = dynamic_allocator_query_free ( &allocator );
    EXPECT_EQ ( total_allocator_size , free );

    void* blk0 = dynamic_allocator_allocate ( &allocator , 256 );
    EXPECT_NEQ ( 0 , blk0 );
    free = dynamic_allocator_query_free ( &allocator );
    EXPECT_EQ ( 768 + 2 * header_size , free );

    void* blk1 = dynamic_allocator_allocate ( &allocator , 512 );
    EXPECT_NEQ ( 0 , blk1 );
    free = dynamic_allocator_query_free ( &allocator );
    EXPECT_EQ ( 256 + header_size , free );

    void* blk2 = dynamic_allocator_allocate( &allocator , 128 );
    EXPECT_NEQ ( 0 , blk2 );
    free = dynamic_allocator_query_free ( &allocator );
    EXPECT_EQ ( 128 , free );

    LOGDEBUG ( "The following warning and error are intentionally triggered by a test:" );

    void* blk_fail = dynamic_allocator_allocate ( &allocator , 256 );
    EXPECT_EQ ( 0 , blk_fail );
    free = dynamic_allocator_query_free ( &allocator );
    EXPECT_EQ ( 128 , free );

    dynamic_allocator_destroy ( &allocator );
    EXPECT_EQ ( 0 , allocator.memory );
    
    memory_free ( memory
                , memory_requirement
                , MEMORY_TAG_APPLICATION
                );

    return true; 
}

u8
test_dynamic_allocator_single_alloc_aligned
( void )
{
    dynamic_allocator_t allocator;

    u64 memory_requirement = 0;
    const u64 allocator_size = 1024;
    const u64 alignment = 16;
    const u64 total_allocator_size = allocator_size
                                   + dynamic_allocator_header_size ()
                                   + alignment;

    EXPECT ( dynamic_allocator_create ( total_allocator_size
                                      , &memory_requirement
                                      , 0
                                      , 0
                                      ));
    
    void* memory = memory_allocate ( memory_requirement
                                   , MEMORY_TAG_APPLICATION
                                   );

    EXPECT ( dynamic_allocator_create ( total_allocator_size
                                      , &memory_requirement
                                      , memory
                                      , &allocator
                                      ));
    EXPECT_NEQ ( 0 , allocator.memory );
    
    u64 free = dynamic_allocator_query_free ( &allocator );
    EXPECT_EQ ( total_allocator_size , free );

    void* blk = dynamic_allocator_allocate_aligned ( &allocator
                                                   , 1024
                                                   , alignment
                                                   );
    EXPECT_NEQ ( 0 , blk );

    u64 blk_size;
    u16 blk_alignment;
    EXPECT ( dynamic_allocator_size_alignment ( blk
                                              , &blk_size
                                              , &blk_alignment
                                              ));
    EXPECT_EQ ( alignment , blk_alignment );
    EXPECT_EQ ( 1024 , blk_size );
    free = dynamic_allocator_query_free ( &allocator );
    EXPECT_EQ ( 0 , free );
    
    dynamic_allocator_free_aligned ( &allocator , blk );
    free = dynamic_allocator_query_free ( &allocator );
    EXPECT_EQ ( total_allocator_size , free );

    dynamic_allocator_destroy ( &allocator );
    EXPECT_EQ ( 0 , allocator.memory );
    
    memory_free ( memory
                , memory_requirement
                , MEMORY_TAG_APPLICATION
                );

    return true;
}

u8
test_dynamic_allocator_multiple_alloc_aligned_different_alignments
( void )
{
    dynamic_allocator_t allocator;

    u64 memory_requirement = 0;
    u64 currently_allocated = 0;
    const u64 header_size = dynamic_allocator_header_size ();

    const u32 alloc_count = 4;
    alloc_t allocs[ 4 ];
    allocs[ 0 ] = ( alloc_t ){ 0 ,  1, 31 };    // 1-byte alignment.
    allocs[ 1 ] = ( alloc_t ){ 0 , 16, 82 };    // 16-byte alignment.
    allocs[ 2 ] = ( alloc_t ){ 0 ,  1, 59 };    // 1-byte alignment.
    allocs[ 3 ] = ( alloc_t ){ 0 ,  8, 73 };    // 1-byte alignment.

    u64 total_allocator_size = 0;
    for ( u32 i = 0; i < alloc_count; ++i )
    {
        total_allocator_size += allocs[ i ].alignment
                              + header_size
                              + allocs[ i ].size
                              ;
    }
    
    EXPECT ( dynamic_allocator_create ( total_allocator_size
                                      , &memory_requirement
                                      , 0
                                      , 0
                                      ));
    
    void* memory = memory_allocate ( memory_requirement
                                   , MEMORY_TAG_APPLICATION
                                   );

    LOGDEBUG ( "The following warning is intentionally triggered by a test:" );

    EXPECT ( dynamic_allocator_create ( total_allocator_size
                                      , &memory_requirement
                                      , memory
                                      , &allocator
                                      ));
    EXPECT_NEQ ( 0 , allocator.memory );
    
    u64 free = dynamic_allocator_query_free ( &allocator );
    EXPECT_EQ ( total_allocator_size , free );
   
    {
        u32 i = 0;
        allocs[ i ].block = dynamic_allocator_allocate_aligned ( &allocator
                                                               , allocs[ i ].size
                                                               , allocs[ i ].alignment
                                                               );
        EXPECT_NEQ ( 0 , allocs[ i ].block );

        u64 block_size;
        u16 block_alignment;
        EXPECT ( dynamic_allocator_size_alignment ( allocs[ i ].block
                                                  , &block_size
                                                  , &block_alignment
                                                  ));
        EXPECT_EQ ( allocs[ i ].alignment , block_alignment );
        EXPECT_EQ ( allocs[ i ].size , block_size );
        currently_allocated += allocs[ i ].size
                             + header_size
                             + allocs[ i ].alignment
                             ;
        free = dynamic_allocator_query_free ( &allocator );
        EXPECT_EQ ( total_allocator_size - currently_allocated , free );
    }
    {
        u32 i = 1;
        allocs[ i ].block = dynamic_allocator_allocate_aligned ( &allocator
                                                               , allocs[ i ].size
                                                               , allocs[ i ].alignment
                                                               );
        EXPECT_NEQ ( 0 , allocs[ i ].block );

        u64 block_size;
        u16 block_alignment;
        EXPECT ( dynamic_allocator_size_alignment ( allocs[ i ].block
                                                  , &block_size
                                                  , &block_alignment
                                                  ));
        EXPECT_EQ ( allocs[ i ].alignment , block_alignment );
        EXPECT_EQ ( allocs[ i ].size , block_size );
        currently_allocated += allocs[ i ].size
                             + header_size
                             + allocs[ i ].alignment
                             ;
        free = dynamic_allocator_query_free ( &allocator );
        EXPECT_EQ ( total_allocator_size - currently_allocated , free );
    }
    {
        u32 i = 2;
        allocs[ i ].block = dynamic_allocator_allocate_aligned ( &allocator
                                                               , allocs[ i ].size
                                                               , allocs[ i ].alignment
                                                               );
        EXPECT_NEQ ( 0 , allocs[ i ].block );

        u64 block_size;
        u16 block_alignment;
        EXPECT ( dynamic_allocator_size_alignment ( allocs[ i ].block
                                                  , &block_size
                                                  , &block_alignment
                                                  ));
        EXPECT_EQ ( allocs[ i ].alignment , block_alignment );
        EXPECT_EQ ( allocs[ i ].size , block_size );
        currently_allocated += allocs[ i ].size
                             + header_size
                             + allocs[ i ].alignment
                             ;
        free = dynamic_allocator_query_free ( &allocator );
        EXPECT_EQ ( total_allocator_size - currently_allocated , free );
    }
    {
        u32 i = 3;
        allocs[ i ].block = dynamic_allocator_allocate_aligned ( &allocator
                                                             , allocs[ i ].size
                                                             , allocs[ i ].alignment
                                                             );
        EXPECT_NEQ ( 0 , allocs[ i ].block );

        u64 block_size;
        u16 block_alignment;
        EXPECT ( dynamic_allocator_size_alignment ( allocs[ i ].block
                                                  , &block_size
                                                  , &block_alignment
                                                  ));
        EXPECT_EQ ( allocs[ i ].alignment , block_alignment );
        EXPECT_EQ ( allocs[ i ].size , block_size );
        currently_allocated += allocs[ i ].size
                             + header_size
                             + allocs[ i ].alignment
                             ;
        free = dynamic_allocator_query_free ( &allocator );
        EXPECT_EQ ( total_allocator_size - currently_allocated , free );
    }
    {
        u32 i = 1;
        dynamic_allocator_free_aligned ( &allocator 
                                       , allocs[ i ].block
                                       );
        allocs[ i ].block = 0;
        currently_allocated -= allocs[ i ].size
                             + header_size
                             + allocs[ i ].alignment
                             ;
        free = dynamic_allocator_query_free ( &allocator );
        EXPECT_EQ ( total_allocator_size - currently_allocated , free );
    }
    {
        u32 i = 3;
        dynamic_allocator_free_aligned ( &allocator 
                                       , allocs[ i ].block
                                       );
        allocs[ i ].block = 0;
        currently_allocated -= allocs[ i ].size
                             + header_size
                             + allocs[ i ].alignment
                             ;
        free = dynamic_allocator_query_free ( &allocator );
        EXPECT_EQ ( total_allocator_size - currently_allocated , free );
    }
    {
        u32 i = 2;
        dynamic_allocator_free_aligned ( &allocator 
                                       , allocs[ i ].block
                                       );
        allocs[ i ].block = 0;
        currently_allocated -= allocs[ i ].size
                             + header_size
                             + allocs[ i ].alignment
                             ;
        free = dynamic_allocator_query_free ( &allocator );
        EXPECT_EQ ( total_allocator_size - currently_allocated , free );
    }
    {
        u32 i = 0;
        dynamic_allocator_free_aligned ( &allocator 
                                       , allocs[ i ].block
                                       );
        allocs[ i ].block = 0;
        currently_allocated -= allocs[ i ].size
                             + header_size
                             + allocs[ i ].alignment
                             ;
        free = dynamic_allocator_query_free ( &allocator );
        EXPECT_EQ ( total_allocator_size - currently_allocated , free );
    }

    dynamic_allocator_destroy ( &allocator );
    EXPECT_EQ ( 0 , allocator.memory );
    
    memory_free ( memory
                , memory_requirement
                , MEMORY_TAG_APPLICATION
                );

    return true;
}

u8
test_dynamic_allocator_util_allocate
(   dynamic_allocator_t*    allocator
,   alloc_t*                data
,   u64*                    currently_allocated
,   u64                     header_size
,   u64                     total_allocator_size
)
{
    ( *data ).block = dynamic_allocator_allocate_aligned ( allocator
                                                         , ( *data ).size
                                                         , ( *data ).alignment
                                                         );
    EXPECT_NEQ ( 0 , ( *data ).block );
    
    u64 block_size;
    u16 block_alignment;
    EXPECT ( dynamic_allocator_size_alignment ( ( *data ).block
                                              , &block_size
                                              , &block_alignment
                                              ));
    EXPECT_EQ ( ( *data ).alignment , block_alignment );
    EXPECT_EQ ( ( *data ).size , block_size );
    *currently_allocated += ( *data ).size
                          + header_size
                          + ( *data ).alignment
                          ;
    u64 free = dynamic_allocator_query_free ( allocator );
    EXPECT_EQ ( total_allocator_size - *currently_allocated , free );

    return true;
}

u8
test_dynamic_allocator_util_free
(   dynamic_allocator_t*    allocator
,   alloc_t*                data
,   u64*                    currently_allocated
,   u64                     header_size
,   u64                     total_allocator_size
)
{
    if ( !dynamic_allocator_free_aligned ( allocator , ( *data ).block ) )
    {
        LOGERROR( "test_dynamic_allocator_util_free:  dynamic_allocator_free_aligned failed.");
        return false;
    }
    ( *data ).block = 0;
    currently_allocated -= ( *data ).size
                         + header_size
                         + ( *data ).alignment
                         ;
    u64 free = dynamic_allocator_query_free ( allocator );
    EXPECT_EQ ( total_allocator_size - *currently_allocated , free );

    return true;
}

u8
test_dynamic_allocator_multiple_alloc_aligned_different_alignments_random
( void )
{
    dynamic_allocator_t allocator;

    u64 memory_requirement = 0;
    u64 currently_allocated = 0;
    const u64 header_size = dynamic_allocator_header_size ();

    const u32 alloc_count = 65556;
    alloc_t allocs[ 65556 ] = { 0 };
    u16 po2[ 8 ] = { 1 , 2 , 4 , 8 , 16 , 32 , 64 , 128 };
    for ( u32 i = 0; i < alloc_count; ++i )
    {
        allocs[ i ].alignment = po2[ random2( 0 , 7 ) ];
        allocs[ i ].size = ( u64 ) random2( 1 , 65536 );
    }

    u64 total_allocator_size = 0;
    for ( u32 i = 0; i < alloc_count; ++i )
    {
        total_allocator_size += allocs[ i ].alignment
                              + header_size
                              + allocs[ i ].size
                              ;
    }
    
    EXPECT ( dynamic_allocator_create ( total_allocator_size
                                      , &memory_requirement
                                      , 0
                                      , 0
                                      ));
    
    void* memory = memory_allocate ( memory_requirement
                                   , MEMORY_TAG_APPLICATION
                                   );

    EXPECT ( dynamic_allocator_create ( total_allocator_size
                                      , &memory_requirement
                                      , memory
                                      , &allocator
                                      ));
    EXPECT_NEQ ( 0 , allocator.memory );
    
    u64 free = dynamic_allocator_query_free ( &allocator );
    EXPECT_EQ ( total_allocator_size , free );


    f32 amt = 0.0f;
    const char* unit = string_bytesize ( total_allocator_size , &amt );
    LOGTRACE ("Preparing to allocate %.2f %s. . ." , amt , unit ); 

    u32 alloc = 0;
    while ( alloc != alloc_count )
    {
        u32 i = random2 ( 0 , alloc_count );
        if ( allocs[ i ].block == 0 )
        {
            if ( !test_dynamic_allocator_util_allocate ( &allocator
                                                       , &allocs[ i ]
                                                       , &currently_allocated
                                                       , header_size
                                                       , total_allocator_size
                                                       ))
            {
                LOGERROR( "test_dynamic_allocator_multiple_alloc_aligned_different_alignments_random:  test_dynamic_allocator_util_allocate failed on index: %u."
                        , i
                        );
                return false;
            }
            alloc += 1;
        }
    }

    LOGTRACE ("Preparing to free %u times. . ."
             , alloc
             );   

    while ( alloc != alloc_count )
    {
        u32 i = random2 ( 0 , alloc_count );
        if ( allocs[ i ].block != 0 )
        {
            if ( !test_dynamic_allocator_util_free ( &allocator
                                                   , &allocs[i]
                                                   , &currently_allocated
                                                   , header_size
                                                   , total_allocator_size
                                                   ))
            {
                LOGERROR ("test_dynamic_allocator_multiple_alloc_aligned_different_alignments_random:  test_dynamic_allocator_util_free failed on index: %u."
                         , i
                         );
                return false;
            }
            alloc -= 1;
        }
    }
 
    dynamic_allocator_destroy ( &allocator );
    EXPECT_EQ ( 0 , allocator.memory );
    
    memory_free ( memory
                , memory_requirement
                , MEMORY_TAG_APPLICATION
                );

    return true;
}

//u8
//test_dynamic_allocator_multiple_alloc_and_free_aligned_different_alignments_random
//( void )
//{
//    dynamic_allocator_t allocator;
//
//    u64 memory_requirement = 0;
//    u64 currently_allocated = 0;
//    const u64 header_size = dynamic_allocator_header_size ();
//
//    const u32 alloc_count = 65556;
//    alloc_t allocs[ 65556 ] = { 0 };
//    u16 po2[ 8 ] = { 1 , 2 , 4 , 8 , 16 , 32 , 64 , 128 };
//    for ( u32 i = 0; i < alloc_count; ++i )
//    {
//        allocs[ i ].alignment = po2[ random2 ( 0 , 7 ) ];
//        allocs[ i ].size = ( u64 ) random2 ( 1 , 65536 );
//    }
//
//    u64 total_allocator_size = 0;
//    for ( u32 i = 0; i < alloc_count; ++i )
//    {
//        total_allocator_size += allocs[ i ].alignment
//                              + header_size
//                              + allocs[ i ].size
//                              ;
//    }
//    
//    EXPECT ( dynamic_allocator_create ( total_allocator_size
//                                      , &memory_requirement
//                                      , 0
//                                      , 0
//                                      ));
//    
//    void* memory = memory_allocate ( memory_requirement
//                                   , MEMORY_TAG_APPLICATION
//                                   );
//
//    EXPECT ( dynamic_allocator_create ( total_allocator_size
//                                      , &memory_requirement
//                                      , memory
//                                      , &allocator
//                                      ));
//    EXPECT_NEQ ( 0 , allocator.memory );
//    u64 free = dynamic_allocator_query_free ( &allocator );
//    EXPECT_EQ ( total_allocator_size , free );
//
//    u32 op = 0;
//    const u32 max_op = 10000000;
//    u32 alloc = 0;
//    while ( op < max_op )
//    {
//        if ( !alloc || random2( 0 , 99 ) > 50 )
//        {
//            for (;;)
//            {
//                u32 i = ( u32 ) random2 ( 0 , alloc_count - 1 );
//                if ( !allocs[ i ].block )
//                {
//                    if ( !test_dynamic_allocator_util_allocate ( &allocator
//                                                               , &allocs[ i ]
//                                                               , &currently_allocated
//                                                               , header_size
//                                                               , total_allocator_size
//                                                               ))
//                    {
//                        LOGERROR ( "test_dynamic_allocator_multiple_alloc_and_free_aligned_different_alignments_random:  test_dynamic_allocator_util_allocate failed on index: %u."
//                                 , i
//                                 );
//                        return false;
//                    }
//                     alloc += 1;
//                    break;
//                }
//            }
//            op += 1;
//        }
//        else
//        {
//            for (;;) 
//            {
//                u32 i = ( u32 ) random2 ( 0 , alloc_count - 1 );
//                if ( allocs[ i ].block )
//                {
//                    if ( !test_dynamic_allocator_util_free ( &allocator
//                                                           , &allocs[ i ]
//                                                           , &currently_allocated
//                                                           , header_size
//                                                           , total_allocator_size
//                                                           ))
//                    {
//                        LOGERROR ( "test_dynamic_allocator_multiple_alloc_and_free_aligned_different_alignments_random:  test_dynamic_allocator_util_free failed on index: %u."
//                                 , i
//                                 );
//                        return false;
//                    }
//                    alloc -= 1;
//                    break;
//                }
//            }
//            op += 1;
//        }
//    }
//
//    LOGTRACE ( "Max op count of %u reached. Freeing remaining allocations."
//             , max_op
//             );
//
//    for ( u32 i = 0; i < alloc_count; ++i )
//    {
//        if ( allocs[ i ].block )
//        {
//            if ( !test_dynamic_allocator_util_free ( &allocator
//                                                   , &allocs[ i ]
//                                                   , &currently_allocated
//                                                   , header_size
//                                                   , total_allocator_size
//                                                   ))
//            {
//                LOGERROR ( "test_dynamic_allocator_multiple_alloc_and_free_aligned_different_alignments_random:  test_dynamic_allocator_util_free failed on index: %u."
//                         , i
//                         );
//                return false;
//            }
//        }
//    }
//
//    dynamic_allocator_destroy ( &allocator );
//    EXPECT_EQ ( 0 , allocator.memory );
//    
//    memory_free ( memory
//                , memory_requirement
//                , MEMORY_TAG_APPLICATION
//                );
//
//    return true;
//}

void
test_register_dynamic_allocator
( void )
{
    test_register ( test_dynamic_allocator_create_and_destroy
                  , "Starting or terminating a dynamic allocator."
                  ); 
    test_register ( test_dynamic_allocator_single_allocation_all_space
				  , "Testing dynamic allocator with a single allocation."
				  );
    test_register ( test_dynamic_allocator_multi_allocation_all_space
				  , "Testing dynamic allocator with a multiple allocations."
				  );
    test_register ( test_dynamic_allocator_multi_allocation_over_allocate
				  , "Testing dynamic allocator overflow handling (1)."
				  );
    test_register ( test_dynamic_allocator_multi_allocation_most_space_request_too_big
				  , "Testing dynamic allocator overflow handling (2)."
				  );
    test_register ( test_dynamic_allocator_single_alloc_aligned
				  , "Testing dynamic allocator with a single aligned allocation."
				  );
    test_register ( test_dynamic_allocator_multiple_alloc_aligned_different_alignments
				  , "Testing dynamic allocator with multiple aligned allocations, each with different alignments."
				  );
    test_register ( test_dynamic_allocator_multiple_alloc_aligned_different_alignments_random
				  , "Testing dynamic allocator with multiple aligned allocations, each with different alignments, in random order."
				  );
//    test_register ( test_dynamic_allocator_multiple_alloc_and_free_aligned_different_alignments_random
//				  , "Testing dynamic allocator randomization."
//				  );

}
