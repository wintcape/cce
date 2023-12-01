/**
 * @author Matthew Weissel (null@mattweissel.info)
 * @file fen.c
 * @brief Implementation of the fen header.
 * (see fen.h for additional details)
 */
#include "chess/fen.h"

#include "chess/string.h"

#include "core/logger.h"
#include "core/string.h"

// Defines FEN parsing tokens.
#define FEN_FILE_SEPARATOR_TOKEN    '/'
#define FEN_FILE_WHITESPACE_TOKEN   ' '

bool
fen_parse
(   const char* fen
,   board_t*    buf
)
{
    board_t board;
    
    // Set latest FEN for the board.
    const u64 len = string_length_clamp ( fen , FEN_STRING_MAX_LENGTH );
    memory_copy ( board.fen , fen , len );
    board.fen[ len ] = 0;

    memory_clear ( board.pieces
                 , sizeof ( board.pieces )
                 );
    memory_clear ( board.occupancies
                 , sizeof ( board.occupancies )
                 );

    board.side = 0;
    board.enpassant = NO_SQ;
    board.castle = 0;
    
    const char* const fen_ = fen;

    // Trim whitespace.
    string_trim ( board.fen );

    // Parse piece placements.
    for ( u8 r = 0; r < 8; ++r )
    {
        for ( u8 f = 0; f < 8; ++f )
        {
            // Piece token. 
            PIECE piece = EMPTY_SQ;
            for ( PIECE piece_ = P; piece_ <= k; ++piece_ )
            {
                if ( *fen == piecechr ( piece_ ) )
                {
                    piece = piece_;
                    break;
                }
            }
            if ( piece != EMPTY_SQ )
            {
                BITSET ( board.pieces[ piece ]
                       , SQUAREINDX ( r , f )
                       );

                fen += 1;
            }

            // Empty square token.
            else if ( numeric ( *fen ) )
            {
                const u8 i = chri ( *fen );
                
                // Validate.
                if ( !i || i > 8 - f )
                {
                    LOGERROR ( "fen_parse: Failed to parse move in invalid FEN format:\n\t    %s\n\tIllegal use of numeral '%c' (index %u, file %u)."
                             , board.fen
                             , *fen
                             , fen - fen_
                             , f
                             );
                    return false;
                }

                f += i - 1;
                fen += 1;
            }

            // File separator token.
            if ( r < 7 && *fen == FEN_FILE_SEPARATOR_TOKEN )
            {
                // Validate.
                if ( f != 7 )
                {
                    LOGERROR ( "fen_parse: Failed to parse move in invalid FEN format:\n\t    %s\n\tIllegal use of file separator '%c' (index %u, file %u)."
                             , board.fen
                             , *fen
                             , fen - fen_
                             , f
                             );
                    return false;
                }

                fen += 1;
            }
        }
    }

    // Validate.
    if (   ( fen[ 0 ] != FEN_FILE_WHITESPACE_TOKEN )
        || ( fen[ 1 ] != 'w' && fen[ 1 ] != 'b' )
        || ( fen[ 2 ] != FEN_FILE_WHITESPACE_TOKEN )
       )
    {
        LOGERROR ( "fen_parse: Failed to parse move in invalid FEN format:\n\t    %s\n\tFailed on index %u, illegal character '%c'."
                 , board.fen
                 , fen - fen_
                 , *fen
                 );
        return false;
    }

    // Parse side.
    board.side = ( fen[ 1 ] == 'w' ) ? WHITE : BLACK;
    fen += 3;

    // Parse castling rights.
    u8 castle = 0;
    while ( !whitespace ( *fen ) )
    {
        // Validate.
        if ( castle >= 4 )
        {
            LOGERROR ( "fen_parse: Failed to parse move in invalid FEN format:\n\t    %s\n\tFailed on index %u, illegal character '%c'."
                     , board.fen
                     , fen - fen_
                     , *fen
                     );
            return false;
        }

        switch ( *fen )
        {
            case 'K':   board.castle |= CASTLE_WK; break;
            case 'Q':   board.castle |= CASTLE_WQ; break;
            case 'k':   board.castle |= CASTLE_BK; break;
            case 'q':   board.castle |= CASTLE_BQ; break;
            case '-':                              break;

            // Validate.
            default:
            {
                LOGERROR ( "fen_parse: Failed to parse move in invalid FEN format:\n\t    %s\n\tFailed on index %u, character '%c'."
                         , board.fen
                         , fen - fen_
                         , *fen
                         );
                return false;
            }
        }

        castle += 1;
        fen += 1;
    }
   
    // Validate.
    if ( fen[ 0 ] != FEN_FILE_WHITESPACE_TOKEN )
    {
        LOGERROR ( "fen_parse: Failed to parse move in invalid FEN format:\n\t    %s\n\tFailed on index %u, character '%c'."
                 , board.fen
                 , fen - fen_
                 , *fen
                 );
        return false;
    }
    fen += 1;

    // Parse enpassant token.
    if ( fen[ 0 ] == '-' && fen[ 1 ] == FEN_FILE_WHITESPACE_TOKEN )
    {
        board.enpassant = NO_SQ;
        fen += 2;
    }
    else if (   fen[ 0 ] >= 'a' && fen[ 0 ] <= 'h'
             && fen[ 1 ] >= '1' && fen[ 1 ] <= '8'
             && fen[ 2 ] == FEN_FILE_WHITESPACE_TOKEN
            )
    {
        board.enpassant = SQUAREINDX ( 8 - chri ( fen[ 1 ] )
                                     , fen[ 0 ] - 2 * '0' - 1
                                     );
        fen += 3;
    }

    // Validate.
    else
    {
        LOGERROR ( "fen_parse: Failed to parse move in invalid FEN format:\n\t    %s\n\tFailed on index %u, character '%c'."
                 , board.fen
                 , fen - fen_
                 , *fen
                 );
        return false;
    }

    // Write the new occupancy maps to the board.
    for ( PIECE piece = P; piece <= K; ++piece )
    {
        board.occupancies[ WHITE ] |= board.pieces[ piece ];
    }
    for ( PIECE piece = p; piece <= k; ++piece )
    {
        board.occupancies[ BLACK ] |= board.pieces[ piece ];
    }
    board.occupancies[ 2 ] = board.occupancies[ WHITE ]
                           | board.occupancies[ BLACK ]
                           ;

    // Write the board to the output buffer.
    memory_copy ( buf , &board , sizeof ( board_t ) );

    return true;
}
