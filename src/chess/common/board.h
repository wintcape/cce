/**
 * @file board.h
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Defines a datatype to represent the state of a chess board.
 */
#ifndef CHESS_COMMON_BOARD_H
#define CHESS_COMMON_BOARD_H

#include "chess/common/bitboard.h"
#include "chess/common/castle.h"
#include "chess/common/fen.h"
#include "chess/common/piece.h"
#include "chess/common/square.h"

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

#endif  // CHESS_COMMON_BOARD_H
