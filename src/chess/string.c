/**
 * @author Matthew Weissel (null@mattweissel.info)
 * @file string.c
 * @brief Implementation of the string header.
 * (see string.h for additional details)
 */
#include "chess/string.h"

char*
string_move
(   char*           dst
,   const move_t    move
)
{
    memory_copy ( &dst[ 0 ]
                , string_square ( move_decode_source_square ( move ) )
                , 2
                );
    memory_copy ( &dst[ 2 ]
                , string_square ( move_decode_target_square ( move ) )
                , 2
                );
    dst[ MOVE_STRING_LENGTH - 1 ] = move_decode_promotion ( move ) ? touppercase ( piecechr ( move_decode_promotion ( move ) ) )
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
                             , " MOVES:    move      piece    capture?  double?   enpassant?  castle?\n\n\t"
                             );

    for ( u32 i = 0; i < ( *moves ).count; ++i )
    {
        const move_t move = ( *moves ).moves[ i ];
        offs += string_format ( dst + offs
                              ,  "   %s      %c        %u         %u         %u           %u\n\t"
                              , string_move ( dst + offs , move )
                              , touppercase ( piecechr ( move_decode_piece ( move ) ) )
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
