/**
 * @author Matthew Weissel (null@mattweissel.info)
 * @file chess.c
 * @brief Implementation of the chess header.
 * (see chess.h for additional details)
 */
#include "chess/chess.h"

#include "chess/attack.h"
#include "chess/board.h"
#include "chess/fen.h"
#include "chess/platform.h"

#include "core/logger.h"
#include "core/string.h"

// Defaults.
#define CHESS_RENDER_LINE_LENGTH 1024

// Type definition for chess engine subsystem state.
typedef struct
{
    attacks_t   attacks;
    board_t     board;
    u32         ply;
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

    // Initialize attack tables.
    attacks_init ( &( *state ).attacks );

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
chess_render
( void )
{
    if ( !state )
    {
        return;
    }
    
    char buf[ CHESS_RENDER_LINE_LENGTH ];
    char* str;
    
    string_format ( buf , "\n PLY: %d\n" , ( *state ).ply );
    platform_console_write ( buf , PLATFORM_COLOR_CHESS_INFO );

    chess_board_render ( &( *state ).board );

    // Temporary.

    str = string_bitboard ( bitboard_attackable ( &( *state ).board
                                                , &( *state ).attacks
                                                , BLACK
                                                ));
    platform_console_write ( str , PLATFORM_COLOR_CHESS_INFO );
    string_free ( str );
}
