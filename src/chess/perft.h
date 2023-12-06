/**
 * @file perft.h
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Interface for a Perft driver for testing move generation.
 * (see also, move.h).
 */
#ifndef CHESS_PERFT_H
#define CHESS_PERFT_H

#include "chess/common.h"

#include "chess/move.h"

/**
 * @brief Runs the Perft driver. Requires pregenerated attack tables.
 * @param board A chess board state.
 * @param depth The maximum recursion depth.
 * @param filter A move filter.
 * @param attacks The pregenerated attack tables.
 */
void
perft
(   const board_t*      board
,   const u32           depth
,   const MOVE_FILTER   filter
,   const attacks_t*    attacks
);

#endif  // CHESS_PERFT_H
