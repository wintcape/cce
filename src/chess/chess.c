/**
 * @author Matthew Weissel (null@mattweissel.info)
 * @file chess.c
 * @brief Implementation of the chess header.
 * (see chess.h for additional details)
 */
#include "chess/chess.h"

#include "core/logger.h"
#include "core/string.h"

// Type definition for chess engine subsystem state.
typedef struct
{
    // Pregenerated attack tables for leaper pieces.
    bitboard_t  pawn_attacks[ 2 ][ 64 ];
    bitboard_t  knight_attacks[ 64 ];
    bitboard_t  king_attacks[ 64 ];

    // Pregenerated attack tables for slider pieces.
    bitboard_t  bishop_masks[ 64 ];
    bitboard_t  bishop_attacks[ 64 ][ 512 ];
    bitboard_t  rook_masks[ 64 ];
    bitboard_t  rook_attacks[ 64 ][ 4096 ];

    // Current board state.
    board_t     board;

    // Ply count.
    u16         ply;
}
state_t;

// Global subsystem state.
static state_t* state;

/**
 * @brief .
 * @param square .
 * @param occupancy .
 * @return .
 */
INLINE
bitboard_t
bitboard_bishop_attack
(   const SQUARE    square
,   bitboard_t      occupancy
)
{
    occupancy &= ( *state ).bishop_masks[ square ];
    occupancy *= bitboard_magic_bishop ( square );
    occupancy >>= 64 - bishop_relevant_count ( square );
    return ( *state ).bishop_attacks[ square ][ occupancy ];
}

/**
 * @brief .
 * @param square .
 * @param occupancy .
 * @return .
 */
INLINE
bitboard_t
bitboard_rook_attack
(   const SQUARE    square
,   bitboard_t      occupancy
)
{
    occupancy &= ( *state ).rook_masks[ square ];
    occupancy *= bitboard_magic_rook ( square );
    occupancy >>= 64 - rook_relevant_count ( square );
    return ( *state ).rook_attacks[ square ][ occupancy ];
}

/**
 * @brief .
 * @param square .
 * @param occupancy .
 * @return .
 */
INLINE
bitboard_t
bitboard_queen_attack
(   const SQUARE    square
,   bitboard_t      occupancy
)
{
    bitboard_t bishop_occupancy = occupancy;
    bitboard_t rook_occupancy = occupancy;

    bishop_occupancy &= ( *state ).bishop_masks[ square ];
    bishop_occupancy *= bitboard_magic_bishop ( square );
    bishop_occupancy >>= 64 - bishop_relevant_count ( square );
    
    rook_occupancy &= ( *state ).rook_masks[ square ];
    rook_occupancy *= bitboard_magic_rook ( square );
    rook_occupancy >>= 64 - rook_relevant_count ( square );

    return ( *state ).bishop_attacks[ square ][ bishop_occupancy ]
         | ( *state ).rook_attacks[ square ][ rook_occupancy ]
         ;

}
/**
 * @brief .
 * @param square .
 * @param  side.
 * @return true if square attacked, false otherwise.
 */
INLINE
bool
chess_square_attacked
(   const SQUARE        square
,   const SIDE          side
)
{
    return (   ( side == WHITE )
            && (  ( *state ).pawn_attacks[ BLACK ][ square ]
                & ( *state ).board.pieces[ P ]
           ))
        || (   ( side == BLACK )
            && (  ( *state ).pawn_attacks[ WHITE ][ square ]
                & ( *state ).board.pieces[ P ]
           ))
        || (  ( *state ).knight_attacks[ square ]
            & ( ( side == WHITE ) ? ( *state ).board.pieces[ N ]
                                  : ( *state ).board.pieces[ n ]
           ))
        || (  bitboard_bishop_attack ( square
                                     , ( *state ).board.occupancies[ 2 ]
                                     )
            & ( ( side == WHITE ) ? ( *state ).board.pieces[ B ]
                                  : ( *state ).board.pieces[ b ]
           ))
        || (  bitboard_rook_attack ( square
                                   , ( *state ).board.occupancies[ 2 ]
                                   )
            & ( ( side == WHITE ) ? ( *state ).board.pieces[ R ]
                                  : ( *state ).board.pieces[ r ]
           ))
        || (  bitboard_queen_attack ( square
                                    , ( *state ).board.occupancies[ 2 ]
                                    )
            & ( ( side == WHITE ) ? ( *state ).board.pieces[ Q ]
                                  : ( *state ).board.pieces[ q ]
           ))
        || (  ( *state ).king_attacks[ square ]
            & ( ( side == WHITE ) ? ( *state ).board.pieces[ K ]
                                  : ( *state ).board.pieces[ k ]
           ))
        ;
}

