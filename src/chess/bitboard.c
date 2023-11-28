/**
 * @author Matthew Weissel (null@mattweissel.info)
 * @file bitboard.c
 * @brief Implementation of the bitboard header.
 * (see bitboard.h for additional details)
 */
#include "chess/bitboard.h"

#include "chess/board.h"

// Defaults.
#define BITBOARD_STRING_BUFFER_LENGTH   \
    ( CHESS_RENDER_BUFFER_LENGTH >> 2 )

bitboard_t
bitboard_attackable
(   const board_t*      board
,   const attacks_t*    attacks
,   const SIDE          side
)
{
    if ( !board || !attacks )
    {
        return 0;
    }

    bitboard_t attackable = 0;

    for ( u8 r = 0; r < 8; ++r )
    {
        for ( u8 f = 0; f < 8; ++f )
        {
            if ( board_square_attackable ( board
                                         , attacks
                                         , SQUAREINDX ( r , f )
                                         , side
                                         ))
            {
                BITSET ( attackable , SQUAREINDX ( r , f ) );
            }
        }
    }

    return attackable;
}
