/**
 * @file board.h
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Datatypes and operations to represent a chess board.
 */
#ifndef CHESS_BOARD_H
#define CHESS_BOARD_H

#include "chess/common.h"
#include "chess/bitboard.h"
#include "chess/move.h"

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
    return square != NO_SQ
        && (   ( bitboard_pawn_attack ( attacks , square , !side ) & ( ( side == WHITE ) ? ( *board ).pieces[ P ] : ( *board ).pieces[ p ] ) )
            || ( bitboard_knight_attack ( attacks , square )  & ( ( side == WHITE ) ? ( *board ).pieces[ N ] : ( *board ).pieces[ n ] ) )
            || ( bitboard_bishop_attack ( attacks , square , ( *board ).occupancies[ 2 ] ) & ( ( side == WHITE ) ? ( *board ).pieces[ B ] : ( *board ).pieces[ b ] ) )
            || ( bitboard_rook_attack ( attacks , square , ( *board ).occupancies[ 2 ] ) & ( ( side == WHITE ) ? ( *board ).pieces[ R ] : ( *board ).pieces[ r ] ) )
            || ( bitboard_queen_attack ( attacks , square , ( *board ).occupancies[ 2 ] ) & ( ( side == WHITE ) ? ( *board ).pieces[ Q ] : ( *board ).pieces[ q ] ) )
            || ( bitboard_king_attack ( attacks , square ) & ( ( side == WHITE ) ? ( *board ).pieces[ K ] : ( *board ).pieces[ k ] ) )
           );
}

/**
 * @brief Computes if a given side is in check. Requires pregenerated attack
 * tables.
 * @param board The chess board state.
 * @param attacks Pregenerated attack tables.
 * @param side The side to test.
 * @return true if side's king is in check, false otherwise.
 */
INLINE
bool
board_check
(   const board_t*      board
,   const attacks_t*    attacks
,   const SIDE          side
)
{
    return board_square_attackable ( board
                                   , attacks
                                   , bitboard_lsb ( ( side == WHITE ) ? ( *board ).pieces[ K ]
                                                                      : ( *board ).pieces[ k ]
                                                                      )
                                   , !side
                                   );
}

/**
 * @brief Checkmate predicate. Requires a pregenerated attack tables and a list
 * of valid moves.
 * @param board A chess board state.
 * @param attacks The pregenerated attack tables.
 * @param moves The pregenerated list of valid moves.
  * @return true if checkmate, false otherwise.
 */
bool
board_checkmate
(   const board_t*      board_
,   const attacks_t*    attacks
,   const moves_t*      moves
);

/**
 * @brief Stalemate predicate. Requires a pregenerated attack tables and a list
 * of valid moves.
 * @param board A chess board state.
 * @param attacks The pregenerated attack tables.
 * @param moves The pregenerated list of valid moves.
  * @return true if stalemate, false otherwise.
 */
INLINE
bool
board_stalemate
(   const board_t*      board
,   const attacks_t*    attacks
,   const moves_t*      moves
)
{
    return !board_check ( board , attacks, ( *board ).side ) && !( *moves ).count;
}

/**
 * @brief Updates the provided board state by performing a move. Requires
 * pregenerated attack tables.
 * @param board The board state to mutate.
 * @param move The move to make.
 * @param attacks The pregenerated attack tables.
 */
void
board_move
(   board_t*            board
,   const move_t        move
,   const attacks_t*    attacks
);

#endif  // CHESS_BOARD_H