bool
chess_startup
(   u64*    memory_requirement
,   void*   state_
)
{
    *memory_requirement = sizeof ( state_t );

    if ( !state_ )
    {
        return true;
    }

    state = state_;

    // Pregenerate attack tables.
    for ( u8 i = 0; i < 64; ++i )
    {
        bitboard_t attack;
        u16 occupancy_indx;
        u8 relevant_count;

        // Leaper pieces.
        ( *state ).pawn_attacks[ WHITE ][ i ] = bitboard_mask_pawn_attack ( WHITE , i );
        ( *state ).pawn_attacks[ BLACK ][ i ] = bitboard_mask_pawn_attack ( BLACK , i );
        ( *state ).knight_attacks[ i ] = bitboard_mask_knight_attack ( i );
        ( *state ).king_attacks[ i ] = bitboard_mask_king_attack ( i );

        // Bishop.
        ( *state ).bishop_masks[ i ] = bitboard_mask_bishop_attack ( i ); 
        attack = ( *state ).bishop_masks[ i ];
        relevant_count = bitboard_count ( attack );
        occupancy_indx = 1 << relevant_count;
        for ( u16 j = 0; j < occupancy_indx; ++j )
        {
            const bitboard_t occupancy = bitboard_mask_attack_with_occupancy ( j
                                                                             , attack
                                                                             , relevant_count
                                                                             );
            const int k = ( occupancy * bitboard_magic_bishop ( i ) ) >> ( 64 - bishop_relevant_count ( i ) );
            ( *state ).bishop_attacks[ i ][ k ] = bitboard_mask_bishop_attack_with_block ( i
                                                                                         , occupancy
                                                                                         );
        }

        // Rook.
        ( *state ).rook_masks[ i ] = bitboard_mask_rook_attack ( i ); 
        attack = ( *state ).rook_masks[ i ];
        relevant_count = bitboard_count ( attack );
        occupancy_indx = 1 << relevant_count;
        for ( u16 j = 0; j < occupancy_indx; ++j )
        {
            const bitboard_t occupancy = bitboard_mask_attack_with_occupancy ( j
                                                                             , attack
                                                                             , relevant_count
                                                                             );
            const u16 k = ( occupancy * bitboard_magic_rook ( i ) ) >> ( 64 - rook_relevant_count ( i ) );
            ( *state ).rook_attacks[ i ][ k ] = bitboard_mask_rook_attack_with_block ( i
                                                                                     , occupancy
                                                                                     );
        }
    }

    // Initialize board.
    fen_parse ( FEN_START , &( *state ).board );

    // Initialize game state.
    ( *state ).ply = 0;

    return true;
}

void
chess_shutdown
(   void* state_
)
{
    if ( !state )
    {
        return;
    }

    state = 0;
}

bool
chess_update
( void )
{
    if ( !state )
    {
        return false;
    }

    return true;
}


void
print_attacked
(SIDE side)
{
    char buf[512];
    u64 offs=0;
    for (u8 r = 0; r < 8; r++)
    {
        for (u8 f = 0; f < 8; f++)
        {
            if (!f)
            {
                offs += string_format(buf+offs, "  %d " , 8-r);
            }
            offs += string_format (buf+offs," %d", chess_square_attacked(SQUAREINDX(r,f),side) ? 1: 0 );
        }
        offs += string_format (buf+offs,"\n");
    }
    offs +=string_format(buf+offs,"\n     A B C D E F G H\n\n");
    platform_console_write(buf,PLATFORM_COLOR_CHESS_INFO);
}

void
chess_render
( void )
{
    if ( !state )
    {
        return;
    }
    
    char buf[ 32 ];
    string_format ( buf , "\n PLY: %d\n" , ( *state ).ply );
    platform_console_write ( buf , PLATFORM_COLOR_CHESS_INFO );

    chess_board_render ( &( *state ).board );

    // Temporary.
    print_attacked(BLACK);
}
