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
 * @brief Parses a move string and writes it to an output buffer if it is valid.
 * Requires pregenerated attacks tables and a list of valid moves.
 * @param s The move string to parse.
 * @param moves A pregenerated list of all valid moves.
 * @param attacks The pregenerated attacks tables.
 * @param move Output buffer.
 * @return false if move invalid, true otherwise.
 */
bool
move_parse
(   const char*         s
,   const moves_t*      moves
,   const attacks_t*    attacks
,   move_t*             move
);

/**
 * @brief Generates the move options for a given board state using pregenerated
 * attack tables.
 * @param moves Output buffer.
 * @param attacks The pregenerated attack tables.
 * @param board A chess board state.
 * @return moves.
 */
moves_t*
moves_compute
(   moves_t*            moves
,   const board_t*      board
,   const attacks_t*    attacks
);

/**
 * @brief Applies a filter to a list of moves.
 * @param moves The list of moves.
 * @param filter The filter to apply.
 * @return moves.
 */
moves_t*
moves_filter
(   moves_t*    moves
,   MOVE_FILTER filter
);

#endif  // CHESS_MOVE_H
