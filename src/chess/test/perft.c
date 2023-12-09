/**
 * @author Matthew Weissel (null@mattweissel.info)
 * @file perft.c
 * @brief Implementation of the perft header.
 * (see perft.h for additional details)
 */
#include "chess/test/perft.h"

#include "chess/move.h"
#include "chess/string.h"

#include "core/clock.h"
#include "core/logger.h"
#include "core/memory.h"

/**
 * @brief Primary implementation of perft (see perft).
 * @param board A chess board state.
 * @param depth The current recursion depth.
 * @param filter A move filter.
 * @param attacks The pregenerated attack tables.
 * @return The current leaf node count.
 */
u64
_perft
(   board_t*            board
,   u32                 depth
,   const MOVE_FILTER   filter
,   const attacks_t*    attacks
);

void
perft
(   const board_t*      board_
,   const u32           depth
,   const attacks_t*    attacks
)
{
    board_t board;

    LOGINFO ( "perft: Started performance test." );

    // Generate moves.
    moves_t moves;
    moves_get ( &moves , board_ , attacks );

    // Initialize the clock.
    clock_t clock;
    clock_start ( &clock );

    // Perform the test.
    u64 leaf_count = 0;
    for ( u32 i = 0; i < moves.count; ++i )
    {
        // Initialize a working board state.
        memory_copy ( &board , board_ , sizeof ( board_t ) );        
        
        // Perform a move.
        if ( !move_parse ( moves.moves[ i ]
                         , MOVE_FILTER_NONE
                         , attacks
                         , &board
                         ))
        {
            continue;
        }
        
        // Recurse.
        const u64 result = _perft ( &board
                                  , depth - 1
                                  , MOVE_FILTER_NONE
                                  , attacks
                                  );
        leaf_count += result;

        // Statistics.
        char s_move[ MOVE_STRING_LENGTH + 1 ];
        LOGINFO ( "perft:\tMOVE:  %s    LEAF NODES: %llu"
                , string_move ( s_move , moves.moves[ i ] )
                , result
                );
    }
    
    
    // Update the clock.
    clock_update ( &clock );

    LOGINFO ( "perft: Successfully generated %llu leaf nodes at depth %u. Took %f seconds."
            , leaf_count
            , depth
            , clock.elapsed
            );

//    // Perform additional filtered tests to generate move statistics.
//    LOGINFO ( "\tCAPTURES:             %llu"
//            , _perft ( memory_copy ( &board , board_ , sizeof ( board_t ) )
//                     , depth
//                     , MOVE_FILTER_ONLY_CAPTURE
//                     , attacks
//                     )
//            , clock.elapsed
//            );
//    LOGINFO ( "\tEN PASSANT CAPTURES:  %llu"
//            , _perft ( memory_copy ( &board , board_ , sizeof ( board_t ) )
//                     , depth
//                     , MOVE_FILTER_ONLY_ENPASSANT
//                     , attacks
//                     )
//            , clock.elapsed
//            );
//    LOGINFO ( "\tCASTLES:              %llu"
//            , _perft ( memory_copy ( &board , board_ , sizeof ( board_t ) )
//                     , depth
//                     , MOVE_FILTER_ONLY_CASTLE
//                     , attacks
//                     )
//            , clock.elapsed
//            );
//    LOGINFO ( "\tPROMOTIONS:           %llu"
//            , _perft ( memory_copy ( &board , board_ , sizeof ( board_t ) )
//                     , depth
//                     , MOVE_FILTER_ONLY_PROMOTION
//                     , attacks
//                     )
//            , clock.elapsed
//            );
}
#include <stdio.h>
char s[32000];
u64
_perft
(   board_t*            board
,   u32                 depth
,   const MOVE_FILTER   filter
,   const attacks_t*    attacks
)
{
    // Base case.
    if ( !depth )
    {
        return 1;
    }

    // Generate move options.
    moves_t moves;
    moves_get ( &moves , board , attacks );

    u64 leaf_count = 0;
    for ( u32 i = 0; i < moves.count; ++i )
    {
        // Preserve board state.
        board_t board_prev;
        memory_copy ( &board_prev , board , sizeof ( board_t ) );

        // Perform the move.
        if ( !move_parse ( moves.moves[ i ]
                         , filter
                         , attacks
                         , board
                         ))
        {
            continue;
        }
        
        // Move succeeded. Recurse.
        leaf_count += _perft ( board , depth - 1 , filter , attacks );
        
        // Restore board state.
        memory_copy ( board , &board_prev , sizeof ( board_t ) );
    }
    
    return leaf_count;
}
