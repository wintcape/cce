/**
 * @file piece.h
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Datatypes and operations to represent a single piece on a chess board.
 */
#ifndef CHESS_PIECE_H
#define CHESS_PIECE_H

#include "common.h"

#include "math/math.h"

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
typedef enum
{
    P , N , B , R , Q , K
,   p , n , b , r , q , k

,   EMPTY_SQ    // Extra piece for detecting an empty board square.
}
PIECE;

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

#endif  // CHESS_PIECE_H
