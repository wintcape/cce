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

// Defines a global node count.
static u64 node_count = 0;

/**
 * @brief Primary implementation of perft (see perft).
 * @param board A chess board.
 * @param attacks The pregenerated attack tables.
 * @param depth Current recursion depth.
 */
void
_perft
(   board_t*            board
,   const attacks_t*    attacks
,   u32                 depth
);

void
perft
(   const board_t*      board_
,   const attacks_t*    attacks
,   const u32           depth
)
{
    // Initialize a working copy of the board.
    board_t board;
    memory_copy ( &board , board_ , sizeof ( board_t ) );

    // Initialize the clock.
    clock_t clock;
    clock_start ( &clock );

    // Perform the test.
    _perft ( &board , attacks , depth );

    // Update the clock.
    clock_update ( &clock );

    LOGINFO ( "perft: Successfully generated %llu nodes. Took %f seconds."
            , node_count
            , clock.elapsed
            );
}

void
_perft
(   board_t*            board
,   const attacks_t*    attacks
,   u32                 depth
)
{
    // Base case.
    if ( !depth )
    {
        node_count += 1;
        return;
    }

    // Generate move options.
    moves_t moves;
    moves_get ( &moves , board , attacks );

    for ( u32 i = 0; i < moves.count; ++i )
    {
        // Preserve board state.
        board_t board_prev;
        memory_copy ( &board_prev , board , sizeof ( board_t ) );

        // Perform the move. If it fails, do nothing.
        if ( !move_parse ( moves.moves[ i ]
                         , MOVE_FILTER_NONE
                         , attacks
                         , board
                         ))
        {
            continue;
        }

        // Recursive case.
        _perft ( board , attacks , depth - 1 );

        // Restore board state.
        memory_copy ( board , &board_prev , sizeof ( board_t ) );
    }
}
