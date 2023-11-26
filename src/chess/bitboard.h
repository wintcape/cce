/**
 * @file bitboard.h
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Datatypes and operations to represent a chess bitboard.
 */
#ifndef CHESS_BITBOARD_H
#define CHESS_BITBOARD_H

#include "common.h"

#include "chess/castle.h"
#include "chess/piece.h"
#include "chess/square.h"

// Type definition for a bitboard.
typedef u64 bitboard_t;

// Preprocessor bindings for typesafe bitboard literals.
#define BITBOARD_MASK_FILE_A    ( ( bitboard_t ) 18374403900871474942ULL )
#define BITBOARD_MASK_FILE_H    ( ( bitboard_t ) 9187201950435737471ULL )
#define BITBOARD_MASK_FILE_AB   ( ( bitboard_t ) 18229723555195321596ULL )
#define BITBOARD_MASK_FILE_HG   ( ( bitboard_t ) 4557430888798830399ULL )

/**
 * @brief Computes the number of set bits within a bitboard.
 * @param bitboard A bitboard.
 * @return The number of set bits within bitboard.
 */
INLINE
u8
bitboard_count
(   bitboard_t bitboard
)
{
    u8 n = 0;
    while ( bitboard )
    {
        n += 1;
        bitboard &= bitboard - 1;
    }
    return n;
}

/**
 * @brief Computes the index of the least significant digit of the first set bit
 * within a bitboard.
 * @param bitboard A bitboard.
 * @return The index of the least significant digit of the first set bit within
 * a bitboard, or -1 if bitboard is empty.
 */
INLINE
i8
bitboard_lsb_indx
(   bitboard_t bitboard
)
{
    return ( bitboard ) ? bitboard_count ( ( bitboard & -bitboard ) - 1 )
                        : -1
                        ;
}

/**
 * @brief Given an attack mask, this function generates a new attack mask which
 * takes into account an occupancy parameter.
 * @param occupancy Used for calculating occupied square.
 * @param attack Attack bitboard mask.
 * @param attack_relevant_count Number of attack mask bits to consider.
 * @return A bitboard with the attack options masked by the occupancy.
 */
bitboard_t
bitboard_mask_attack_with_occupancy
(   const int   occupancy
,   bitboard_t  attack
,   const u8    attack_relevant_count
);

/**
 * @brief Generates the attack options for a single pawn.
 * @param square The pawn's position.
 * @param side The pawn's side.
 * @return A bitboard with the pawn's attack options set.
 */
bitboard_t
bitboard_mask_pawn_attack
(   const SIDE      side
,   const SQUARE    square
);

/**
 * @brief Generates the attack options for a single knight.
 * @param square The knight's position.
 * @return A bitboard with the knight's attack options set.
 */
bitboard_t
bitboard_mask_knight_attack
(   const SQUARE square
);

/**
 * @brief Generates the attack options for a single bishop.
 * @param square The bishop's position.
 * @return A bitboard with the bishop's attack options set.
 */
bitboard_t
bitboard_mask_bishop_attack
(   const SQUARE square
);

/**
 * @brief Variation of bitboard_mask_bishop_attack which accepts a block mask
 * parameter.
 * @param square The bishop's position.
 * @param block A mask to optionally block the bishop in the specified
 * direction(s).
 * @return A bitboard with the bishop's attack options set.
 */
bitboard_t
bitboard_mask_bishop_attack_with_block
(   const SQUARE        square
,   const bitboard_t    block
);

/**
 * @brief Generates the attack options for a single rook.
 * @param square The rook's position.
 * @return A bitboard with the rook's attack options set.
 */
bitboard_t
bitboard_mask_rook_attack
(   const SQUARE square
);

/**
 * @brief Variation of bitboard_mask_rook_attack which accepts a block mask
 * parameter.
 * @param square The rook's position.
 * @param block A mask to optionally block the rook in the specified
 * direction(s).
 * @return A bitboard with the rook's attack options set.
 */
bitboard_t
bitboard_mask_rook_attack_with_block
(   const SQUARE        square
,   const bitboard_t    block 
);

/**
 * @brief Generates the attack options for a single king.
 * @param square The king's position.
 * @return A bitboard with the king's attack options set.
 */
bitboard_t
bitboard_mask_king_attack
(   const SQUARE square
);

/**
 * @brief Stringify utility.
 * @param bitboard A bitboard.
 * @return A string representation of bitboard, or 0 if chess subsystem is
 * uninitialized.
 */
char*
string_bitboard
(   const bitboard_t bitboard
);

#endif  // CHESS_BITBOARD_H
