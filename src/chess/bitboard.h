/**
 * @file bitboard.h
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Bitboard datatypes and operations.
 */
#ifndef CHESS_BITBOARD_H
#define CHESS_BITBOARD_H

#include "chess/common.h"

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
bitboard_lsb
(   bitboard_t bitboard
)
{
    return ( bitboard ) ? bitboard_count ( ( bitboard & -bitboard ) - 1 )
                        : -1
                        ;
}

/**
 * @brief Computes a requested pawn attack using a pregenerated attack table.
 * @param attacks The pregenerated attack tables.
 * @param square The pawn position.
 * @param side The pawn side.
 * @return The requested pawn attack bitboard.
 */
INLINE
bitboard_t
bitboard_pawn_attack
(   const attacks_t*    attacks
,   const SQUARE        square
,   const SIDE          side
)
{
    return ( *attacks ).pawn[ side ][ square ];
}

/**
 * @brief Computes a requested knight attack using a pregenerated attack table.
 * @param attacks The pregenerated attack tables.
 * @param square The knight position.
 * @return The requested knight attack bitboard.
 */
INLINE
bitboard_t
bitboard_knight_attack
(   const attacks_t*    attacks
,   const SQUARE        square
)
{
    return ( *attacks ).knight[ square ];
}

/**
 * @brief Computes a requested bishop attack using a pregenerated attack table.
 * @param attacks The pregenerated attack tables.
 * @param square The bishop position.
 * @param occupancy An occupancy mask.
 * @return The requested bishop attack bitboard.
 */
INLINE
bitboard_t
bitboard_bishop_attack
(   const attacks_t*    attacks
,   const SQUARE        square
,   bitboard_t          occupancy
)
{
    occupancy &= ( *attacks ).bishop_masks[ square ];
    occupancy *= bitboard_magic_bishops[ square ];
    occupancy >>= 64 - bishop_attack_relevant_counts[ square ];
    return ( *attacks ).bishop[ square ][ occupancy ];
}

/**
 * @brief Computes a requested rook attack using a pregenerated attack table.
 * @param attacks The pregenerated attack tables.
 * @param square The rook position.
 * @param occupancy An occupancy mask.
 * @return The requested rook attack bitboard.
 */
INLINE
bitboard_t
bitboard_rook_attack
(   const attacks_t*    attacks
,   const SQUARE        square
,   bitboard_t          occupancy
)
{
    occupancy &= ( *attacks ).rook_masks[ square ];
    occupancy *= bitboard_magic_rooks[ square ];
    occupancy >>= 64 - rook_attack_relevant_counts[ square ];
    return ( *attacks ).rook[ square ][ occupancy ];
}

/**
 * @brief Computes a requested queen attack using a pregenerated attack table.
 * @param attacks The pregenerated attack tables.
 * @param square The queen position.
 * @param occupancy An occupancy mask.
 * @return The requested rook attack bitboard.
 */
INLINE
bitboard_t
bitboard_queen_attack
(   const attacks_t*    attacks
,   const SQUARE        square
,   bitboard_t          occupancy
)
{
    bitboard_t bishop_occupancy = occupancy;
    bitboard_t rook_occupancy = occupancy;

    bishop_occupancy &= ( *attacks ).bishop_masks[ square ];
    bishop_occupancy *= bitboard_magic_bishops[ square ];
    bishop_occupancy >>= 64 - bishop_attack_relevant_counts[ square ];
    
    rook_occupancy &= ( *attacks ).rook_masks[ square ];
    rook_occupancy *= bitboard_magic_rooks[ square ];
    rook_occupancy >>= 64 - rook_attack_relevant_counts[ square ];

    return ( *attacks ).bishop[ square ][ bishop_occupancy ]
         | ( *attacks ).rook[ square ][ rook_occupancy ]
         ;
}

/**
 * @brief Computes a requested king attack using a pregenerated attack table.
 * @param attacks The pregenerated attack tables.
 * @param square The king position.
 * @param occupancy An occupancy mask.
 * @return The requested knight attack bitboard.
 */
INLINE
bitboard_t
bitboard_king_attack
(   const attacks_t*    attacks
,   const SQUARE        square
)
{
    return ( *attacks ).king[ square ];
}

/**
 * @brief Generates a bitboard whose bits are set if the corresponding square on
 * a chess board may be attacked by a given side. Requires pregenerated attack
 * tables.
 * @param board The chess board state.
 * @param attacks Pregenerated attack tables.
 * @param side The current side.
 * @return A bitboard with all valid attacks set.
 */
bitboard_t
bitboard_attackable
(   const board_t*      board
,   const attacks_t*    attacks
,   const SIDE          side
);

/**
 * @brief Stringify utility. Uses memory allocator; call string_free to free.
 * @param bitboard A bitboard.
 * @return A string representation of bitboard.
 */
char*
string_bitboard
(   const bitboard_t bitboard
);

#endif  // CHESS_BITBOARD_H
