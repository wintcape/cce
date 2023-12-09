/**
 * @file application.c
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Implementation of the application header.
 * (see application.h for additional details)
 */
#include "cce/application.h"

#include "cce/input.h"
#include "cce/platform.h"

#include "core/event.h"
#include "core/logger.h"
#include "core/memory.h"
#include "core/string.h"

#include "chess/chess.h"

// ---------- TEMPORARY ----------
#include <stdio.h>              //
// -------------------------------

// Defines textbuffer sizes.
#define CCE_RENDER_TEXTBUFFER_LENGTH    16384
#define CCE_COMMAND_TEXTBUFFER_LENGTH   8

// Type definition for game variant.
typedef enum
{
    CCE_GAME_PLAYER_VERSUS_PLAYER
,   CCE_GAME_PLAYER_VERSUS_ENGINE
,   CCE_GAME_ENGINE_VERSUS_ENGINE
,   CCE_GAME_TAG_COUNT
}
CCE_GAME_TAG;

// Type definition for game state.
typedef enum
{
    CCE_GAME_STATE_GAME_START
,   CCE_GAME_STATE_GAME_END
,   CCE_GAME_STATE_PROMPT_GAME_TYPE
,   CCE_GAME_STATE_PROMPT_COMMAND
,   CCE_GAME_STATE_EXECUTE_COMMAND
,   CCE_GAME_STATE_EXECUTE_MOVE
,   CCE_GAME_STATE_COUNT
}
CCE_GAME_STATE;

// Type definition for render tag.
typedef enum
{
    CCE_RENDER_NONE
,   CCE_RENDER_TITLE
,   CCE_RENDER_PROMPT_GAME_TYPE
,   CCE_RENDER_PROMPT_COMMAND
,   CCE_RENDER_LIST_COMMANDS
,   CCE_RENDER_LIST_MOVES
,   CCE_RENDER_BOARD
,   CCE_RENDER_TAG_COUNT
}
CCE_RENDER_TAG;

// Type definition for internal application state.
typedef struct
{
    // Game state.
    CCE_GAME_TAG    game;
    CCE_GAME_STATE  state;
    CCE_RENDER_TAG  render;
    u64             update;
    bool            ioerr;

    // Chess.
    attacks_t       attacks;
    board_t         board;
    moves_t         moves;
    move_t          move;

    // Textbuffers.
    char            textbuffer[ CCE_RENDER_TEXTBUFFER_LENGTH ];
    char            cmd[ CCE_COMMAND_TEXTBUFFER_LENGTH ];
}
state_t;

// Global application state.
static cce_t* cce;

/**
 * @brief Primary implementation of cce_update and cce_render.
 * (see cce_update, cce_render).
 */
bool cce_game_start              ( void );
bool cce_game_end                ( void );
bool cce_prompt_game_type        ( void );
bool cce_prompt_command          ( void );
bool cce_execute_command         ( void );
bool cce_execute_move            ( void );
void cce_render_title            ( void );
void cce_render_prompt_game_type ( void );
void cce_render_prompt_command   ( void );
void cce_render_list_commands    ( void );
void cce_render_list_moves       ( void );
void cce_render_board            ( void );

bool
cce_startup
(   application_t*  app
)
{
    // Allocate memory for application state, including internal game state.
    cce = memory_allocate ( sizeof ( cce_t ) + sizeof ( state_t )
                          , MEMORY_TAG_APPLICATION
                          );
    ( *app ).app_state = cce;
    ( *cce ).internal = ( ( void* ) cce ) + sizeof ( cce_t );

    // Initialize game state.
    
    state_t* state = ( *cce ).internal;
    
    ( *state ).update = 0;
    ( *state ).ioerr = false;

    memory_clear ( ( *state ).textbuffer
                 , sizeof ( ( *state ).textbuffer )
                 );
    memory_clear ( ( *state ).cmd
                 , sizeof ( ( *state ).cmd )
                 );

    // Pregenerate attack tables.
    attacks_init ( &( *state ).attacks );

    // Initialize chess board.
    ( *state ).board.history = 0;
    fen_parse ( FEN_START , &( *state ).board );
    
    // Populate move list.
    moves_get ( &( *state ).moves
              , &( *state ).board
              , &( *state ).attacks
              );

    ( *state ).state = CCE_GAME_STATE_GAME_START;
    return true;
}

void
cce_shutdown
(   application_t*  app
)
{
    if ( !cce )
    {
        return;
    }

    // Free memory used by application.
    memory_free ( cce
                , sizeof ( cce_t ) + sizeof ( state_t )
                , MEMORY_TAG_APPLICATION
                );
    cce = 0;
}

bool
cce_update
(   application_t*  app
,   f32             dt
)
{
    if ( !cce )
    {
        return false;
    }
    
    state_t* state = ( *cce ).internal;

    ( *state ).update += 1;
    
    switch ( ( *state ).state )
    {
        case CCE_GAME_STATE_GAME_START:       return cce_game_start ()       ;
        case CCE_GAME_STATE_GAME_END:         return cce_game_end ()         ;
        case CCE_GAME_STATE_PROMPT_GAME_TYPE: return cce_prompt_game_type () ;
        case CCE_GAME_STATE_PROMPT_COMMAND:   return cce_prompt_command ()   ;
        case CCE_GAME_STATE_EXECUTE_COMMAND:  return cce_execute_command ()  ;
        case CCE_GAME_STATE_EXECUTE_MOVE:     return cce_execute_move ()     ;
        default:                              break                          ;
    }

    return true;
}

