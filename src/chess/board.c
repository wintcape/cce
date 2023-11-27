/**
 * @author Matthew Weissel (null@mattweissel.info)
 * @file chess.c
 * @brief Implementation of the chess header.
 * (see chess.h for additional details)
 */
#include "chess/board.h"

#include "chess/platform.h"
#include "chess/string.h"

#include "core/string.h"

void
chess_board_render
(   char*           dst
,   const board_t*  board
)
{
    u64 offs = 0;

    u8 i = 0;
    u8 r = 0;
    u8 f = 0;

    while ( r < 8 )
    {
        platform_console_write ( "\n\t" , PLATFORM_COLOR_CHESS_INFO );
        
        while ( f < 8 )
        {
            if ( !f )
            {
                offs = string_format ( dst , "%u   " , 8 - r );
                platform_console_write ( dst , PLATFORM_COLOR_CHESS_INFO );
            }

            PIECE piece = EMPTY_SQ; 

            for ( PIECE bb = P; bb < 12; ++bb )
            {
                if ( bit ( ( *board ).pieces[ bb ] , SQUAREINDX ( r , f ) ) )
                {
                    piece = bb;
                }
            }

            dst[ 0 ] = ' ';
            dst[ 1 ] = ( piece == EMPTY_SQ ) ? ' '
                                             : piecechr ( piece )
                                             ;
            dst[ 2 ] = ' ';
            dst[ 3 ] = 0;

            if ( !( i % 2 ) )
            {
                platform_console_write ( dst
                                       , ( piece >= p && piece <= k ) ? PLATFORM_COLOR_CHESS_WSBP
                                                                      : PLATFORM_COLOR_CHESS_WSWP
                                       );
            }
            else
            {
                platform_console_write ( dst
                                       , ( piece >= p && piece <= k ) ? PLATFORM_COLOR_CHESS_BSBP
                                                                      : PLATFORM_COLOR_CHESS_BSWP
                                       );
            }
            
            f += 1;
            i += 1;
        }

        f = 0;
        r += 1;
        i += 1;
    }
    
    platform_console_write ( "\n\n\t    " , PLATFORM_COLOR_CHESS_INFO );

    offs = 0;
    r = 0;
    while ( r < 8 )
    {
        offs += string_format ( dst + offs
                              , " %c "
                              , 'A' + r
                              );
        r += 1;
    }

    offs += string_format ( dst + offs
                          , "\n\n\n\tFEN:          %s\n\tSide:         %s\n\tEn passant:      %s\n\tCastling:      %c%c%c%c\n\n\n"
                          , ( *board ).fen
                          , ( ( *board ).side == WHITE ) ? "white"
                                                         : "black"
                          , ( ( *board ).enpassant != NO_SQ ) ? string_chess_board_square ( ( *board ).enpassant )
                                                              : "no"
                          , ( ( *board ).castle & CASTLE_WK ) ? 'K' : '-'
                          , ( ( *board ).castle & CASTLE_WQ ) ? 'Q' : '-'
                          , ( ( *board ).castle & CASTLE_BK ) ? 'k' : '-'
                          , ( ( *board ).castle & CASTLE_BQ ) ? 'q' : '-'
                          );

    platform_console_write ( dst , PLATFORM_COLOR_CHESS_INFO );
}