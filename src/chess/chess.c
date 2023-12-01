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
#include "chess/move.h"
#include "chess/platform.h"
#include "chess/string.h"

#include "core/logger.h"
#include "core/string.h"

// Type definition for chess engine subsystem state.
typedef struct
{
    // Pregenerated attack table.
    attacks_t   attacks;

    // Game state.
    board_t     board;
    u32         ply;

    // Render textbuffer.
    char        textbuffer[ CHESS_RENDER_TEXTBUFFER_LENGTH ];

    // Temporary.
    moves_t moves;
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
    attacks_init ( &( *state ).attacks );

    // Initialize game state.
    fen_parse ( FEN_START , &( *state ).board );
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

    ( *state ).ply += 1;

    moves_push ( &( *state ).moves
               , move_encode ( D7
                             , E8
                             , P
                             , Q
                             , true
                             , false
                             , false
                             , false
                             ));

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
    
    string_format ( ( *state ).textbuffer
                  , "\n PLY: %d\n"
                  , ( *state ).ply
                  );
    platform_console_write ( ( *state ).textbuffer
                           , PLATFORM_COLOR_CHESS_INFO
                           );

    board_render ( ( *state ).textbuffer , &( *state ).board );

    string_moves ( ( *state ).textbuffer , &( *state ).moves );
    platform_console_write ( ( *state ).textbuffer , PLATFORM_COLOR_CHESS_INFO );
}

