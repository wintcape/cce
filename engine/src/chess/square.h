/**
 * @file square.h
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Datatypes and operations to represent a single square on a chess board.
 */
#ifndef CHESS_SQUARE_H
#define CHESS_SQUARE_H

#include "common.h"

// Type definition for board square coordinate.
typedef enum
{
    A8 , B8 , C8 , D8 , E8 , F8 , G8 , H8
,   A7 , B7 , C7 , D7 , E7 , F7 , G7 , H7
,   A6 , B6 , C6 , D6 , E6 , F6 , G6 , H6
,   A5 , B5 , C5 , D5 , E5 , F5 , G5 , H5
,   A4 , B4 , C4 , D4 , E4 , F4 , G4 , H4
,   A3 , B3 , C3 , D3 , E3 , F3 , G3 , H3
,   A2 , B2 , C2 , D2 , E2 , F2 , G2 , H2
,   A1 , B1 , C1 , D1 , E1 , F1 , G1 , H1
}
SQUARE;

/**
 * @brief Bitboard index to square coordinate string.
 * @param i The index to convert.
 * @return A string representation of the board square coordinate corresponding
 * to i.
 */
const char*
string_bitboard_square
(   u8 i
);

#endif  // CHESS_SQUARE_H