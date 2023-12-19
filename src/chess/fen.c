/**
 * @author Matthew Weissel (null@mattweissel.info)
 * @file fen.c
 * @brief Implementation of the fen header.
 * (see fen.h for additional details)
 */
#include "chess/fen.h"

#include "chess/string.h"

#include "core/logger.h"
#include "core/memory.h"

// Defines FEN parsing tokens.
#define FEN_FILE_SEPARATOR_TOKEN    '/'
#define FEN_WHITESPACE_TOKEN        ' '

bool
fen_parse
(   const char* fen_
,   board_t*    board_
)
{
    // Initialize a new board.
    board_t board;
    board.side = 0;
    board.enpassant = NO_SQ;
    board.castle = 0;
    memory_clear ( board.pieces , sizeof ( board.pieces ) );
    memory_clear ( board.occupancies , sizeof ( board.occupancies ) );
    
    // Copy FEN into a working text buffer.
    char buf[ FEN_STRING_MAX_LENGTH ];
    const u64 len = string_length_clamp ( fen_ , FEN_STRING_MAX_LENGTH );    
    memory_copy ( buf , fen_ , len );
    buf[ len ] = 0;         // Append terminator.
    string_trim ( buf );    // Trim whitespace.
   
    // Read from the working buffer.
    char* fen = buf;

    // Parse piece placements.
    u8 r = 0;
    u8 f = 0;
    while ( r < 8 )
    {
        while ( f < 8 )
        {
            PIECE piece = EMPTY_SQ;

            for ( PIECE piece_ = P; piece_ <= k; ++piece_ )
            {
                if ( *fen == piecechr ( piece_ ) )
                {
                    piece = piece_;
                    break;
                }
            }
            
            // Piece token. 
            if ( piece != EMPTY_SQ )
            {
                BITSET ( board.pieces[ piece ]
                       , SQUAREINDX ( r , f )
                       );

                fen += 1;

                f += 1;
                continue;
            }

            // Empty square token.
            if ( digit ( *fen ) )
            {
                const u8 i = to_digit ( *fen );
                
                // Validate.
                if ( !i || i > 8 - f )
                {
                    LOGERROR ( "fen_parse: Failed to parse move in invalid FEN format:\n\t    %s\n\tIllegal use of numeral '%c' (index %u, file %u)."
                             , buf , *fen , fen - buf , f
                             );
                    return false;
                }

                fen += 1;
                
                f += i;
                continue;
            }

            // Validate.
            LOGERROR ( "fen_parse: Failed to parse move in invalid FEN format:\n\t    %s\n\tIllegal character '%c' (index %u, file %u)."
                     , buf , *fen , fen - buf , f
                     );
            return false;
        }

        // File separator token.
        if ( r < 7 )
        {
            // Validate.
            if ( *fen != FEN_FILE_SEPARATOR_TOKEN )
            {
                LOGERROR ( "fen_parse: Failed to parse move in invalid FEN format:\n\t    %s\n\tFailed on index %u, illegal character '%c' (expected file separator '%c')."
                         , buf , fen - buf , *fen , FEN_FILE_SEPARATOR_TOKEN
                         );
                return false;
            }
        
            fen += 1;
        }

        f = 0;
        r += 1;
    }

    // Validate.
    if (   ( fen[ 0 ] != FEN_WHITESPACE_TOKEN )
        || ( fen[ 1 ] != 'w' && fen[ 1 ] != 'b' )
        || ( fen[ 2 ] != FEN_WHITESPACE_TOKEN )
       )
    {
        LOGERROR ( "fen_parse: Failed to parse move in invalid FEN format:\n\t    %s\n\tFailed on index %u, illegal character '%c'."
                 , buf , fen - buf , *fen
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
                     , buf , fen - buf , *fen
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
                         , buf , fen - buf , *fen
                         );
                return false;
            }
        }

        castle += 1;
        fen += 1;
    }
   
    // Validate.
    if ( fen[ 0 ] != FEN_WHITESPACE_TOKEN )
    {
        LOGERROR ( "fen_parse: Failed to parse move in invalid FEN format:\n\t    %s\n\tFailed on index %u, character '%c'."
                 , buf , fen - buf , *fen
                 );
        return false;
    }
    fen += 1;

    // Parse enpassant token.
    if ( fen[ 0 ] == '-' && fen[ 1 ] == FEN_WHITESPACE_TOKEN )
    {
        board.enpassant = NO_SQ;
        fen += 2;
    }
    else if (   fen[ 0 ] >= 'a' && fen[ 0 ] <= 'h'
             && fen[ 1 ] >= '1' && fen[ 1 ] <= '8'
             && fen[ 2 ] == FEN_WHITESPACE_TOKEN
            )
    {
        board.enpassant = SQUAREINDX ( 8 - to_digit ( fen[ 1 ] )
                                     , fen[ 0 ] - 2 * '0' - 1
                                     );
        LOGINFO ( string_square (board.enpassant));
        fen += 3;
    }

    // Validate.
    else
    {
        LOGERROR ( "fen_parse: Failed to parse move in invalid FEN format:\n\t    %s\n\tFailed on index %u, character '%c'."
                 , buf , fen - buf , *fen
                 );
        return false;
    }

    // Update occupancy maps.
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

    // Preserve board history.
    board.history = ( *board_ ).history;

    // Write the board to the output buffer.
    memory_copy ( board_ , &board , sizeof ( board_t ) );

    return true;
}

