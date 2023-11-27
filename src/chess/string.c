/**
 * @author Matthew Weissel (null@mattweissel.info)
 * @file string.c
 * @brief Implementation of the string header.
 * (see string.h for additional details)
 */
#include "chess/string.h"

#include "core/string.h"

u64
string_bitboard
(   char*               dst
,   const bitboard_t    bitboard
)
{
    u64 offs = string_format ( dst
                             , "  BITBOARD:  %llu\n"
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
                                  , "%u"
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
                          , "%c"
                          , 'A' + r
                          );
    }
    
    offs += string_format ( dst + offs , "\n\n" );
    
    return offs;
}
