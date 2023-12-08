/**
 * @file move.h
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Defines operations for pregenerating move options.
 */
#ifndef CHESS_MOVE_H
#define CHESS_MOVE_H

#include "chess/common.h"

// Defines filters for generating or applying only certain types of move.
typedef enum
{
    MOVE_FILTER_NONE
,   MOVE_FILTER_ONLY_PAWN
,   MOVE_FILTER_ONLY_KNIGHT
,   MOVE_FILTER_ONLY_BISHOP
,   MOVE_FILTER_ONLY_ROOK
,   MOVE_FILTER_ONLY_QUEEN
,   MOVE_FILTER_ONLY_KING
,   MOVE_FILTER_ONLY_CAPTURE
,   MOVE_FILTER_ONLY_PROMOTION
,   MOVE_FILTER_ONLY_DOUBLE_PUSH
,   MOVE_FILTER_ONLY_ENPASSANT
,   MOVE_FILTER_ONLY_CASTLE
}
MOVE_FILTER;

/**
 * @brief Parses a move and updates the provided board. Requires pregenerated
 * attack tables.
 * @param move The move to make.
 * @param filter A move filter.
 * @param attacks The pregenerated attack tables.
 * @param board The board state to mutate.
 * @return false if move not made, true otherwise.
 */
bool
move_parse
(   const move_t        move
,   const MOVE_FILTER   filter
,   const attacks_t*    attacks
,   board_t*            board
);

/**
 * @brief Generates the move options for a given board state using pregenerated
 * attack tables.
 * @param attacks The pregenerated attack tables.
 * @param board A chess board state.
 */
void
moves_get
(   moves_t*            moves
,   const board_t*      board
,   const attacks_t*    attacks
);

#endif  // CHESS_MOVE_H
