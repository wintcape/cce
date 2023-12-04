/**
 * @file string.h
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Stringify utility.
 */
#ifndef CHESS_STRING_H
#define CHESS_STRING_H

#include "chess/common.h"

#include "core/string.h"

// Defines the number of characters needed to represent a move in string format.
#define MOVE_STRING_LENGTH 5

// Defines the number of characters needed to represent a board square
// coordinate in string format.
#define SQUARE_STRING_LENGTH 2

// Defines a string representation of each square coordinate on a chess board.
static const char* square_coordinate_tags[] = { "A8" , "B8" , "C8" , "D8" , "E8" , "F8" , "G8" , "H8"
                                              , "A7" , "B7" , "C7" , "D7" , "E7" , "F7" , "G7" , "H7"
                                              , "A6" , "B6" , "C6" , "D6" , "E6" , "F6" , "G6" , "H6"
                                              , "A5" , "B5" , "C5" , "D5" , "E5" , "F5" , "G5" , "H5"
                                              , "A4" , "B4" , "C4" , "D4" , "E4" , "F4" , "G4" , "H4"
                                              , "A3" , "B3" , "C3" , "D3" , "E3" , "F3" , "G3" , "H3"
                                              , "A2" , "B2" , "C2" , "D2" , "E2" , "F2" , "G2" , "H2"
                                              , "A1" , "B1" , "C1" , "D1" , "E1" , "F1" , "G1" , "H1"
                                              };

// Defines a map for all pieces: piece -> ASCII alias.
static const char piecechrs[] = { [ P ] = 'P'
                                , [ N ] = 'N'
                                , [ B ] = 'B'
                                , [ R ] = 'R'
                                , [ Q ] = 'Q'
                                , [ K ] = 'K'
                                , [ p ] = 'p'
                                , [ n ] = 'n'
                                , [ b ] = 'b'
                                , [ r ] = 'r'
                                , [ q ] = 'q'
                                , [ k ] = 'k'
                                };

// Defines a map for all pieces: ASCII alias -> piece.
static const PIECE pieces[] = { [ 'P' ] = P
                              , [ 'N' ] = N
                              , [ 'B' ] = B
                              , [ 'R' ] = R
                              , [ 'Q' ] = Q
                              , [ 'K' ] = K
                              , [ 'p' ] = p
                              , [ 'n' ] = n
                              , [ 'b' ] = b
                              , [ 'r' ] = r
                              , [ 'q' ] = q
                              , [ 'k' ] = k
                              };

/**
 * @brief Generates the ASCII alias for a given piece.
 * @param piece The piece to display.
 * @return ASCII char representing the piece.
 */
INLINE
char
piecechr
(   const PIECE piece
)
{
    return piecechrs[ piece ];
}

/**
 * @brief Generates the piece for a given ASCII alias.
 * @param alias ASCII alias for the piece.
 * @return The piece.
 */
INLINE
PIECE
chrpiece
(   const char alias
)
{
    return pieces[ max ( alias ,  0 ) ];
}

/**
 * @brief Stringify square.
 * @param square A square.
 * @return A string representation of the board square coordinate.
 */
INLINE
const char*
string_square
(   const SQUARE square
)
{
    return square_coordinate_tags[ square ];
}

/**
 * @brief Stringify move.
 * @param dst Output buffer. Should have adequate space for exactly six bytes:
 *        _ _ (source square) _ _ (target square) _ (piece promotion) _ (terminator)
 * @param move A single move, encoded as a bitpacked 32-bit unsigned integer.
 * @return dst.
 */
char*
string_move
(   char*           dst
,   const move_t    move
);

/**
 * @brief Stringify moves.
 * @param dst Output buffer.
 * @param move A container of pregenerated moves.
 * @return dst.
 */
char*
string_moves
(   char*           dst
,   const moves_t*  moves
);

/**
 * @brief Stringify bitboard.
 * @param dst Output buffer.
 * @param bitboard A bitboard.
 * @return dst.
 */
char*
string_bitboard
(   char*               dst
,   const bitboard_t    bitboard
);

#endif  // CHESS_STRING_H
