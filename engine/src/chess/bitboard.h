/**
 * @file bitboard.h
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Datatypes and operations to represent a chess bitboard.
 */
#ifndef CHESS_BITBOARD_H
#define CHESS_BITBOARD_H

#include "common.h"

#include "chess/piece.h"
#include "chess/square.h"

// Defaults.
#define BITBOARD_STRING_MAX_LENGTH 256

// Type definition for a bitboard.
typedef u64 bitboard_t;

// Preprocessor bindings for typesafe bitboard literals.
#define BITBOARD_1              ( ( bitboard_t ) 1 )
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
 * takes into account an occupied square.
 * @param occupied The occupied square.
 * @param mask Attack bitboard mask.
 */
bitboard_t
bitboard_mask_attack_with_occupancy
(   const bitboard_t    occupied
,   bitboard_t          mask
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
 * @param block A mask to optionally block the bishop in the specified
 * direction(s).
 * @return A bitboard with the bishop's attack options set.
 */
bitboard_t
bitboard_mask_bishop_attack
(   const SQUARE        square
,   const bitboard_t    block
);

/**
 * @brief Generates the attack options for a single rook.
 * @param square The rook's position.
 * @param block A mask to optionally block the rook in the specified
 * direction(s).
 * @return A bitboard with the rook's attack options set.
 */
bitboard_t
bitboard_mask_rook_attack
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
 * @brief Stringify utility. Writes a string representation of a bitboard to
 * the provided buffer.
 * @param bitboard A bitboard.
 * @param string A buffer to write the string to.
 * @return string.
 */
char*
string_bitboard
(   const bitboard_t    bitboard
,   char*               string
);

#endif  // CHESS_BITBOARD_H
