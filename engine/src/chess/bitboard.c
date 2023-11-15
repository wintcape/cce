/**
 * @author Matthew Weissel (null@mattweissel.info)
 * @f bitboard.c
 * @brief Implementation of the bitboard header.
 * (see bitboard.h for additional details)
 */
#include "chess/bitboard.h"

#include "core/string.h"

// Defaults.
#define BITBOARD_STRING_MAX_LENGTH 256

// Defines a string representation of each board square.
static const char* square_coordinate_tags[] = { "A8" , "B8" , "C8" , "D8" , "E8" , "F8" , "G8" , "H8"
                                              , "A7" , "B7" , "C7" , "D7" , "E7" , "F7" , "G7" , "H7"
                                              , "A6" , "B6" , "C6" , "D6" , "E6" , "F6" , "G6" , "H6"
                                              , "A5" , "B5" , "C5" , "D5" , "E5" , "F5" , "G5" , "H5"
                                              , "A4" , "B4" , "C4" , "D4" , "E4" , "F4" , "G4" , "H4"
                                              , "A3" , "B3" , "C3" , "D3" , "E3" , "F3" , "G3" , "H3"
                                              , "A2" , "B2" , "C2" , "D2" , "E2" , "F2" , "G2" , "H2"
                                              , "A1" , "B1" , "C1" , "D1" , "E1" , "F1" , "G1" , "H1"
                                              };

// Defines relevant occupancy bit count for each board square.
static const u8 bishop_relevant_bits[ 64 ] = { 6 , 5 , 5 , 5 , 5 , 5 , 5 , 6
                                             , 5 , 5 , 5 , 5 , 5 , 5 , 5 , 5
                                             , 5 , 5 , 7 , 7 , 7 , 7 , 5 , 5
                                             , 5 , 5 , 7 , 9 , 9 , 7 , 5 , 5
                                             , 5 , 5 , 7 , 9 , 9 , 7 , 5 , 5
                                             , 5 , 5 , 7 , 7 , 7 , 7 , 5 , 5
                                             , 5 , 5 , 5 , 5 , 5 , 5 , 5 , 5
                                             , 6 , 5 , 5 , 5 , 5 , 5 , 5 , 6
                                             };
static const u8 rook_relevant_bits[ 64 ] = { 12 , 11 , 11 , 11 , 11 , 11 , 11 , 12
                                           , 11 , 10 , 10 , 10 , 10 , 10 , 10 , 11
                                           , 11 , 10 , 10 , 10 , 10 , 10 , 10 , 11
                                           , 11 , 10 , 10 , 10 , 10 , 10 , 10 , 11
                                           , 11 , 10 , 10 , 10 , 10 , 10 , 10 , 11
                                           , 11 , 10 , 10 , 10 , 10 , 10 , 10 , 11
                                           , 11 , 10 , 10 , 10 , 10 , 10 , 10 , 11
                                           , 12 , 11 , 11 , 11 , 11 , 11 , 11 , 12
                                           };

bitboard_t
bitboard_mask_attack_with_occupancy
(   const bitboard_t    occupied
,   bitboard_t          mask
)
{
    bitboard_t occupancy = 0; 
    const u8 n = bitboard_count ( mask );
    for ( u8 i = 0; i < n; ++i )
    {
        SQUARE square = bitboard_lsb_indx ( mask );
        if ( occupied & bitset ( 0 , i ) )
        {
            occupancy = bitset ( occupancy , square );
        }
        mask = bitclr ( mask , square );
    }
    return occupancy;
}

bitboard_t
bitboard_mask_pawn_attack
(   const SIDE      side
,   const SQUARE    square
)
{
    bitboard_t attacks = 0;
    
    // Set the piece.
    bitboard_t bitboard = bitset ( 0 , square );

    // Set attack options.
    if ( side == WHITE )
    {
        if ( ( bitboard >> 7 ) & BITBOARD_MASK_FILE_A )
        {
            attacks |= ( bitboard >> 7 );
        }
        if ( ( bitboard >> 9 ) & BITBOARD_MASK_FILE_H )
        {
            attacks |= ( bitboard >> 9 );
        } 
    }
    else
    {
        if ( ( bitboard << 9 ) & BITBOARD_MASK_FILE_A )
        {
            attacks |= ( bitboard << 9 );
        }
        if ( ( bitboard << 7 ) & BITBOARD_MASK_FILE_H )
        {
            attacks |= ( bitboard << 7 );
        }
    }

    return attacks;
}

