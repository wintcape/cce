/**
 * @author Matthew Weissel (null@mattweissel.info)
 * @file attack.c
 * @brief Implementation of the attack header.
 * (see attack.h for additional details)
 */
#include "chess/attack.h"

#include "chess/bitboard.h"

/**
 * @brief For pregenerating attack tables. Given an attack mask, this function
 * generates a new attack mask which takes into account an occupancy parameter.
 * @param occupancy Used for calculating occupied square.
 * @param attack Attack bitboard mask.
 * @param attack_relevant_count Number of attack mask bits to consider.
 * @return A bitboard with the attack options masked by the occupancy.
 */
bitboard_t
attack_mask_with_occupancy
(   const int   occupancy
,   bitboard_t  attack
,   const u8    attack_relevant_count
)
{
    bitboard_t mask = 0;
    for ( u8 i = 0; i < attack_relevant_count; ++i )
    {
        SQUARE square = bitboard_lsb ( attack );
        if ( occupancy & ( 1 << i ) )
        {
            BITSET ( mask , square );
        }
        BITCLR ( attack , square );
    }
    return mask;
}

/**
 * @brief For pregenerating attack tables. Generates every attack option for a
 * single pawn.
 * @param square The pawn's position.
 * @return A bitboard with the pawn's attack options set.
 */
bitboard_t
attack_mask_pawn
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

/**
 * @brief For pregenerating attack tables. Generates every attack option for a
 * single knight.
 * @param square The knight's position.
 * @return A bitboard with the knight's attack options set.
 */
bitboard_t
attack_mask_knight
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

/**
 * @brief For pregenerating attack tables. Generates every attack option for a
 * single bishop.
 * @param square The bishop's position.
 * @return A bitboard with the bishop's attack options set.
 */
bitboard_t
attack_mask_bishop
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

/**
 * @brief Variation of attack_mask_bishop which accepts a block mask parameter.
 * @param square The bishop's position.
 * @param block A mask to optionally block the bishop in the specified
 * direction(s).
 * @return A bitboard with the bishop's attack options set.
 */
bitboard_t
attack_mask_bishop_with_block
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

/**
 * @brief For pregenerating attack tables. Generates every attack option for a
 * single rook.
 * @param square The rook's position.
 * @return A bitboard with the rook's attack options set.
 */
bitboard_t
attack_mask_rook
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

/**
 * @brief Variation of attack_mask_rook which accepts a block mask
 * parameter.
 * @param square The rook's position.
 * @param block A mask to optionally block the rook in the specified
 * direction(s).
 * @return A bitboard with the rook's attack options set.
 */
bitboard_t
attack_mask_rook_with_block
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

/**
 * @brief For pregenerating attack tables. Generates the attack options for a
 * single king.
 * @param square The king's position.
 * @return A bitboard with the king's attack options set.
 */
bitboard_t
attack_mask_king
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

bool
attacks_init
(   attacks_t* attacks
)
{
    if ( !attacks )
    {
        return false;
    }

    for ( u8 i = 0; i < 64; ++i )
    {
        bitboard_t attack;
        u16 occupancy_indx;
        u8 attack_relevant_count;

        // Leaper pieces.
        ( *attacks ).pawn[ WHITE ][ i ] = attack_mask_pawn ( WHITE , i );
        ( *attacks ).pawn[ BLACK ][ i ] = attack_mask_pawn ( BLACK , i );
        ( *attacks ).knight[ i ] = attack_mask_knight ( i );
        ( *attacks ).king[ i ] = attack_mask_king ( i );

        // Bishop.
        ( *attacks ).bishop_masks[ i ] = attack_mask_bishop ( i ); 
        attack = ( *attacks ).bishop_masks[ i ];
        attack_relevant_count = bitboard_count ( attack );
        occupancy_indx = 1 << attack_relevant_count;
        for ( u16 j = 0; j < occupancy_indx; ++j )
        {
            const bitboard_t occupancy = attack_mask_with_occupancy ( j
                                                                    , attack
                                                                    , attack_relevant_count
                                                                    );
            const u16 k = ( occupancy * bitboard_magic_bishops [ i ] ) >> ( 64 - bishop_attack_relevant_counts[ i ] );
            ( *attacks ).bishop[ i ][ k ] = attack_mask_bishop_with_block ( i
                                                                          , occupancy
                                                                          );
        }

        // Rook.
        ( *attacks ).rook_masks[ i ] = attack_mask_rook ( i ); 
        attack = ( *attacks ).rook_masks[ i ];
        attack_relevant_count = bitboard_count ( attack );
        occupancy_indx = 1 << attack_relevant_count;
        for ( u16 j = 0; j < occupancy_indx; ++j )
        {
            const bitboard_t occupancy = attack_mask_with_occupancy ( j
                                                                    , attack
                                                                    , attack_relevant_count
                                                                    );
            const u16 k = ( occupancy * bitboard_magic_rooks[ i ] ) >> ( 64 - rook_attack_relevant_counts [ i ] );
            ( *attacks ).rook[ i ][ k ] = attack_mask_rook_with_block ( i
                                                                      , occupancy
                                                                      );
        }
    }

    return true;
}
