/**
 * @author Matthew Weissel (null@mattweissel.info)
 * @file bitboard.c
 * @brief Implementation of the bitboard header.
 * (see bitboard.h for additional details)
 */
#include "chess/bitboard.h"

#include "core/string.h"

// Defaults.
#define BITBOARD_STRING_MAX_LENGTH 256

bitboard_t
bitboard_mask_attack_with_occupancy
(   const int   occupancy
,   bitboard_t  attack 
,   const u8    attack_relevant_count
)
{
    bitboard_t mask = 0;
    for ( u8 i = 0; i < attack_relevant_count; ++i )
    {
        SQUARE square = bitboard_lsb_indx ( attack );
        if ( occupancy & ( 1 << i ) )
        {
            BITSET ( mask , square );
        }
        BITCLR ( attack , square );
    }
    return mask;
}

bitboard_t
bitboard_mask_pawn_attack
(   const SIDE      side
,   const SQUARE    square
)
{
    bitboard_t mask = 0;
    
    // Set the piece.
    bitboard_t b = bitset ( 0 , square );

    // Set attack options.
    if ( side == WHITE )
    {
        if ( ( b >> 7 ) & BITBOARD_MASK_FILE_A )
        {
            mask |= ( b >> 7 );
        }
        if ( ( b >> 9 ) & BITBOARD_MASK_FILE_H )
        {
            mask |= ( b >> 9 );
        } 
    }
    else
    {
        if ( ( b << 9 ) & BITBOARD_MASK_FILE_A )
        {
            mask |= ( b << 9 );
        }
        if ( ( b << 7 ) & BITBOARD_MASK_FILE_H )
        {
            mask |= ( b << 7 );
        }
    }

    return mask;
}

bitboard_t
bitboard_mask_knight_attack
(   const SQUARE square
)
{
    bitboard_t mask = 0; 
    
    // Set the piece.
    bitboard_t b = bitset ( 0 , square );

    // Set attack options.
    if ( ( b >> 15 ) & BITBOARD_MASK_FILE_A )
    {
        mask |= ( b >> 15 );
    }
    if ( ( b >> 17 ) & BITBOARD_MASK_FILE_H )
    {
        mask |= ( b >> 17 );
    }
    if ( ( b >> 6 ) & BITBOARD_MASK_FILE_AB )
    {
        mask |= ( b >> 6 );
    }
    if ( ( b >> 10 ) & BITBOARD_MASK_FILE_HG )
    {
        mask |= ( b >> 10 );
    }
    if ( ( b << 17 ) & BITBOARD_MASK_FILE_A )
    {
        mask |= ( b << 17 );
    }
    if ( ( b << 15 ) & BITBOARD_MASK_FILE_H )
    {
        mask |= ( b << 15 );
    }
    if ( ( b << 10 ) & BITBOARD_MASK_FILE_AB )
    {
        mask |= ( b << 10 );
    }
    if ( ( b << 6 ) & BITBOARD_MASK_FILE_HG )
    {
        mask |= ( b << 6 );
    }

    return mask;
}

bitboard_t
bitboard_mask_bishop_attack
(   const SQUARE square
)
{
    bitboard_t mask = 0; 

    // Target rank and file.
    const i8 r_target = square / 8;
    const i8 f_target = square % 8;
    
    // Set attack options.
    i8 r;
    i8 f;
    for ( r = r_target + 1 , f = f_target + 1; r <= 6 && f <= 6; ++r , ++f )
    {
        BITSET ( mask , SQUAREINDX ( r , f ) );
    }
    for ( r = r_target - 1 , f = f_target + 1; r >= 1 && f <= 6; --r , ++f )
    {
        BITSET ( mask , SQUAREINDX ( r , f ) );
    }
    for ( r = r_target + 1 , f = f_target - 1; r <= 6 && f >= 1; ++r , --f )
    {
        BITSET ( mask , SQUAREINDX ( r , f ) );
    }
    for ( r = r_target - 1 , f = f_target - 1; r >= 1 && f >= 1; --r , --f )
    {
        BITSET ( mask , SQUAREINDX ( r , f ) );
    }

    return mask;
}