bitboard_t
bitboard_mask_knight_attack
(   const SQUARE square
)
{
    bitboard_t attacks = 0; 
    
    // Set the piece.
    bitboard_t bitboard = bitset ( 0 , square );

    // Set attack options.
    if ( ( bitboard >> 15 ) & BITBOARD_MASK_FILE_A )
    {
        attacks |= ( bitboard >> 15 );
    }
    if ( ( bitboard >> 17 ) & BITBOARD_MASK_FILE_H )
    {
        attacks |= ( bitboard >> 17 );
    }
    if ( ( bitboard >> 6 ) & BITBOARD_MASK_FILE_AB )
    {
        attacks |= ( bitboard >> 6 );
    }
    if ( ( bitboard >> 10 ) & BITBOARD_MASK_FILE_HG )
    {
        attacks |= ( bitboard >> 10 );
    }
    if ( ( bitboard << 17 ) & BITBOARD_MASK_FILE_A )
    {
        attacks |= ( bitboard << 17 );
    }
    if ( ( bitboard << 15 ) & BITBOARD_MASK_FILE_H )
    {
        attacks |= ( bitboard << 15 );
    }
    if ( ( bitboard << 10 ) & BITBOARD_MASK_FILE_AB )
    {
        attacks |= ( bitboard << 10 );
    }
    if ( ( bitboard << 6 ) & BITBOARD_MASK_FILE_HG )
    {
        attacks |= ( bitboard << 6 );
    }

    return attacks;
}

bitboard_t
bitboard_mask_bishop_attack
(   const SQUARE square
)
{
    bitboard_t attacks = 0; 

    // Target rank and file.
    const i8 r_target = square / 8;
    const i8 f_target = square % 8;
    
    // Set attack options.
    i8 r;
    i8 f;
    for ( r = r_target + 1 , f = f_target + 1; r <= 6 && f <= 6; ++r , ++f )
    {
        attacks = bitset ( attacks , SQUAREINDX ( r , f ) );
    }
    for ( r = r_target - 1 , f = f_target + 1; r >= 1 && f <= 6; --r , ++f )
    {
        attacks = bitset ( attacks , SQUAREINDX ( r , f ) );
    }
    for ( r = r_target + 1 , f = f_target - 1; r <= 6 && f >= 1; ++r , --f )
    {
        attacks = bitset ( attacks , SQUAREINDX ( r , f ) );
    }
    for ( r = r_target - 1 , f = f_target - 1; r >= 1 && f >= 1; --r , --f )
    {
        attacks = bitset ( attacks , SQUAREINDX ( r , f ) );
    }

    return attacks;
}

bitboard_t
bitboard_mask_bishop_attack_with_block_mask
(   const SQUARE        square
,   const bitboard_t    mask
)
{
    bitboard_t attacks = 0; 

    // Target rank and file.
    const i8 r_target = square / 8;
    const i8 f_target = square % 8;
    
    // Set attack options.
    i8 r;
    i8 f;
    for ( r = r_target + 1 , f = f_target + 1; r <= 7 && f <= 7; ++r , ++f )
    {
        attacks = bitset ( attacks , SQUAREINDX ( r , f ) );
        if ( bitset ( 0 , SQUAREINDX ( r , f ) ) & mask )
        {
            break;
        }
    }
    for ( r = r_target - 1 , f = f_target + 1; r >= 0 && f <= 7; --r , ++f )
    {
        attacks = bitset ( attacks , r * 8 + f );
        if ( bitset ( 0 , SQUAREINDX ( r , f ) ) & mask )
        {
            break;
        }
    }
    for ( r = r_target + 1 , f = f_target - 1; r <= 7 && f >= 0; ++r , --f )
    {
        attacks = bitset ( attacks , SQUAREINDX ( r , f ) );
        if ( bitset ( 0 , SQUAREINDX ( r , f ) ) & mask )
        {
            break;
        }
    }
    for ( r = r_target - 1 , f = f_target - 1; r >= 0 && f >= 0; --r , --f )
    {
        attacks = bitset ( attacks , SQUAREINDX ( r , f ) );
        if ( bitset ( 0 , SQUAREINDX ( r , f ) ) & mask )
        {
            break;
        }
    }

    return attacks;
}

