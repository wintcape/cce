/**
 * @author Matthew Weissel (null@mattweissel.info)
 * @file chess.c
 * @brief Implementation of the chess header.
 * (see chess.h for additional details)
 */
#include "chess/board.h"

#include "core/string.h"

// Defaults.
#define CHESS_BOARD_STRING_MAX_LENGTH 1024

char*
string_chess_board
(   const board_t* board
)
{
    char buf[ CHESS_BOARD_STRING_MAX_LENGTH ];
    
    u64 offs = string_format ( buf
                             , "\n\tFEN:    %s\n"
                             , ( *board ).fen
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

            PIECE piece = EMPTY_SQ; 

            for ( PIECE bb = P; bb < 12; ++bb )
            {
                if ( bit ( ( *board ).pieces[ bb ] , SQUAREINDX ( r , f ) ) )
                {
                    piece = bb;
                }
            }

            offs += string_format ( buf + offs
                                  , "%c"
                                  , ( piece == EMPTY_SQ ) ? '.'
                                                          : piecechr ( piece )
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

    offs += string_format ( buf + offs
                          , "\n\n\n\tSide:         %s\n\tEnpassant:       %s\n\tCastling:      %c%c%c%c\n"
                          , ( ( *board ).side == WHITE ) ? "white"
                                                         : "black"
                          , ( ( *board ).enpassant != NO_SQ ) ? string_bitboard_square ( ( *board ).enpassant )
                                                              : "no"
                          , ( ( *board ).castle & CASTLE_WK ) ? 'K' : '-'
                          , ( ( *board ).castle & CASTLE_WQ ) ? 'Q' : '-'
                          , ( ( *board ).castle & CASTLE_BK ) ? 'k' : '-'
                          , ( ( *board ).castle & CASTLE_BQ ) ? 'q' : '-'
                          );

    return string_allocate_from ( buf );
}
