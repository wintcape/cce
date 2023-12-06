/**
 * @author Matthew Weissel (null@mattweissel.info)
 * @file perft.c
 * @brief Implementation of the perft header.
 * (see perft.h for additional details)
 */
#include "chess/perft.h"

#include "chess/move.h"

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
,   const MOVE_FILTER   filter
,   const attacks_t*    attacks
)
{
    // Initialize a working copy of the board.
    board_t board;
    memory_copy ( &board , board_ , sizeof ( board_t ) );

    // Initialize the clock.
    clock_t clock;
    clock_start ( &clock );

    // Perform the test.
    const u64 count = _perft ( &board , depth , filter , attacks );

    // Update the clock.
    clock_update ( &clock );

    LOGINFO ( "perft: Successfully generated %llu leaf nodes. Took %f seconds."
            , count
            , clock.elapsed
            );
}

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

        // Perform the move. If it succeeds, recurse.
        if ( move_parse ( moves.moves[ i ]
                        , filter
                        , attacks
                        , board
                        ))
        {
            leaf_count += _perft ( board , depth - 1 , filter , attacks );
        }

        // Restore board state.
        memory_copy ( board , &board_prev , sizeof ( board_t ) );
    }
    
    return leaf_count;
}