bitboard_t
bitboard_mask_bishop_attack_with_block
(   const SQUARE        square
,   const bitboard_t    block
)
{
    bitboard_t mask = 0; 

    // Target rank and file.
    const i8 r_target = square / 8;
    const i8 f_target = square % 8;
    
    // Set attack options.
    i8 r;
    i8 f;
    for ( r = r_target + 1 , f = f_target + 1; r <= 7 && f <= 7; ++r , ++f )
    {
        BITSET ( mask , SQUAREINDX ( r , f ) );
        if ( bitset ( 0 , SQUAREINDX ( r , f ) ) & block )
        {
            break;
        }
    }
    for ( r = r_target - 1 , f = f_target + 1; r >= 0 && f <= 7; --r , ++f )
    {
        BITSET ( mask , r * 8 + f );
        if ( bitset ( 0 , SQUAREINDX ( r , f ) ) & block )
        {
            break;
        }
    }
    for ( r = r_target + 1 , f = f_target - 1; r <= 7 && f >= 0; ++r , --f )
    {
        BITSET ( mask , SQUAREINDX ( r , f ) );
        if ( bitset ( 0 , SQUAREINDX ( r , f ) ) & block )
        {
            break;
        }
    }
    for ( r = r_target - 1 , f = f_target - 1; r >= 0 && f >= 0; --r , --f )
    {
        BITSET ( mask , SQUAREINDX ( r , f ) );
        if ( bitset ( 0 , SQUAREINDX ( r , f ) ) & block )
        {
            break;
        }
    }

    return mask;
}

bitboard_t
bitboard_mask_rook_attack
(   const SQUARE square
)
{
    bitboard_t mask = 0;

    // Target rank and file.
    const i8 r_target = square / 8;
    const i8 f_target = square % 8;
    
    // Set attack options.
    i8 r;
    i8 f;
    for ( r = r_target + 1; r <= 6; ++r )
    {
        BITSET ( mask , SQUAREINDX ( r , f_target ) );
    }
    for ( r = r_target - 1; r >= 1; --r )
    {
        BITSET ( mask , SQUAREINDX ( r , f_target ) );
    }
    for ( f = f_target + 1; f <= 6; ++f )
    {
        BITSET ( mask , SQUAREINDX ( r_target , f ) );
    }
    for ( f = f_target - 1; f >= 1; --f )
    {
        BITSET ( mask , SQUAREINDX ( r_target , f ) );
    }

    return mask;
}

bitboard_t
bitboard_mask_rook_attack_with_block
(   const SQUARE        square
,   const bitboard_t    block
)
{
    bitboard_t mask = 0; 

    // Target rank and file.
    const i8 r_target = square / 8;
    const i8 f_target = square % 8;
    
    // Set attack options.
    i8 r;
    i8 f;
    for ( r = r_target + 1; r <= 7; ++r )
    {
        BITSET ( mask , SQUAREINDX ( r , f_target ) );
        if ( bitset ( 0 , SQUAREINDX ( r , f_target ) ) & block )
        {
            break;
        }
    }
    for ( r = r_target - 1; r >= 0; --r )
    {
        BITSET ( mask , SQUAREINDX ( r , f_target ) );
        if ( bitset ( 0 , SQUAREINDX ( r , f_target ) ) & block )
        {
            break;
        }
    }
    for ( f = f_target + 1; f <= 7; ++f )
    {
        BITSET ( mask , SQUAREINDX ( r_target , f ) );
        if ( bitset ( 0 , SQUAREINDX ( r_target , f ) ) & block )
        {
            break;
        }
    }
    for ( f = f_target - 1; f >= 0; --f )
    {
        BITSET ( mask , SQUAREINDX ( r_target , f ) );
        if ( bitset ( 0 , SQUAREINDX ( r_target , f ) ) & block )
        {
            break;
        }
    }

    return mask;
}

bitboard_t
bitboard_mask_king_attack
(   const SQUARE square
)
{
    bitboard_t mask = 0; 
    
    // Set the piece.
    bitboard_t b = bitset ( 0 , square );

    // Set attack options.
    if ( b >> 8 )
    {
        mask |= ( b >> 8 );
    }
    if ( ( b >> 7 ) & BITBOARD_MASK_FILE_A )
    {
        mask |= ( b >> 7 );
    }
    if ( ( b >> 1 ) & BITBOARD_MASK_FILE_H )
    {
        mask |= ( b >> 1 );
    }
    if ( ( b >> 9 ) & BITBOARD_MASK_FILE_H )
    {
        mask |= ( b >> 9 );
    }
    if ( b << 8 )
    {
        mask |= ( b << 8 );
    }
    if ( ( b << 1 ) & BITBOARD_MASK_FILE_A )
    {
        mask |= ( b << 1 );
    }
    if ( ( b << 9 ) & BITBOARD_MASK_FILE_A )
    {
        mask |= ( b << 9 );
    }
    if ( ( b << 7 ) & BITBOARD_MASK_FILE_H )
    {
        mask |= ( b << 7 );
    }

    return mask;
}

char*
string_bitboard
(   const bitboard_t bitboard
)
{
    char buf[ BITBOARD_STRING_MAX_LENGTH ];

    u64 offs = string_format ( buf
                             , "BITBOARD:  %llu\n"
                             , bitboard
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

            offs += string_format ( buf + offs
                                  , "%u"
                                  , bit ( bitboard , SQUAREINDX ( r , f ) )
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

    return string_allocate_from ( buf );
}
