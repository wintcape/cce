/**
 * @file attack.h
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Defines a container to hold pregenerated attack tables.
 */
#ifndef CHESS_COMMON_ATTACK_H
#define CHESS_COMMON_ATTACK_H

#include "chess/common/bitboard.h"

// Type definition for a container to hold pregenerated attack tables.
typedef struct
{
    bitboard_t pawn[ 2 ][ 64 ];
    bitboard_t knight[ 64 ];
    bitboard_t bishop[ 64 ][ 512 ];
    bitboard_t rook[ 64 ][ 4096 ];
    bitboard_t king[ 64 ];

    bitboard_t bishop_masks[ 64 ];
    bitboard_t rook_masks[ 64 ];
}
attacks_t;

#endif  // CHESS_COMMON_ATTACK_H
