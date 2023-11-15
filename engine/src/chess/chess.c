/**
 * @author Matthew Weissel (null@mattweissel.info)
 * @file chess.c
 * @brief Implementation of the chess header.
 * (see chess.h for additional details)
 */
#include "chess/chess.h"

#include "core/logger.h"
#include "core/string.h"

#include "math/math.h"

// Type definition for chess engine subsystem state.
typedef struct
{
    // Pregenerated attack tables for leaper pieces.
    bitboard_t  pawn_attacks[ 2 ][ 64 ];
    bitboard_t  knight_attacks[ 64 ];
    bitboard_t  king_attacks[ 64 ];
}
state_t;

// Global subsystem state.
static state_t* state;

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
        ( *state ).pawn_attacks[ WHITE ][ i ] = bitboard_mask_pawn_attack ( WHITE , i );
        ( *state ).pawn_attacks[ BLACK ][ i ] = bitboard_mask_pawn_attack ( BLACK , i );
        ( *state ).knight_attacks[ i ] = bitboard_mask_knight_attack ( i );
        ( *state ).king_attacks[ i ] = bitboard_mask_king_attack ( i );
    }

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

/*  TEMPORARY  */

#define prntbb(bb)                      \
    ({ char* s = string_bitboard (bb);  \
       LOGTRACE ( s );                  \
       string_free ( s );               \
     })

void
chess_run_tests
( void )
{
    if ( !state )
    {
        return;
    }
    
    prntbb ( random64 () );
    prntbb ( random64 () );
    prntbb ( random64 () );
    prntbb ( random64 () );
    prntbb ( random64 () );
    prntbb ( random64 () );
    prntbb ( random64 () );
    prntbb ( random64 () );
    prntbb ( random64 () );
    prntbb ( random64 () );
    prntbb ( random64 () );
    prntbb ( random64 () );
}
