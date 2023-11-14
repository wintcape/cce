/**
 * @file test_linear_allocator.c
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Tests the linear allocator.
 *        (see test.h for additional details)
 */
#include "memory/test_linear_allocator.h"
#include "memory/linear_allocator.h"

#include "common.h"

#include "test/expect.h"
#include "test/test.h"

u8
test_linear_allocator_startup_and_shutdown
( void )
{
    linear_allocator_t allocator;

    linear_allocator_startup ( sizeof ( u64 ) , 0 , &allocator );
    EXPECT_NEQ ( 0 , allocator.memory );
    EXPECT_EQ ( sizeof ( u64 ) , allocator.cap );
    EXPECT_EQ ( 0 , allocator.allocated );

    linear_allocator_shutdown ( &allocator );
    EXPECT_EQ ( 0 , allocator.memory );
    EXPECT_EQ ( 0 , allocator.cap );
    EXPECT_EQ ( 0 , allocator.allocated );

    return true;
}

u8
test_linear_allocator_max_allocation_count
( void )
{
    const u64 max_allocations = 1024;
    
    linear_allocator_t allocator;
    linear_allocator_startup ( sizeof ( u64 ) * max_allocations , 0 , &allocator );
    
    void* blk;
    for ( u64 i = 0; i < max_allocations; ++i )
    {
        blk = linear_allocator_allocate ( &allocator , sizeof ( u64 ) );
        EXPECT_NEQ ( 0 , blk );
        EXPECT_EQ ( sizeof ( u64 ) * ( i + 1 ) , allocator.allocated );
    }

    linear_allocator_shutdown ( &allocator );

    return true;
}

u8
test_linear_allocator_max_allocation_size
( void )
{
    linear_allocator_t allocator;
    linear_allocator_startup ( sizeof ( u64 ) , 0 , &allocator );
    
    void* blk = linear_allocator_allocate ( &allocator , sizeof ( u64 ) );
    EXPECT_NEQ ( 0 , blk );
    EXPECT_EQ ( sizeof ( u64 ) , allocator.allocated );

    linear_allocator_shutdown ( &allocator );

    return true;
}

u8
test_linear_allocator_overflow
( void )
{
    const u64 max_allocations = 1024;
    
    linear_allocator_t allocator;
    linear_allocator_startup ( sizeof ( u64 ) * max_allocations , 0 , &allocator );
    
    void* blk;
    for ( u64 i = 0; i < max_allocations; ++i )
    {
        blk = linear_allocator_allocate ( &allocator , sizeof ( u64 ) );
        EXPECT_NEQ ( 0 , blk );
        EXPECT_EQ ( sizeof ( u64 ) * ( i + 1 ) , allocator.allocated );
    }

    LOGDEBUG ( "The following error is intentionally triggered by a test:" );

    blk = linear_allocator_allocate ( &allocator , sizeof ( u64 ) );
    EXPECT_EQ ( 0 , blk );
    EXPECT_EQ ( sizeof ( u64 ) * max_allocations , allocator.allocated );

    linear_allocator_shutdown ( &allocator );

    return true;
}

u8
test_linear_allocator_free
( void )
{
    const u64 max_allocations = 1024;
    
    linear_allocator_t allocator;
    linear_allocator_startup ( sizeof ( u64 ) * max_allocations , 0 , &allocator );
    
    void* blk;
    for ( u64 i = 0; i < max_allocations; ++i )
    {
        blk = linear_allocator_allocate ( &allocator , sizeof ( u64 ) );
        EXPECT_NEQ ( 0 , blk );
        EXPECT_EQ ( sizeof ( u64 ) * ( i + 1 ) , allocator.allocated );
    }

    linear_allocator_free ( &allocator );
    EXPECT_EQ ( 0 , allocator.allocated );

    linear_allocator_shutdown ( &allocator );

    return true;
}

void
test_register_linear_allocator
( void )
{
    test_register ( test_linear_allocator_startup_and_shutdown
                  , "Starting or terminating a linear allocator."
                  );
    test_register ( test_linear_allocator_max_allocation_count
                  , "Testing linear allocator max allocation count."
                  );
    test_register ( test_linear_allocator_max_allocation_size
                  , "Testing linear allocator max allocation size."
                  );
    test_register ( test_linear_allocator_overflow
                  , "Testing linear allocator overflow handling."
                  );
    test_register ( test_linear_allocator_free
                  , "Testing linear allocator free."
                  );
}
