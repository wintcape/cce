/**
 * @file perft.h
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Interface for a Perft driver for testing moves.
 */
#ifndef CHESS_PERFT_H
#define CHESS_PERFT_H

#include "chess/common.h"

/**
 * @brief Runs the Perft driver. Requires pregenerated attack tables.
 * @param board A chess board.
 * @param attacks The pregenerated attack tables.
 * @param depth The maximum recursion depth.
 */
void
perft
(   const board_t*      board
,   const attacks_t*    attacks
,   const u32           depth
);

#endif  // CHESS_PERFT_H