char*
fen_from_board
(   char*           dst
,   const board_t*  board
)
{
    char* const dst_ = dst;

    // Stringify piece placements.
    i8 nonempty = -1;
    u8 r = 0;
    u8 f = 0;
    while ( r < 8 )
    {
        while ( f < 8 )
        {
            PIECE piece = EMPTY_SQ;
            for ( PIECE piece_ = P; piece_ <= k; ++piece_ )
            {
                if ( bit ( ( *board ).pieces[ piece_ ]
                         , SQUAREINDX ( r , f )
                         ))
                {
                    piece = piece_;
                    break;
                }
            }

            if ( piece != EMPTY_SQ )
            {
                // Append empty square count token, if necessary.
                const u8 i = f - nonempty - 1;
                if ( i )
                {
                    *dst = char_digit ( i );
                    dst += 1;
                }

                // Append occupied square token.
                nonempty = f;
                *dst = piecechr ( piece );
                dst += 1;
            }

            f += 1;
        }

        // Append empty square count token, if necessary.
        const u8 i = f - nonempty - 1;
        if ( i )
        {
            *dst = char_digit ( i );
            dst += 1;
        }
        
        // Append file separator token, if necessary.
        if ( r < 7 )
        {
            *dst = FEN_FILE_SEPARATOR_TOKEN;
            dst += 1;
        }

        nonempty = -1;
        f = 0;
        r += 1;
    }
    
    dst[ 0 ] = FEN_WHITESPACE_TOKEN;

    // Append side token.
    dst[ 1 ] = ( ( *board ).side == WHITE ) ? 'w' : 'b';
    
    dst[ 2 ] = FEN_WHITESPACE_TOKEN;
    dst += 3;
    
    // Append castle token.
    if ( !( *board ).castle )
    {
        dst[ 0 ] = '-';
        dst += 1;

    }
    else
    {
        dst[ 0 ] = ( ( *board ).castle & CASTLE_WK ) ? 'K' : '-';
        dst[ 1 ] = ( ( *board ).castle & CASTLE_WQ ) ? 'Q' : '-';
        dst[ 2 ] = ( ( *board ).castle & CASTLE_BK ) ? 'k' : '-';
        dst[ 3 ] = ( ( *board ).castle & CASTLE_BQ ) ? 'q' : '-';
        dst += 4;
    }

    dst[ 0 ] = FEN_WHITESPACE_TOKEN;
    dst += 1;

    // Append enpassant token.
    if ( ( *board ).enpassant != NO_SQ )
    {
        memory_copy ( dst
                    , string_square ( ( *board ).enpassant )
                    , SQUARE_STRING_LENGTH
                    );
        *dst = to_lowercase ( *dst );
        dst += SQUARE_STRING_LENGTH;
    }
    else
    {
        dst[ 0 ] = '-';
        dst += 1;
    }
    
    *dst = 0;   // Append terminator.
    
    return dst_;
}