void
cce_render
(   application_t*  app
,   f32             dt
)
{
    if ( !cce )
    {
        return;
    }

    state_t* state = ( *cce ).internal;
    switch ( ( *state ).render )
    {
        case CCE_RENDER_NONE:                                               break;
        case CCE_RENDER_TITLE:              cce_render_title ()            ;break;
        case CCE_RENDER_PROMPT_GAME_TYPE:   cce_render_prompt_game_type () ;break;
        case CCE_RENDER_PROMPT_COMMAND:     cce_render_prompt_command ()   ;break;
        case CCE_RENDER_LIST_COMMANDS:      cce_render_list_commands ()    ;break;
        case CCE_RENDER_LIST_MOVES:         cce_render_list_moves ()       ;break;
        case CCE_RENDER_BOARD:              cce_render_board ()            ;break;
        default:                                                            break;
    }
}

bool
cce_game_start
( void )
{
    state_t* state = ( *cce ).internal;
    ( *state ).render = CCE_RENDER_TITLE;
    ( *state ).state = CCE_GAME_STATE_PROMPT_GAME_TYPE;
    return true;
}

bool
cce_game_end
( void )
{
    state_t* state = ( *cce ).internal;

    // Signal engine to shutdown.
    event_context_t ctx = {};
    event_fire ( EVENT_CODE_APPLICATION_QUIT , 0 , ctx );

    ( *state ).render = CCE_RENDER_NONE;
    ( *state ).state = CCE_GAME_STATE_COUNT;
    return true;
}

bool
cce_prompt_game_type
( void )
{
    state_t* state = ( *cce ).internal;
    
    // Clear any previous user input.
    ( *state ).cmd[ 0 ] = 0;

    // Read new user input.
    if ( scanf ( "%8s" , ( *state ).cmd ) <= 0 )
    {
        LOGERROR ( "cce_prompt_game_type: Failed to read user input from stdin." );
        return false;
    }
    
    // Flush the input stream.
    int garbage;
    while ( ( garbage = getchar () ) != '\n' && garbage != EOF );
       
    // Sanitize input string.
    ( *state ).cmd[ sizeof ( ( *state ).cmd ) - 2 ] = 0;
    
    // Validate input string.
    if ( !(   string_length ( ( *state ).cmd ) == 1
           && (   ( *state ).cmd[ 0 ] == '1'
               || ( *state ).cmd[ 0 ] == '2'
               || ( *state ).cmd[ 0 ] == '3'
              )))
    {
        ( *state ).ioerr = true;
        return true;
    }

    ( *state ).render = CCE_RENDER_BOARD;
    ( *state ).state = CCE_GAME_STATE_GAME_END;
    return true;
}

bool
cce_prompt_command
( void )
{
    return true;
}

bool
cce_execute_command
( void )
{
    return true;
}

bool
cce_execute_move
( void )
{
    return true;
}

void
cce_render_title
( void )
{
    state_t* state = ( *cce ).internal;
    string_format ( ( *state ).textbuffer
                  , "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
                    "\n\t=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-="
                    "\n\t~                                                 ~"
                    "\n\t~                CCE CHESS ENGINE.                ~"
                    "\n\t~                (   v. %i.%i.%i   )                 ~"
                    "\n\t~           created by Matthew Weissel.           ~"
                    "\n\t~                                                 ~"
                    "\n\t=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-="
                    "\n\n\n"
                  , VERSION_MAJOR , VERSION_MINOR , VERSION_PATCH
                  );
    platform_console_write ( ( *state ).textbuffer , PLATFORM_COLOR_CCE_INFO );
    cce_render_prompt_game_type ();
}

void
cce_render_prompt_game_type
( void )
{
    state_t* state = ( *cce ).internal;
    if ( !( *state ).ioerr )
    {
        platform_console_write ( "\n"
                                 "\n\t                CHOOSE A GAME STYLE:               "
                                 "\n\t                                                   "
                                 "\n\t            [ 1 ]  PLAYER VERSUS PLAYER.           "
                                 "\n\t            [ 2 ]  PLAYER VERSUS ENGINE.           "
                                 "\n\t            [ 3 ]  ENGINE VERSUS ENGINE.           "
                                 "\n\t                                                   "
                                 "\n\tCHOICE:        "
                               , PLATFORM_COLOR_CCE_INFO
                               );
    }
    else
    {
        platform_console_write ( "\n\tPLEASE CHOOSE FROM THE OPTIONS PROVIDED."
                               , LOG_ERROR
                               );
        platform_console_write ( "\n\n"
                                 "\n\t                CHOOSE A GAME STYLE:               "
                                 "\n\t                                                   "
                                 "\n\t            [ 1 ]  PLAYER VERSUS PLAYER.           "
                                 "\n\t            [ 2 ]  PLAYER VERSUS ENGINE.           "
                                 "\n\t            [ 3 ]  ENGINE VERSUS ENGINE.           "
                                 "\n\t                                                   "
                               , PLATFORM_COLOR_CCE_INFO
                               );
        platform_console_write ( "\n\tCHOICE:        "
                               , PLATFORM_COLOR_CCE_INFO
                               );
    }
}

void
cce_render_prompt_command
( void )
{
    platform_console_write ( "\n\n\tISSUE MOVE OR COMMAND:  "
                           , PLATFORM_COLOR_CCE_INFO
                           );
}

void
cce_render_list_commands
( void )
{}

void
cce_render_list_moves
( void )
{}

void
cce_render_board
( void )
{
    state_t* state = ( *cce ).internal;
    platform_console_write ( "\n\n" , PLATFORM_COLOR_CCE_INFO );
    board_render ( ( *state ).textbuffer
                 , &( *state ).board
                 , PLATFORM_COLOR_CCE_INFO
                 );
    platform_console_write ( "\n\n" , PLATFORM_COLOR_CCE_INFO );
}
