/**
 * @author Matthew Weissel (null@mattweissel.info)
 * @file chess.c
 * @brief Implementation of the chess header.
 * (see chess.h for additional details)
 */
#include "chess/board.h"

#include "chess/platform.h"

#include "core/string.h"

// Defaults.
#define CHESS_BOARD_BUFFER_LENGTH   512

void
render_chess_board
(   const board_t* board
)
{
    char buf[ CHESS_BOARD_BUFFER_LENGTH ];
    u64 offs = 0;

    u8 r = 0;
    u8 f = 0;
    u8 i = 0;

    while ( r < 8 )
    {
        platform_console_write ( "\n\t" , PLATFORM_COLOR_CHESS_INFO );
        
        while ( f < 8 )
        {
            if ( !f )
            {
                offs = string_format ( buf , "%u   " , 8 - r );
                buf[ offs ] = 0;
                platform_console_write ( buf , PLATFORM_COLOR_CHESS_INFO );
            }

            PIECE piece = EMPTY_SQ; 

            for ( PIECE bb = P; bb < 12; ++bb )
            {
                if ( bit ( ( *board ).pieces[ bb ] , SQUAREINDX ( r , f ) ) )
                {
                    piece = bb;
                }
            }

            if ( !( i % 2 ) )
            { 
                if ( piece >= P && piece <= K )
                {
                    buf[ 0 ] = ' ';
                    buf[ 1 ] = piecechr ( piece );
                    buf[ 2 ] = ' ';
                    buf[ 3 ] = 0;
                    platform_console_write ( buf , PLATFORM_COLOR_CHESS_WSWP );
                }
                else if ( piece >= p && piece <= k )
                {
                    buf[ 0 ] = ' ';
                    buf[ 1 ] = piecechr ( piece );
                    buf[ 2 ] = ' ';
                    buf[ 3 ] = 0;
                    platform_console_write ( buf , PLATFORM_COLOR_CHESS_WSBP );
                }
                else
                {
                    buf[ 0 ] = ' ';
                    buf[ 1 ] = ' ';
                    buf[ 2 ] = ' ';
                    buf[ 3 ] = 0;
                    platform_console_write ( buf , PLATFORM_COLOR_CHESS_WSWP );
                }
            }
            else
            {
                if ( piece >= P && piece <= K )
                {
                    buf[ 0 ] = ' ';
                    buf[ 1 ] = piecechr ( piece );
                    buf[ 2 ] = ' ';
                    buf[ 3 ] = 0;
                    platform_console_write ( buf , PLATFORM_COLOR_CHESS_BSWP );
                }
                else if ( piece >= p && piece <= k )
                {
                    buf[ 0 ] = ' ';
                    buf[ 1 ] = piecechr ( piece );
                    buf[ 2 ] = ' ';
                    buf[ 3 ] = 0;
                    platform_console_write ( buf , PLATFORM_COLOR_CHESS_BSBP );
                }
                else
                {
                    buf[ 0 ] = ' ';
                    buf[ 1 ] = ' ';
                    buf[ 2 ] = ' ';
                    buf[ 3 ] = 0;
                    platform_console_write ( buf , PLATFORM_COLOR_CHESS_BSWP );
                }                
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
        offs += string_format ( buf + offs
                              , " %c "
                              , 'A' + r
                              );
        r += 1;
    }

    offs += string_format ( buf + offs
                          , "\n\n\n\tFEN:          %s\n\tSide:         %s\n\tEn passant:      %s\n\tCastling:      %c%c%c%c\n\n"
                          , ( *board ).fen
                          , ( ( *board ).side == WHITE ) ? "white"
                                                         : "black"
                          , ( ( *board ).enpassant != NO_SQ ) ? string_bitboard_square ( ( *board ).enpassant )
                                                              : "no"
                          , ( ( *board ).castle & CASTLE_WK ) ? 'K' : '-'
                          , ( ( *board ).castle & CASTLE_WQ ) ? 'Q' : '-'
                          , ( ( *board ).castle & CASTLE_BK ) ? 'k' : '-'
                          , ( ( *board ).castle & CASTLE_BQ ) ? 'q' : '-'
                          );
    buf[ offs ] = 0;

    platform_console_write ( buf , PLATFORM_COLOR_CHESS_INFO );
}
