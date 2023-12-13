/**
 * @file board.h
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Datatypes and operations to represent a chess board.
 */
#ifndef CHESS_BOARD_H
#define CHESS_BOARD_H

#include "chess/common.h"
#include "chess/bitboard.h"

/**
 * @brief Computes if a square on a chess board may be attacked by a given
 * side. Requires pregenerated attack tables.
 * @param board The chess board state.
 * @param attacks Pregenerated attack tables.
 * @param square The square to check.
 * @param side The current side.
 * @return true if square attackable, false otherwise.
 */
INLINE
bool
board_square_attackable
(   const board_t*      board
,   const attacks_t*    attacks
,   const SQUARE        square
,   const SIDE          side
)
{
    return ( bitboard_pawn_attack ( attacks , square , !side ) & ( ( side == WHITE ) ? ( *board ).pieces[ P ] : ( *board ).pieces[ p ] ) )
        || ( bitboard_knight_attack ( attacks , square )  & ( ( side == WHITE ) ? ( *board ).pieces[ N ] : ( *board ).pieces[ n ] ) )
        || ( bitboard_bishop_attack ( attacks , square , ( *board ).occupancies[ 2 ] ) & ( ( side == WHITE ) ? ( *board ).pieces[ B ] : ( *board ).pieces[ b ] ) )
        || ( bitboard_rook_attack ( attacks , square , ( *board ).occupancies[ 2 ] ) & ( ( side == WHITE ) ? ( *board ).pieces[ R ] : ( *board ).pieces[ r ] ) )
        || ( bitboard_queen_attack ( attacks , square , ( *board ).occupancies[ 2 ] ) & ( ( side == WHITE ) ? ( *board ).pieces[ Q ] : ( *board ).pieces[ q ] ) )
        || ( bitboard_king_attack ( attacks , square ) & ( ( side == WHITE ) ? ( *board ).pieces[ K ] : ( *board ).pieces[ k ] ) )
        ;
}

#endif  // CHESS_BOARD_H
