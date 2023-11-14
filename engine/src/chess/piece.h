/**
 * @file piece.h
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Datatypes and operations to represent a single piece on a chess board.
 */
#ifndef CHESS_PIECE_H
#define CHESS_PIECE_H

#include "common.h"

// Type definition for side.
typedef enum
{
    WHITE
,   BLACK
}
SIDE;

// Type definition for player.
typedef enum
{
    PAWN
,   KNIGHT
,   BISHOP
,   ROOK
,   QUEEN
,   KING
}
PLAYER;

// Type definition for a piece.
typedef struct
{
    SIDE    side;
    PLAYER  player;
}
piece_t;

#endif  // CHESS_PIECE_H
