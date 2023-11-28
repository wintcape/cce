/**
 * @author Matthew Weissel (null@mattweissel.info)
 * @file fen.c
 * @brief Implementation of the fen header.
 * (see fen.h for additional details)
 */
#include "chess/fen.h"

#include "core/logger.h"
#include "core/string.h"

// Defines FEN file separator token.
#define FEN_FILE_SEPARATOR_TOKEN '/'

void
fen_parse
(   const char* fen
,   board_t*    board
)
{
    // Set latest FEN.
    const u64 len = min ( string_length ( fen )
                        , ( u64 ) FEN_STRING_MAX_LENGTH
                        );
    memory_copy ( ( *board ).fen , fen , len );
    ( *board ).fen[ len ] = 0;

    // Parse board.
    memory_clear ( ( *board ).pieces
                 , sizeof ( ( *board ).pieces )
                 );
    memory_clear ( ( *board ).occupancies
                 , sizeof ( ( *board ).occupancies )
                 );

    ( *board ).side = 0;
    ( *board ).enpassant = NO_SQ;
    ( *board ).castle = 0;

    for ( u8 r = 0; r < 8; ++r )
    {
        for ( u8 f = 0; f < 8; ++f )
        {
            if ( alpha ( *fen ) )
            {
                PIECE piece = chrpiece ( *fen );
                BITSET ( ( *board ).pieces[ piece ]
                       , SQUAREINDX ( r , f )
                       );

                fen += 1;
            }

            if ( numeric ( *fen ) )
            {
                PIECE piece = EMPTY_SQ;
                for ( u8 i = P; i < 12; ++i )
                {
                    if ( bit ( ( *board ).pieces[ i ]
                               , SQUAREINDX ( r , f )
                               ))
                    {
                        piece = i;
                        break;
                    }
                }

                if ( piece == EMPTY_SQ )
                {
                    f -= 1;
                }
                f += *fen - '0';

                fen += 1;
            }

            if ( *fen == FEN_FILE_SEPARATOR_TOKEN )
            {
                fen += 1;
            }
        }
    }
    fen += 1;

    // Parse side.
    ( *board ).side = ( *fen == 'w' ) ? WHITE : BLACK;
    fen += 2;

    // Parse castling rights.
    while ( !whitespace ( *fen ) )
    {
        switch ( *fen )
        {
            case 'K':   ( *board ).castle |= CASTLE_WK; break;
            case 'Q':   ( *board ).castle |= CASTLE_WQ; break;
            case 'k':   ( *board ).castle |= CASTLE_BK; break;
            case 'q':   ( *board ).castle |= CASTLE_BQ; break;
            case '-':                                   break;
        }
        fen += 1;
    }

    fen += 1;
    
    // Parse enpassant.
    ( *board ).enpassant = ( *fen != '-' ) ? SQUAREINDX ( 8 - ( fen[ 1 ] - '0' )
                                                        , fen[ 0 ] - 2 * '0' - 1
                                                        )
                                           : NO_SQ
                                           ;

    // Parse occupancies.
    for ( PIECE piece = P; piece <= K; ++piece )
    {
        ( *board ).occupancies[ WHITE ] |= ( *board ).pieces[ piece ];
    }
    for ( PIECE piece = p; piece <= k; ++piece )
    {
        ( *board ).occupancies[ BLACK ] |= ( *board ).pieces[ piece ];
    }
    ( *board ).occupancies[ 2 ] = ( *board ).occupancies[ WHITE ]
                                | ( *board ).occupancies[ BLACK ]
                                ;
}
