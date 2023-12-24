/**
 * @file negamax.h
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Negamax depth search algorithm.
 */
#ifndef CHESS_NEGAMAX_H
#define CHESS_NEGAMAX_H

#include "chess/common.h"

/**
 * @brief Depth search to compute the best move given a chess board state.
 * Requires pregenerated attack tables.
 * @param board A chess board state.
 * @param alpha Alpha negamax cutoff.
 * @param beta Beta negamax cutoff.
 * @param depth Maximum recursion depth.
 * @param attacks The pregenerated attack tables.
 * @return The best move.
 */
move_t
negamax
(   const board_t*      board
,   const i32           alpha
,   const i32           beta
,   const u32           depth
,   const attacks_t*    attacks
);

#endif  // CHESS_NEGAMAX_H