bitboard_t
bitboard_mask_rook_attack
(   const SQUARE square
)
{
    bitboard_t attacks = 0;

    // Target rank and file.
    const i8 r_target = square / 8;
    const i8 f_target = square % 8;
    
    // Set attack options.
    i8 r;
    i8 f;
    for ( r = r_target + 1; r <= 6; ++r )
    {
        attacks = bitset ( attacks , SQUAREINDX ( r , f_target ) );
    }
    for ( r = r_target - 1; r >= 1; --r )
    {
        attacks = bitset ( attacks , SQUAREINDX ( r , f_target ) );
    }
    for ( f = f_target + 1; f <= 6; ++f )
    {
        attacks = bitset ( attacks , SQUAREINDX ( r_target , f ) );
    }
    for ( f = f_target - 1; f >= 1; --f )
    {
        attacks = bitset ( attacks , SQUAREINDX ( r_target , f ) );
    }

    return attacks;
}

bitboard_t
bitboard_mask_rook_attack_with_block_mask
(   const SQUARE        square
,   const bitboard_t    mask
)
{
    bitboard_t attacks = 0; 

    // Target rank and file.
    const i8 r_target = square / 8;
    const i8 f_target = square % 8;
    
    // Set attack options.
    i8 r;
    i8 f;
    for ( r = r_target + 1; r <= 7; ++r )
    {
        attacks = bitset ( attacks , SQUAREINDX ( r , f_target ) );
        if ( bitset ( 0 , SQUAREINDX ( r , f_target ) ) & mask )
        {
            break;
        }
    }
    for ( r = r_target - 1; r >= 0; --r )
    {
        attacks = bitset ( attacks , SQUAREINDX ( r , f_target ) );
        if ( bitset ( 0 , SQUAREINDX ( r , f_target ) ) & mask )
        {
            break;
        }
    }
    for ( f = f_target + 1; f <= 7; ++f )
    {
        attacks = bitset ( attacks , SQUAREINDX ( r_target , f ) );
        if ( bitset ( 0 , SQUAREINDX ( r_target , f ) ) & mask )
        {
            break;
        }
    }
    for ( f = f_target - 1; f >= 0; --f )
    {
        attacks = bitset ( attacks , SQUAREINDX ( r_target , f ) );
        if ( bitset ( 0 , SQUAREINDX ( r_target , f ) ) & mask )
        {
            break;
        }
    }

    return attacks;
}

bitboard_t
bitboard_mask_king_attack
(   const SQUARE square
)
{
    bitboard_t attacks = 0; 
    
    // Set the piece.
    bitboard_t bitboard = bitset ( 0 , square );

    // Set attack options.
    if ( bitboard >> 8 )
    {
        attacks |= ( bitboard >> 8 );
    }
    if ( ( bitboard >> 7 ) & BITBOARD_MASK_FILE_A )
    {
        attacks |= ( bitboard >> 7 );
    }
    if ( ( bitboard >> 1 ) & BITBOARD_MASK_FILE_H )
    {
        attacks |= ( bitboard >> 1 );
    }
    if ( ( bitboard >> 9 ) & BITBOARD_MASK_FILE_H )
    {
        attacks |= ( bitboard >> 9 );
    }
    if ( bitboard << 8 )
    {
        attacks |= ( bitboard << 8 );
    }
    if ( ( bitboard << 1 ) & BITBOARD_MASK_FILE_A )
    {
        attacks |= ( bitboard << 1 );
    }
    if ( ( bitboard << 9 ) & BITBOARD_MASK_FILE_A )
    {
        attacks |= ( bitboard << 9 );
    }
    if ( ( bitboard << 7 ) & BITBOARD_MASK_FILE_H )
    {
        attacks |= ( bitboard << 7 );
    }

    return attacks;
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

const char*
string_bitboard_square
(   u8 i
)
{
    return square_coordinate_tags[ i ];
}
