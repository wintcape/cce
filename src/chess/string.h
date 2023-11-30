/**
 * @file string.h
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Stringify utility.
 */
#ifndef CHESS_STRING_H
#define CHESS_STRING_H

#include "chess/common.h"

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

// Defines an ASCII alias for each piece.
static const char piecechrs[] = { 'P' , 'N' , 'B' , 'R' , 'Q' , 'K'
                                , 'p' , 'n' , 'b' , 'r' , 'q' , 'k'
                                };

// Defines each piece and alias.
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
string_board_square
(   const SQUARE square
)
{
    return square_coordinate_tags[ square ];
}

/**
 * @brief Stringify bitboard.
 * @param dst Output buffer.
 * @param bitboard A bitboard.
 * @return The number of characters written to the buffer.
 */
u64
string_bitboard
(   char*               dst
,   const bitboard_t    bitboard
);

#endif  // CHESS_STRING_H
