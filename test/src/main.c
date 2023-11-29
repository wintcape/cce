/**
 * @file main.c
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Entry point for the engine test suite program.
 */
#include "test/test.h"

#include "core/logger.h"
#include "core/memory.h"

#include "memory/test_linear_allocator.h"
#include "memory/test_dynamic_allocator.h"

// Max system memory usage.
#define TEST_MEMORY_REQUIREMENTS    ( GIBIBYTES ( 2.5 ) )

int
main
( void )
{
    memory_startup ( TEST_MEMORY_REQUIREMENTS );

    test_startup ();

    // Initialize tests.
    test_register_linear_allocator ();
    test_register_dynamic_allocator ();

    // Run tests.
    LOGDEBUG ( "Running test suite. . ." );
    const bool fail = test_run_all ();

    memory_shutdown ();

    return fail;
}
