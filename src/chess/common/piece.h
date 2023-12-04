/**
 * @file piece.h
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Datatypes and operations to represent a single piece on a chess board.
 */
#ifndef CHESS_COMMON_PIECE_H
#define CHESS_COMMON_PIECE_H

#include "common.h"

#include "math/math.h"

// Type definition for side.
typedef enum
{
    WHITE
,   BLACK
}
SIDE;

// Type definition for a piece.
typedef enum
{
    P , N , B , R , Q , K
,   p , n , b , r , q , k

,   EMPTY_SQ    // Extra piece for detecting an empty board square.
}
PIECE;

#endif  // CHESS_COMMON_PIECE_H
