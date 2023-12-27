/**
 * @file best.h
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Best move search algorithm.
 */
#ifndef CHESS_BEST_H
#define CHESS_BEST_H

#include "chess/common.h"

/**
 * @brief Given a board state, computes the best possible move. Requires
 * pregenerated attack tables.
 * @param board A chess board state.
 * @param attacks The pregenerated attacks tables.
 * @param depth Number of moves ahead to peek.
 * @return The optimal move.
 */
move_t
board_best_move
(   const board_t*      board
,   const attacks_t*    attacks
,   const u32           depth
);

#endif  // CHESS_BEST_H