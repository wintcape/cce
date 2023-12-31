/**
 * @author Matthew Weissel (null@mattweissel.info)
 * @file string.c
 * @brief Implementation of the string header.
 * (see string.h for additional details)
 */
#include "chess/string.h"

#include "chess/fen.h"

char*
string_move
(   char*           dst
,   const move_t    move
)
{
    memory_copy ( &dst[ 0 ]
                , string_square ( move_decode_src ( move ) )
                , SQUARE_STRING_LENGTH
                );
    memory_copy ( &dst[ SQUARE_STRING_LENGTH ]
                , string_square ( move_decode_dst ( move ) )
                , SQUARE_STRING_LENGTH
                );
    dst[ MOVE_STRING_LENGTH - 1 ] = move_decode_promotion ( move ) ? to_uppercase ( piecechr ( move_decode_promotion ( move ) ) )
                                                                   : ' '
                                                                   ;
    dst[ MOVE_STRING_LENGTH ] = 0;  // Append terminator.
    return dst;
}

char*
string_moves
(   char*           dst
,   const moves_t*  moves
)
{
    if ( !( *moves ).count )
    {
        *dst = 0;   // Append terminator.
        return dst;
    }

    u64 offs = string_format ( dst
                             , " MOVES:    move      piece    capture?  double?   enpassant?  castle?\n\n"
                             );

    for ( u32 i = 0; i < ( *moves ).count; ++i )
    {
        const move_t move = ( *moves ).moves[ i ];
        offs += string_format ( dst + offs
                              ,  "           %s      %s        %u         %u         %u           %u\n"
                              , string_move ( dst + offs , move )
                              , piecewchr ( move_decode_piece ( move ) )
                              , move_decode_capture ( move )
                              , move_decode_double_push ( move )
                              , move_decode_enpassant ( move )
                              , move_decode_castle ( move )
                              );
    }
    
    offs += string_format ( dst + offs
                          , "\n\t   Move count:  %u\n\n"
                          , ( *moves ).count
                          );

    return dst;
}

char*
string_bitboard
(   char*               dst
,   const bitboard_t    bitboard
)
{
    u64 offs = string_format ( dst
                             , " BITBOARD:  0x%X\n"
                             , bitboard
                             );

    for ( u8 r = 0; r < 8; ++r )
    {
        offs += string_format ( dst + offs
                              , "\n\t"
                              );
        for ( u8 f = 0; f < 8; ++f )
        {
            if ( !f )
            {
                offs += string_format ( dst + offs
                                      , "%u   "
                                      , 8 - r
                                      );
            }
            offs += string_format ( dst + offs
                                  , " %u "
                                  , bit ( bitboard , SQUAREINDX ( r , f ) )
                                  );
        }
    }
    
    offs += string_format ( dst + offs
                          , "\n\n\t    "
                          );

    for ( u8 r = 0; r < 8; ++r )
    {
        offs += string_format ( dst + offs
                          , " %c "
                          , 'A' + r
                          );
    }
    
    offs += string_format ( dst + offs , "\n\n" );
    
    return dst;
}

char*
string_board
(   char*           dst
,   const board_t*  board
)
{
    u64 offs = string_format ( dst
                             , "\n\tBOARD: %s\n"
                             , fen_from_board ( dst , board )
                             );

    u8 i = 0;
    u8 r = 0;
    u8 f = 0;

    while ( r < 8 )
    {
        offs += string_format ( dst  + offs , "\n\t\t  " );
        
        while ( f < 8 )
        {
            if ( !f )
            {
                offs += string_format ( dst + offs , "%u   " , 8 - r );
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
            offs += string_format ( dst + offs , " %c " , piecechr ( piece ) );

            f += 1;
            i += 1;
        }

        f = 0;
        r += 1;
        i += 1;
    }
    
    offs += string_format ( dst + offs , "\n\n\t\t      " );

    r = 0;
    while ( r < 8 )
    {
        offs += string_format ( dst + offs , " %c ", 'A' + r );
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

    return dst;
}
