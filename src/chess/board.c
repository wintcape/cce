/**
 * @author Matthew Weissel (null@mattweissel.info)
 * @file chess.c
 * @brief Implementation of the chess header.
 * (see chess.h for additional details)
 */
#include "chess/board.h"

#include "chess/fen.h"
#include "chess/platform.h"
#include "chess/string.h"

void
board_render
(   char*           dst
,   const board_t*  board
,   const u8        color
)
{
    u64 offs = string_format ( dst
                             , "\n\tBOARD: %s\n"
                             , fen_from_board ( dst , board )
                             );
    platform_console_write ( dst , color );

    u8 i = 0;
    u8 r = 0;
    u8 f = 0;

    while ( r < 8 )
    {
        platform_console_write ( "\n\t\t  " , color );
        
        while ( f < 8 )
        {
            if ( !f )
            {
                string_format ( dst , "%u   " , 8 - r );
                platform_console_write ( dst , color );
            }

            PIECE piece = EMPTY_SQ; 

            for ( PIECE piece_ = P; piece_ < 12; ++piece_ )
            {
                if ( bit ( ( *board ).pieces[ piece_ ]
                         , SQUAREINDX ( r , f )
                         ))
                {
                    piece = piece_;
                }
            }
            
            const bool blackchr = piece >= p && piece <= k;
            
            string_format ( dst
                          , " %s "
                          , ( piece != EMPTY_SQ ) ? ( blackchr ) ? piecewchr ( piece )
                                                                 : piecewchr ( p + piece )
                                                  : " "
                          );

            if ( !( i % 2 ) )
            {
                platform_console_write ( dst
                                       , blackchr ? PLATFORM_COLOR_CHESS_WSBP
                                                  : PLATFORM_COLOR_CHESS_WSWP
                                       );
            }
            else
            {
                platform_console_write ( dst
                                       , blackchr ? PLATFORM_COLOR_CHESS_BSBP
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
    
    offs = string_format ( dst
                         , "\n\n\t\t      "
                         , color
                         );

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
                          , "\n\n\n\t\t  Side:         %s\n\t\t  En passant:      %s\n\t\t  Castling:      %c%c%c%c\n\n"
                          , ( ( *board ).side == WHITE ) ? "white"
                                                         : "black"
                          , ( ( *board ).enpassant != NO_SQ ) ? string_square ( ( *board ).enpassant )
                                                              : "no"
                          , ( ( *board ).castle & CASTLE_WK ) ? 'K' : '-'
                          , ( ( *board ).castle & CASTLE_WQ ) ? 'Q' : '-'
                          , ( ( *board ).castle & CASTLE_BK ) ? 'k' : '-'
                          , ( ( *board ).castle & CASTLE_BQ ) ? 'q' : '-'
                          );

    platform_console_write ( dst , color );
}
