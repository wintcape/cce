/**
 * @file test.c
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Implementation of test header.
 *        (see test.h for additional details)
 */
#include "test/test.h"

#include "container/array.h"

#include "core/clock.h"
#include "core/logger.h"
#include "core/memory.h"
#include "core/string.h"

typedef struct
{
    PFN_test    fn;
    char*       desc;
}
test_entry_t;

static test_entry_t*    tests;

void
test_startup
( void )
{
    tests = array_allocate_new ( test_entry_t );    
}

void
test_register
(   u8      ( *PFN_test )()
,   char*   desc
)
{
    test_entry_t e;
    e.fn = PFN_test;
    e.desc = desc;

    array_push ( tests , e );
}

bool
test_run_all
( void )
{
    const u32 test_count = array_length ( tests );
    
    u32 pass = 0;
    u32 fail = 0;
    u32 skip = 0;

    clock_t clock_master;
    clock_start ( &clock_master );

    for ( u32 i = 0; i < test_count; ++i )
    {
        clock_t clock_test;
        clock_start ( &clock_test );
        u8 result = tests[ i ].fn ();
        clock_update ( &clock_test );

        if ( result == true )
        {
            pass += 1;
        }
        else if ( result == BYPASS )
        {
            LOGWARN ( "    SKIPPED:\t%s" , tests[ i ].desc );
            skip += 1;
        }
        else
        {
            LOGERROR ( "    FAILED:\t%s" , tests[ i ].desc );
            fail += 1;
        }

        char status[ 20 ];
        string_format ( status , fail ? "*** %d FAILED ***"
                                      : "SUCCESS"
                      , fail
                     );

        clock_update ( &clock_master );

        LOGINFO ( "Executed %d of %d (%d skipped) %s (%.6f sec / %.6f sec total)"
                , i + 1 , test_count , skip , status
                , clock_test.elapsed , clock_master.elapsed
                );
    }

    clock_stop ( &clock_master );

    LOGINFO ( "Results: %d passed, %d failed, %d skipped.\n\t"
              "Took %.6f seconds."
            , pass , fail , skip , clock_master.elapsed
            );

    array_free ( tests );

    return fail;
}
