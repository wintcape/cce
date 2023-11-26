/**
 * @file magic.h
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Magic number bitboards.
 */
#ifndef CHESS_MAGIC_H
#define CHESS_MAGIC_H

#include "common.h"

#include "chess/bitboard.h"

// Defines relevant occupancy count for each board square by player.
static const u8 bishop_relevant_counts[ 64 ] = { 6 , 5 , 5 , 5 , 5 , 5 , 5 , 6
                                               , 5 , 5 , 5 , 5 , 5 , 5 , 5 , 5
                                               , 5 , 5 , 7 , 7 , 7 , 7 , 5 , 5
                                               , 5 , 5 , 7 , 9 , 9 , 7 , 5 , 5
                                               , 5 , 5 , 7 , 9 , 9 , 7 , 5 , 5
                                               , 5 , 5 , 7 , 7 , 7 , 7 , 5 , 5
                                               , 5 , 5 , 5 , 5 , 5 , 5 , 5 , 5
                                               , 6 , 5 , 5 , 5 , 5 , 5 , 5 , 6
                                               };
static const u8 rook_relevant_counts[ 64 ] = { 12 , 11 , 11 , 11 , 11 , 11 , 11 , 12
                                             , 11 , 10 , 10 , 10 , 10 , 10 , 10 , 11
                                             , 11 , 10 , 10 , 10 , 10 , 10 , 10 , 11
                                             , 11 , 10 , 10 , 10 , 10 , 10 , 10 , 11
                                             , 11 , 10 , 10 , 10 , 10 , 10 , 10 , 11
                                             , 11 , 10 , 10 , 10 , 10 , 10 , 10 , 11
                                             , 11 , 10 , 10 , 10 , 10 , 10 , 10 , 11
                                             , 12 , 11 , 11 , 11 , 11 , 11 , 11 , 12
                                             };

// Defines magic bitboards for slider pieces.
static const bitboard_t bitboard_magic_bishops[ 64 ] = { 0x89A1121896040240ULL
                                                       , 0x2004844802002010ULL
                                                       , 0x2068080051921000ULL
                                                       , 0x62880A0220200808ULL
                                                       , 0x4042004000000ULL
                                                       , 0x100822020200011ULL
                                                       , 0xC00444222012000AULL
                                                       , 0x28808801216001ULL
                                                       , 0x400492088408100ULL
                                                       , 0x201C401040C0084ULL
                                                       , 0x840800910A0010ULL
                                                       , 0x82080240060ULL
                                                       , 0x2000840504006000ULL
                                                       , 0x30010C4108405004ULL
                                                       , 0x1008005410080802ULL
                                                       , 0x8144042209100900ULL
                                                       , 0x208081020014400ULL
                                                       , 0x4800201208CA00ULL
                                                       , 0xF18140408012008ULL
                                                       , 0x1004002802102001ULL
                                                       , 0x841000820080811ULL
                                                       , 0x40200200A42008ULL
                                                       , 0x800054042000ULL
                                                       , 0x88010400410C9000ULL
                                                       , 0x520040470104290ULL
                                                       , 0x1004040051500081ULL
                                                       , 0x2002081833080021ULL
                                                       , 0x400C00C010142ULL
                                                       , 0x941408200C002000ULL
                                                       , 0x658810000806011ULL
                                                       , 0x188071040440A00ULL
                                                       , 0x4800404002011C00ULL
                                                       , 0x104442040404200ULL
                                                       , 0x511080202091021ULL
                                                       , 0x4022401120400ULL
                                                       , 0x80C0040400080120ULL
                                                       , 0x8040010040820802ULL
                                                       , 0x480810700020090ULL
                                                       , 0x102008E00040242ULL
                                                       , 0x809005202050100ULL
                                                       , 0x8002024220104080ULL
                                                       , 0x431008804142000ULL
                                                       , 0x19001802081400ULL
                                                       , 0x200014208040080ULL
                                                       , 0x3308082008200100ULL
                                                       , 0x41010500040C020ULL
                                                       , 0x4012020C04210308ULL
                                                       , 0x208220A202004080ULL
                                                       , 0x111040120082000ULL
                                                       , 0x6803040141280A00ULL
                                                       , 0x2101004202410000ULL
                                                       , 0x8200000041108022ULL
                                                       , 0x21082088000ULL
                                                       , 0x2410204010040ULL
                                                       , 0x40100400809000ULL
                                                       , 0x822088220820214ULL
                                                       , 0x40808090012004ULL
                                                       , 0x910224040218C9ULL
                                                       , 0x402814422015008ULL
                                                       , 0x90014004842410ULL
                                                       , 0x1000042304105ULL
                                                       , 0x10008830412A00ULL
                                                       , 0x2520081090008908ULL
                                                       , 0x40102000A0A60140ULL
                                                       };
