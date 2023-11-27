/**
 * @file fen.h
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief FEN notation.
 */
#ifndef CHESS_FEN_H
#define CHESS_FEN_H

#include "chess/common.h"

// Preprocessor bindings for FEN literals.
#define FEN_EMPTY   "8/8/8/8/8/8/8/8 w . ."
#define FEN_START   "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
#define FEN_TRICKY  "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"
#define FEN_KILLER  "rnbqkb1r/pp1p1pPp/8/2p1pP2/1P1P4/3P3P/P1P1P3/RNBQKBNR w KQkq e6 0 1"
#define FEN_CMK     "r2q1rk1/ppp2ppp/2n1bn2/2b1p3/3pP3/3P1NPP/PPP1NPB1/R1BQ1RK1 b - - 0 9"

/**
 * @brief Parses a FEN move and updates the provided board.
 * @param fen The FEN string to parse.
 * @param board The board state to mutate.
 * @return false on error, true otherwise.
 */
bool
fen_parse
(   const char* fen
,   board_t*    board
);

#endif  // CHESS_FEN_H
