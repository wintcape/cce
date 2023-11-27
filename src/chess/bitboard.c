/**
 * @author Matthew Weissel (null@mattweissel.info)
 * @file bitboard.c
 * @brief Implementation of the bitboard header.
 * (see bitboard.h for additional details)
 */
#include "chess/bitboard.h"

#include "chess/board.h"

#include "core/string.h"

// Defaults.
#define BITBOARD_STRING_MAX_LENGTH 256

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

    for ( u8 r = 0; r < 8; r++ )
    {
        for ( u8 f = 0; f < 8; f++ )
        {
            if ( chess_board_square_attackable ( board
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

char*
string_bitboard
(   const bitboard_t bitboard
)
{
    char buf[ BITBOARD_STRING_MAX_LENGTH ];

    u64 offs = string_format ( buf
                             , "BITBOARD:  %llu\n"
                             , bitboard
                             );

    u8 r = 0;
    u8 f = 0;

    while ( r < 8 )
    {
        offs += string_format ( buf + offs
                              , "\n\t"
                              );
        while ( f < 8 )
        {
            if ( !f )
            {
                offs += string_format ( buf + offs
                                      , "%u   "
                                      , 8 - r
                                      );
            }

            offs += string_format ( buf + offs
                                  , "%u"
                                  , bit ( bitboard , SQUAREINDX ( r , f ) )
                                  );
            
            f += 1;
        }

        f = 0;
        r += 1;
    }
    
    offs += string_format ( buf + offs
                          , "\n\n\t    "
                          );

    r = 0;
    while ( r < 8 )
    {
        offs += string_format ( buf + offs
                          , "%c"
                          , 'A' + r
                          );
        r += 1;
    }
    
    offs += string_format ( buf + offs , "\n\n" );
    
    return string_allocate_from ( buf );
}
