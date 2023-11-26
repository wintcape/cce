/**
 * @file board.h
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Datatypes and operations to represent a chess board.
 */
#ifndef CHESS_BOARD_H
#define CHESS_BOARD_H

#include "common.h"

#include "chess/bitboard.h"
#include "chess/castle.h"

// Defaults.
#define FEN_STRING_MAX_LENGTH 128

// Type definition for a container to hold chess board state information.
typedef struct
{
    bitboard_t  pieces[ 12 ];
    bitboard_t  occupancies[ 3 ];

    SIDE        side;
    SQUARE      enpassant;
    CASTLE      castle;

    char        fen[ FEN_STRING_MAX_LENGTH ];
}
board_t;

/**
 * @brief Renders a chess board to the console.
 * @param board A chess board.
 */
void
render_chess_board
(   const board_t* board
);

#endif  // CHESS_BOARD_H
