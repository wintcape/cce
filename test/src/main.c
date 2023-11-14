/**
 * @file main.c
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Entry point for the engine test suite program.
 */
#include "test/test.h"

#include "core/logger.h"
#include "core/memory.h"

#include "memory/test_linear_allocator.h"

// Max system memory usage.
#define TEST_MEMORY_REQUIREMENTS    ( MEBIBYTES ( 128 ) )

int
main
( void )
{
    memory_startup ( TEST_MEMORY_REQUIREMENTS );

    test_startup ();

    // Initialize tests.
    test_register_linear_allocator ();

    // Run tests.
    LOGDEBUG ( "Running test suite. . ." );
    test_run_all ();

    memory_shutdown ();

    return 0;
}
