/**
 * @file best.h
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Best move search algorithm.
 */
#ifndef CHESS_BEST_H
#define CHESS_BEST_H

#include "chess/common.h"

// Type definition for a container to hold move internal search function
// parameters.
typedef struct
{
    // Pregenerated attack tables.
    const attacks_t*    attacks;
    
    // Current ply, leaf count, move count.
    u32                 ply;
    u32                 leaf_count;
    u32                 move_count;

    // Current board state.
    board_t             board;

    // Move tables: killer, history.
    move_t              killer_moves[ 2 ][ 64 ];
    move_t              history_moves[ 12 ][ 64 ];

    // Move tables: principal variation.
    move_t              pv[ 64 ][ 64 ];
    u32                 pv_len[ 64 ];
}
move_search_t;

/**
 * @brief Computes the best possible move given a board state. Requires
 * pregenerated attack tables.
 * @param board A chess board state.
 * @param attacks The pregenerated attacks tables.
 * @param depth Number of moves ahead to peek.
 * @param args Buffer to hold internal search function arguments.
 * @return The optimal move.
 */
move_t
board_best_move
(   const board_t*      board
,   const attacks_t*    attacks
,   const u32           depth
,   move_search_t*      args
);

#endif  // CHESS_BEST_H