static const bitboard_t bitboard_magic_rooks[ 64 ] = { 0xA8002C000108020ULL
                                                     , 0x6C00049B0002001ULL
                                                     , 0x100200010090040ULL
                                                     , 0x2480041000800801ULL
                                                     , 0x280028004000800ULL
                                                     , 0x900410008040022ULL
                                                     , 0x280020001001080ULL
                                                     , 0x2880002041000080ULL
                                                     , 0xA000800080400034ULL
                                                     , 0x4808020004000ULL
                                                     , 0x2290802004801000ULL
                                                     , 0x411000D00100020ULL
                                                     , 0x402800800040080ULL
                                                     , 0xB000401004208ULL
                                                     , 0x2409000100040200ULL
                                                     , 0x1002100004082ULL
                                                     , 0x22878001E24000ULL
                                                     , 0x1090810021004010ULL
                                                     , 0x801030040200012ULL
                                                     , 0x500808008001000ULL
                                                     , 0xA08018014000880ULL
                                                     , 0x8000808004000200ULL
                                                     , 0x201008080010200ULL
                                                     , 0x801020000441091ULL
                                                     , 0x800080204005ULL
                                                     , 0x1040200040100048ULL
                                                     , 0x120200402082ULL
                                                     , 0xD14880480100080ULL
                                                     , 0x12040280080080ULL
                                                     , 0x100040080020080ULL
                                                     , 0x9020010080800200ULL
                                                     , 0x813241200148449ULL
                                                     , 0x491604001800080ULL
                                                     , 0x100401000402001ULL
                                                     , 0x4820010021001040ULL
                                                     , 0x400402202000812ULL
                                                     , 0x209009005000802ULL
                                                     , 0x810800601800400ULL
                                                     , 0x4301083214000150ULL
                                                     , 0x204026458E001401ULL
                                                     , 0x40204000808000ULL
                                                     , 0x8001008040010020ULL
                                                     , 0x8410820820420010ULL
                                                     , 0x1003001000090020ULL
                                                     , 0x804040008008080ULL
                                                     , 0x12000810020004ULL
                                                     , 0x1000100200040208ULL
                                                     , 0x430000A044020001ULL
                                                     , 0x280009023410300ULL
                                                     , 0xE0100040002240ULL
                                                     , 0x200100401700ULL
                                                     , 0x2244100408008080ULL
                                                     , 0x8000400801980ULL
                                                     , 0x2000810040200ULL
                                                     , 0x8010100228810400ULL
                                                     , 0x2000009044210200ULL
                                                     , 0x4080008040102101ULL
                                                     , 0x40002080411D01ULL
                                                     , 0x2005524060000901ULL
                                                     , 0x502001008400422ULL
                                                     , 0x489A000810200402ULL
                                                     , 0x1004400080A13ULL
                                                     , 0x4000011008020084ULL
                                                     , 0x26002114058042ULL
                                                     };

/**
 * @brief .
 * @param square .
 * @return .
 */
INLINE
bitboard_t
bitboard_magic_bishop
(   const SQUARE square
)
{
    return bitboard_magic_bishops[ square ];
}

/**
 * @brief .
 * @param square .
 * @return .
 */
INLINE
bitboard_t
bitboard_magic_rook
(   const SQUARE square
)
{
    return bitboard_magic_rooks[ square ];
}

/**
 * @brief . 
 * @param square .
 * @return .
 */
INLINE
u8
bishop_relevant_count
(   const SQUARE square
)
{
    return bishop_relevant_counts[ square ];
}

/**
 * @brief . 
 * @param square .
 * @return .
 */
INLINE
u8
rook_relevant_count
(   const SQUARE square
)
{
    return rook_relevant_counts[ square ];
}

#endif  // CHESS_MAGIC_H
