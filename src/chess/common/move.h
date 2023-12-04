/**
 * @file bitboard.h
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Defines datatypes and operations to represent a singular move on a
 * chess board.
 */
#ifndef CHESS_COMMON_MOVE_H
#define CHESS_COMMON_MOVE_H

#include "common.h"

// Type definition for a move.
typedef u32 move_t;

// Defines the maximum number of moves which may be added to a single container.
#define MOVES_BUFFER_LENGTH 256

// Type definition for a container to hold pregenerated moves.
typedef struct
{
    move_t  moves[ MOVES_BUFFER_LENGTH ];
    u32     count;
}
moves_t;

/**
 * @brief Encodes a single move via bitpacking.
 * @param src The source square.
 * @param target The target square.
 * @param piece The piece on the source square.
 * @param promotion Promoted piece, if any.
 * @param capture Is move capture? Y/N
 * @param enpassant Is move en passant? Y/N
 * @param castle Is move castle? Y/N
 * @return A single move, encoded as a bitpacked 32-bit unsigned integer.
 */
INLINE
move_t
move_encode
(   const SQUARE    src
,   const SQUARE    target
,   const PIECE     piece
,   const PIECE     promotion
,   const bool      capture
,   const bool      double_push
,   const bool      enpassant
,   const bool      castle
)
{
    return src
         | ( target << 6 )
         | ( piece << 12 )
         | ( promotion << 16 )
         | ( capture << 20 )
         | ( double_push << 21 )
         | ( enpassant << 22 )
         | ( castle << 23 )
         ;
}

/**
 * @brief Retrieves source square from a bitpacked move.
 * @param move A move.
 * @return The source square of move.
 */
INLINE
SQUARE
move_decode_source_square
(   const move_t move
)
{
    return move & 0x3F;
}

/**
 * @brief Retrieves target square from a bitpacked move.
 * @param move A move.
 * @return The target square of move.
 */
INLINE
SQUARE
move_decode_target_square
(   const move_t move
)
{
    return ( move & 0xFC0 ) >> 6;
}

/**
 * @brief Retrieves piece from a bitpacked move.
 * @param move A move.
 * @return The piece of move.
 */
INLINE
PIECE
move_decode_piece
(   const move_t move
)
{
    return ( move & 0xF000 ) >> 12;
}

/**
 * @brief Retrieves piece promotion from a bitpacked move.
 * @param move A move.
 * @return The piece promotion of move.
 */
INLINE
PIECE
move_decode_promotion
(   const move_t move
)
{
    return ( move & 0xF0000 ) >> 16;
}

/**
 * @brief Retrieves capture status from a bitpacked move.
 * @param move A move.
 * @return true if capture move, false otherwise.
 */
INLINE
bool
move_decode_capture
(   const move_t move
)
{
    return move & 0x100000;
}

/**
 * @brief Retrieves double push status from a bitpacked move.
 * @param move A move.
 * @return true if double push move, false otherwise.
 */
INLINE
bool
move_decode_double_push
(   const move_t move
)
{
    return move & 0x200000;
}

/**
 * @brief Retrieves en passant status from a bitpacked move.
 * @param move A move.
 * @return true if en passant move, false otherwise.
 */
INLINE
bool
move_decode_enpassant
(   const move_t move
)
{
    return move & 0x400000;
}

/**
 * @brief Retrieves castling status from a bitpacked move.
 * @param move A move.
 * @return true if castling move, false otherwise.
 */
INLINE
bool
move_decode_castle
(   const move_t move
)
{
    return move & 0x800000;
}

#endif  // CHESS_COMMON_MOVE_H
