/**
 * @file best.h
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Best move search algorithm.
 */
#ifndef CHESS_BEST_H
#define CHESS_BEST_H

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

/**
 * @brief Board state evaluation function.
 * @param board A chess board state.
 * @return A score corresponding to the board state.
 */
i32
score_board
(   const board_t* board
);

/**
 * @brief Move evaluation function.
 * @param move A move.
 * @param board A chess board state.
 * @return A score corresponding to the move.
 */
i32
score_move
(   const move_t    move
,   const board_t*  board
);

/**
 * @brief Sorts a move list by move score.
 * @param moves A pregenerated list of valid moves.
 * @param board A chess board state.
 * @return moves.
 */
moves_t*
moves_sort_by_score
(   moves_t*        moves
,   const board_t*  board
);

#endif  // CHESS_BEST_H