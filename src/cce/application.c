/**
 * @file application.c
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Implementation of the application header.
 * (see application.h for additional details)
 */
#include "cce/application.h"

#include "cce/input.h"
#include "cce/platform.h"

#include "core/clock.h"
#include "core/event.h"
#include "core/logger.h"
#include "core/memory.h"
#include "core/string.h"

#include "math/math.h"

#include "chess/chess.h"

// Defines textbuffer sizes.
#define CCE_RENDER_TEXTBUFFER_LENGTH    65535
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
    CCE_GAME_STATE_GAME_INIT
,   CCE_GAME_STATE_GAME_START
,   CCE_GAME_STATE_GAME_END
,   CCE_GAME_STATE_PROMPT_GAME_TYPE
,   CCE_GAME_STATE_PROMPT_COMMAND
,   CCE_GAME_STATE_EXECUTE_COMMAND
,   CCE_GAME_STATE_EXECUTE_MOVE_PLAYER
,   CCE_GAME_STATE_EXECUTE_MOVE_ENGINE

,   CCE_GAME_STATE_COUNT
}
CCE_GAME_STATE;

// Type definition for render tag.
typedef enum
{
    CCE_RENDER_NONE
,   CCE_RENDER_START
,   CCE_RENDER_END
,   CCE_RENDER_GAME_SELECTED
,   CCE_RENDER_PROMPT_GAME_TYPE
,   CCE_RENDER_PROMPT_COMMAND
,   CCE_RENDER_EXECUTE_COMMAND
,   CCE_RENDER_EXECUTE_MOVE_PLAYER
,   CCE_RENDER_EXECUTE_MOVE_ENGINE
,   CCE_RENDER_CHECK

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
    u32             ioerr;
    
    // Chess.
    attacks_t       attacks;
    board_t         board;
    moves_t         moves;
    move_t          move;
    u32             ply;

    // Benchmarking.
    clock_t         clock;
    f64             elapsed;

    // Textbuffers.
    u64             textbuffer_offs;
    char            textbuffer[ CCE_RENDER_TEXTBUFFER_LENGTH ];
    char            cmd[ CCE_COMMAND_TEXTBUFFER_LENGTH ];
}
state_t;

// Global application state.
static cce_t* cce;

/**
 * @brief Primary implementation of cce_update (see cce_update).
 */
bool cce_game_init                  ( void );
bool cce_game_start                 ( void );
bool cce_game_end                   ( void );
bool cce_prompt_game_type           ( void );
bool cce_prompt_command             ( void );
bool cce_execute_command            ( void );
bool cce_execute_move_player        ( void );
bool cce_execute_move_engine        ( void );

/**
 * @brief Primary implementation of cce_render (see cce_render).
 */
void cce_render_start               ( void );
void cce_render_end                 ( void );
void cce_render_game_selected       ( void );
void cce_render_prompt_game_type    ( void );
void cce_render_prompt_command      ( void );
void cce_render_execute_command     ( void );
void cce_render_execute_move_player ( void );
void cce_render_execute_move_engine ( void );
void cce_render_check               ( void );
void cce_render_splash_title        ( void );
void cce_render_splash_end          ( void );
void cce_render_list_commands       ( void );
void cce_render_list_moves          ( void );
void cce_render_board               ( void );
void cce_render_move                ( void );
void cce_render_buffer              ( void );

/**
 * @brief Renders the application state to the console.
 */
#define RENDER()        \
    ( platform_console_write ( ( *state ).textbuffer ) )
    
/**
 * @brief Clears the render buffer.
 */
#define RENDER_CLEAR()  \
    ( ( *state ).textbuffer_offs = 0 )

bool
cce_startup
(   application_t*  app
)
{
    // Allocate memory for the application, including internal game state.
    cce = memory_allocate ( sizeof ( cce_t ) + sizeof ( state_t )
                          , MEMORY_TAG_APPLICATION
                          );
    ( *app ).app_state = cce;
    ( *cce ).internal = ( ( void* ) cce ) + sizeof ( cce_t );

    // Initialize game state.
    
    state_t* state = ( *cce ).internal;
    
    ( *state ).update = 0;
    ( *state ).ioerr = 0;

    ( *state ).textbuffer_offs = 0;
    memory_clear ( ( *state ).textbuffer
                 , sizeof ( ( *state ).textbuffer )
                 );
    memory_clear ( ( *state ).cmd
                 , sizeof ( ( *state ).cmd )
                 );

    // Pregenerate attack tables.
    attacks_init ( &( *state ).attacks );

    ( *state ).render = CCE_RENDER_NONE;
    ( *state ).state = CCE_GAME_STATE_GAME_INIT;
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

    // Free memory used by the application.
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
        case CCE_GAME_STATE_GAME_INIT          : return cce_game_init ()           ;
        case CCE_GAME_STATE_GAME_START         : return cce_game_start ()          ;
        case CCE_GAME_STATE_GAME_END           : return cce_game_end ()            ;
        case CCE_GAME_STATE_PROMPT_GAME_TYPE   : return cce_prompt_game_type ()    ;
        case CCE_GAME_STATE_PROMPT_COMMAND     : return cce_prompt_command ()      ;
        case CCE_GAME_STATE_EXECUTE_COMMAND    : return cce_execute_command ()     ;
        case CCE_GAME_STATE_EXECUTE_MOVE_PLAYER: return cce_execute_move_player () ;
        case CCE_GAME_STATE_EXECUTE_MOVE_ENGINE: return cce_execute_move_engine () ;
        default                                : break                             ;
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
    const CCE_RENDER_TAG render = ( *state ).render;

    // Write game state info to the textbuffer.
    switch ( render )
    {
        case CCE_RENDER_NONE               : return          ;
        case CCE_RENDER_START              : RENDER_CLEAR () ; cce_render_start ()               ;break;
        case CCE_RENDER_END                : RENDER_CLEAR () ; cce_render_end ()                 ;break;
        case CCE_RENDER_GAME_SELECTED      : RENDER_CLEAR () ; cce_render_game_selected ()       ;break;
        case CCE_RENDER_PROMPT_GAME_TYPE   : RENDER_CLEAR () ; cce_render_prompt_game_type ()    ;break;
        case CCE_RENDER_PROMPT_COMMAND     : RENDER_CLEAR () ; cce_render_prompt_command ()      ;break;
        case CCE_RENDER_EXECUTE_COMMAND    : RENDER_CLEAR () ; cce_render_execute_command ()     ;break;
        case CCE_RENDER_EXECUTE_MOVE_PLAYER: RENDER_CLEAR () ; cce_render_execute_move_player () ;break;
        case CCE_RENDER_EXECUTE_MOVE_ENGINE: RENDER_CLEAR () ; cce_render_execute_move_engine () ;break;
        case CCE_RENDER_CHECK              : RENDER_CLEAR () ; cce_render_check ()               ;break;
        default                            : RENDER_CLEAR () ; break                             ;
    }

    // Render the textbuffer to the console.
    RENDER ();
}

// ---------- TEMPORARY ----------
#include <stdio.h>              // TODO: Eliminate scanf calls for user input.
// -------------------------------

bool
cce_game_init
( void )
{
    state_t* state = ( *cce ).internal;

    ( *state ).render = CCE_RENDER_START;
    ( *state ).state = CCE_GAME_STATE_PROMPT_GAME_TYPE;
    return true;
}

bool
cce_game_start
( void )
{
    state_t* state = ( *cce ).internal;

    ( *state ).ioerr = 0;
    ( *state ).ply = 0;

    ( *state ).elapsed = 0;

    // Initialize chess board.
    ( *state ).board.history = 0;
    fen_parse ( FEN_START , &( *state ).board );
    
    // Populate move list.
    moves_get ( &( *state ).moves
              , &( *state ).board
              , &( *state ).attacks
              );

    ( *state ).render = CCE_RENDER_GAME_SELECTED;
    ( *state ).state = ( ( *state ).game == CCE_GAME_ENGINE_VERSUS_ENGINE ) ? CCE_GAME_STATE_EXECUTE_MOVE_ENGINE
                                                                            : CCE_GAME_STATE_PROMPT_COMMAND
                                                                            ;
    return true;
}

bool
cce_game_end
( void )
{
    state_t* state = ( *cce ).internal;

    // Render ending splashtext.
    // Render immediately, since shutdown is coming next.
    RENDER_CLEAR ();
    cce_render_end ();
    RENDER ();

    // Signal engine to shutdown the application.
    event_context_t ctx = {};
    event_fire ( EVENT_CODE_APPLICATION_QUIT , 0 , ctx );

    // Turn off rendering and clear the game state.
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
        ( *state ).ioerr += 1;
        return true;
    }
    ( *state ).ioerr = 0;

    // Set the game type.
    ( *state ).game = chri ( ( *state ).cmd[ 0 ] ) - 1;

    ( *state ).render = CCE_RENDER_NONE;
    ( *state ).state = CCE_GAME_STATE_GAME_START;
    return true;
}

bool
cce_prompt_command
( void )
{
    state_t* state = ( *cce ).internal;

    ( *state ).render = CCE_RENDER_PROMPT_COMMAND;
    
    // Clear any previous user input.
    ( *state ).cmd[ 0 ] = 0;

    // Read new user input.
    if ( scanf ( "%8s" , ( *state ).cmd ) <= 0 )
    {
        LOGERROR ( "cce_prompt_command: Failed to read user input from stdin." );
        return false;
    }
    
    // Flush the input stream.
    int garbage;
    while ( ( garbage = getchar () ) != '\n' && garbage != EOF );
       
    // Sanitize input string.
    ( *state ).cmd[ sizeof ( ( *state ).cmd ) - 2 ] = 0;
    
    // Validate input string.
    
    bool valid;
    const u8 len = string_length ( ( *state ).cmd );

    // Validate if input is a command key signal.
    if ( len == 1 )
    {
        ( *state ).cmd[ 0 ] = to_lowercase ( ( *state ).cmd[ 0 ] );
        switch ( ( *state ).cmd[ 0 ] )
        {
            case CCE_KEY_SIGNAL_COMMAND_HELP              :
            case CCE_KEY_SIGNAL_COMMAND_LIST_MOVES        :
            case CCE_KEY_SIGNAL_COMMAND_CHOOSE_RANDOM_MOVE:
            case CCE_KEY_SIGNAL_COMMAND_QUIT              : valid = true  ;break;
            default                                       : valid = false ;break;
        }
        
        if ( !valid )
        {
            ( *state ).ioerr += 1;
            return true;
        }
        ( *state ).ioerr = 0;

        ( *state ).render = CCE_RENDER_NONE;
        ( *state ).state = CCE_GAME_STATE_EXECUTE_COMMAND;
        return true;
    }

    // Validate move.

    // Sanitize.
    for ( u8 i = 0; i < MOVE_STRING_LENGTH; ++i )
    {
        ( *state ).cmd[ i ] = to_uppercase ( ( *state ).cmd[ i ] );
    }

    // Validate input format.
    if ( len < MOVE_STRING_LENGTH - 1 || len > MOVE_STRING_LENGTH )
    {
        ( *state ).ioerr += 1;
        return true;
    }
    else if (   ( *state ).cmd[ 0 ] < 'A'
             || ( *state ).cmd[ 0 ] > 'H'
             || ( *state ).cmd[ 1 ] < '1'
             || ( *state ).cmd[ 1 ] > '8'
             || ( *state ).cmd[ 2 ] < 'A'
             || ( *state ).cmd[ 2 ] > 'H'
             || ( *state ).cmd[ 3 ] < '1'
             || ( *state ).cmd[ 3 ] > '8'
            )
    {
        ( *state ).ioerr += 1;
        return true;
    }
    else if ( len == MOVE_STRING_LENGTH )
    {
        if (   ( *state ).cmd[ MOVE_STRING_LENGTH - 1 ] != 'N'
            && ( *state ).cmd[ MOVE_STRING_LENGTH - 1 ] != 'B'
            && ( *state ).cmd[ MOVE_STRING_LENGTH - 1 ] != 'R'
            && ( *state ).cmd[ MOVE_STRING_LENGTH - 1 ] != 'Q'
           )
        {
            ( *state ).ioerr += 1;
            return true;
        }
    }
    else
    {
        // Sanitize for string comparison.
        ( *state ).cmd[ MOVE_STRING_LENGTH - 1 ] = ' ';
        ( *state ).cmd[ MOVE_STRING_LENGTH ] = 0;
    }
    ( *state ).ioerr += 0;
    
    // Check if present in list of valid move options.
    valid = false;
    for ( u32 i = 0; i < ( *state ).moves.count; ++i )
    {
        if ( string_equal ( ( *state ).cmd
                          , string_move ( ( *state ).textbuffer
                                        , ( *state ).moves.moves[ i ]
                                        )))
        {
            ( *state ).move = ( *state ).moves.moves[ i ];
            valid = true;
            break;
        }
    }
    
    if ( !valid )
    {
        ( *state ).ioerr += 1;
            return true;
    }
    ( *state ).ioerr = 0;

    ( *state ).render = CCE_RENDER_NONE;
    ( *state ).state = CCE_GAME_STATE_EXECUTE_MOVE_PLAYER;
    return true;
}

bool
cce_execute_command
( void )
{
    state_t* state = ( *cce ).internal;
    
    // Evaluate command.
    switch ( ( *state ).cmd[ 0 ] )
    {
            case CCE_KEY_SIGNAL_COMMAND_HELP:
            case CCE_KEY_SIGNAL_COMMAND_LIST_MOVES:
            {
                ( *state ).render = CCE_RENDER_EXECUTE_COMMAND;
                ( *state ).state = CCE_GAME_STATE_PROMPT_COMMAND;
                return true;
            }

            case CCE_KEY_SIGNAL_COMMAND_CHOOSE_RANDOM_MOVE:
            {
                ( *state ).move = ( *state ).moves.moves[ random2 ( 0 , ( *state ).moves.count - 1 ) ];
                ( *state ).render = CCE_RENDER_NONE;
                ( *state ).state = CCE_GAME_STATE_EXECUTE_MOVE_PLAYER;
                return true;
            }
            
            case CCE_KEY_SIGNAL_COMMAND_QUIT:
            {
                ( *state ).render = CCE_RENDER_NONE;
                ( *state ).state = CCE_GAME_STATE_GAME_END;
                return true;
            }
            
            default:
            {
                LOGERROR ( "cce_execute_command: called, but '%c' is not a known command."
                         , ( *state ).cmd[ 0 ]
                         );
                return false;
            }
    }
    return true;
}

bool
cce_execute_move_player
( void )
{
    state_t* state = ( *cce ).internal;
    
    // Parse the move.
    if ( !move_parse ( ( *state ).move
                     , MOVE_FILTER_NONE
                     , &( *state ).attacks
                     , &( *state ).board
                     ))
    {
        ( *state ).render = CCE_RENDER_CHECK;
        ( *state ).state = CCE_GAME_STATE_GAME_END;
        return true;
    }

    ( *state ).ply += 1;

    // Populate move list.
    moves_get ( &( *state ).moves
              , &( *state ).board
              , &( *state ).attacks
              );
    
    ( *state ).render = CCE_RENDER_EXECUTE_MOVE_PLAYER;
    ( *state ).state = ( ( *state ).game == CCE_GAME_PLAYER_VERSUS_ENGINE ) ? CCE_GAME_STATE_EXECUTE_MOVE_ENGINE
                                                                            : CCE_GAME_STATE_PROMPT_COMMAND
                                                                            ;
    return true;
}

bool
cce_execute_move_engine
( void )
{
    state_t* state = ( *cce ).internal;
    
    // Render simulated input.
    RENDER_CLEAR ();
    string_format ( ( *state ).textbuffer
                  , CCE_COLOR_HINT "Calculating best move. . .  "
                  );
    RENDER ();

    // Start clock.
    clock_start ( &( *state ).clock );
    
    // Calculate best move. ( TODO: Make this not just a random move. . . )
    ( *state ).move = ( *state ).moves.moves[ random2 ( 0 , ( *state ).moves.count - 1 ) ];

    // Stop clock.
    clock_update ( &( *state ).clock );
    ( *state ).elapsed += ( *state ).clock.elapsed;

    // Render simulated input.
    RENDER_CLEAR ();
    string_format ( ( *state ).textbuffer
                  , CCE_COLOR_HIGHLIGHT "%s"
                  , string_move ( ( *state ).textbuffer , ( *state ).move )
                  );
    const u64 indx = string_length ( ( *state ).textbuffer ) - 1;
    if ( ( *state ).textbuffer[ indx ] == ' ' )
    {
        ( *state ).textbuffer[ indx ] = 0;
    }
    RENDER ();

    // Parse the move.
    if ( !move_parse ( ( *state ).move
                     , MOVE_FILTER_NONE
                     , &( *state ).attacks
                     , &( *state ).board
                     ))
    {
        ( *state ).render = CCE_RENDER_CHECK;
        ( *state ).state = CCE_GAME_STATE_GAME_END;
        return true;
    }

    ( *state ).ply += 1;

    // Populate move list.
    moves_get ( &( *state ).moves
              , &( *state ).board
              , &( *state ).attacks
              );
    
    ( *state ).render = CCE_RENDER_EXECUTE_MOVE_ENGINE;
    ( *state ).state = ( ( *state ).game == CCE_GAME_ENGINE_VERSUS_ENGINE ) ? CCE_GAME_STATE_EXECUTE_MOVE_ENGINE
                                                                            : CCE_GAME_STATE_PROMPT_COMMAND
                                                                            ;
    return true;
}

void
cce_render_start
( void )
{
    // ---------- TEMPORARY ----------
    cce_render_buffer ();           // Refreshes console by rendering newlines.
    // -------------------------------

    // Render splashtext.
    cce_render_splash_title ();

    // Peek first prompt.
    cce_render_prompt_game_type ();
}

void
cce_render_end
( void )
{
    state_t* state = ( *cce ).internal;

    // Render benchmarking info.
    if ( ( *state ).game == CCE_GAME_ENGINE_VERSUS_ENGINE )
    {
        ( *state ).textbuffer_offs += string_format ( ( *state ).textbuffer + ( *state ).textbuffer_offs
                                                    , CCE_COLOR_HINT "\n\tEngine calculations took a total of %f seconds.\n\n"
                                                    , ( *state ).elapsed
                                                    );
    }

    // Render splashtext.
    cce_render_splash_end ();
}

void
cce_render_game_selected
( void )
{
    state_t* state = ( *cce ).internal;
    const CCE_GAME_TAG game = ( *state ).game;
        
    // ---------- TEMPORARY ----------
    cce_render_buffer ();           // Refreshes console by rendering newlines.
    // -------------------------------
    
    // Render splashtext.
    cce_render_splash_title ();

    // Render info.
    const char* s_game;
    switch ( game )
    {
        case CCE_GAME_PLAYER_VERSUS_PLAYER: s_game = "PLAYER VERSUS PLAYER" ;break;
        case CCE_GAME_PLAYER_VERSUS_ENGINE: s_game = "PLAYER VERSUS ENGINE" ;break;
        case CCE_GAME_ENGINE_VERSUS_ENGINE: s_game = "ENGINE VERSUS ENGINE" ;break;
        default                           : s_game = ""                     ;break;
    }
    ( *state ).textbuffer_offs += string_format ( ( *state ).textbuffer + ( *state ).textbuffer_offs
                                                , CCE_COLOR_PLUS "\n\tStarting new game of %s. . .\n\n\n"
                                                , s_game
                                                );
    
    // Render starting board state.
    cce_render_board ();

    // Render first prompt.
    cce_render_prompt_command ();
}

void
cce_render_prompt_game_type
( void )
{
    state_t* state = ( *cce ).internal;
    ( *state ).textbuffer_offs += string_format ( ( *state ).textbuffer + ( *state ).textbuffer_offs
                                                , CCE_COLOR_DEFAULT "\n\t%s" CCE_COLOR_INFO "\n"
                                                  "\n\t                CHOOSE A GAME STYLE:               "
                                                  "\n\t                                                   "
                                                  "\n\t            [ 1 ]  PLAYER VERSUS PLAYER.           "
                                                  "\n\t            [ 2 ]  PLAYER VERSUS ENGINE.           "
                                                  "\n\t            [ 3 ]  ENGINE VERSUS ENGINE.           "
                                                  "\n\t                                                   "
                                                  "\n\tCHOICE:       "
                                                , ( ( *state ).ioerr ) ? CCE_COLOR_ALERT "\tPLEASE CHOOSE FROM THE OPTIONS PROVIDED."
                                                                       : "\n"
                                                );
}

void
cce_render_prompt_command
( void )
{
    state_t* state = ( *cce ).internal;

    if ( ( *state ).ioerr )
    {
        // ---------- TEMPORARY ----------
        cce_render_buffer ();           // Refresh console by rendering newlines.
        // -------------------------------

        // Render hint.
        if ( ( *state ).ioerr > 3 )
        {
            ( *state ).textbuffer_offs += string_format ( ( *state ).textbuffer + ( *state ).textbuffer_offs
                                                        , CCE_COLOR_HINT "\n\n\tTrying to exit? Pass 'q' as the next move."
                                                                           "\n\tNeed help? Pass 'h' for help options.\n"
                                                        );
        }

        // Render the board again.
        cce_render_board ();

        // Render prompt.
        if (   string_length ( ( *state ).cmd ) == MOVE_STRING_LENGTH
            && ( *state ).cmd[ MOVE_STRING_LENGTH - 1 ] == ' '
           )
        {
            ( *state ).cmd[ MOVE_STRING_LENGTH - 1 ] = 0;
        }

        ( *state ).textbuffer_offs += string_format ( ( *state ).textbuffer + ( *state ).textbuffer_offs
                                                    , CCE_COLOR_INFO "\n\t" CCE_COLOR_DEFAULT
                                                      CCE_COLOR_HIGHLIGHT "%s"
                                                      CCE_COLOR_DEFAULT CCE_COLOR_ALERT " is not a known command or valid chess move."
                                                      CCE_COLOR_INFO    "\n\tISSUE MOVE OR COMMAND:  " CCE_COLOR_DEFAULT
                                                    , ( *state ).cmd
                                                    );
    }
    else
    {
        ( *state ).textbuffer_offs += string_format ( ( *state ).textbuffer + ( *state ).textbuffer_offs
                                                    , CCE_COLOR_INFO "\n\n\n\tISSUE MOVE OR COMMAND:  " CCE_COLOR_DEFAULT
                                                    );
    }
}

void
cce_render_execute_command
( void )
{
    state_t* state = ( *cce ).internal;
            
    // ---------- TEMPORARY ----------
    cce_render_buffer ();           // Refreshes console by rendering newlines.
    // -------------------------------
    
    // Evaluate command.
    switch ( ( *state ).cmd[ 0 ] )
    {
            case CCE_KEY_SIGNAL_COMMAND_HELP      : cce_render_list_commands () ;break;
            case CCE_KEY_SIGNAL_COMMAND_LIST_MOVES: cce_render_list_moves ()    ;break;
            default:                                                             break;
    }
    
    // Render the board state.
    cce_render_board ();

    // Render the prompt.
    cce_render_prompt_command ();
}

void
cce_render_execute_move_player
( void )
{
    // Render the move.
    cce_render_move ();

    // Render the board state.
    cce_render_board ();

    // Render the prompt.
    cce_render_prompt_command ();
}

void
cce_render_execute_move_engine
( void )
{
    state_t* state = ( *cce ).internal;

    // Render the time taken to choose the move.
    ( *state ).textbuffer_offs += string_format ( ( *state ).textbuffer + ( *state ).textbuffer_offs
                                                , CCE_COLOR_HINT "\n\t\t\t\tTook %f seconds."
                                                , ( *state ).clock.elapsed
                                                );

    // Render the move.
    cce_render_move ();

    // Render the board state.
    cce_render_board ();

    // Render the prompt.
    cce_render_prompt_command ();
}

void
cce_render_check
( void )
{
    state_t* state = ( *cce ).internal;
    ( *state ).textbuffer_offs += string_format ( ( *state ).textbuffer + ( *state ).textbuffer_offs
                                                , CCE_COLOR_PLUS "\n\n\t%s CHECK: %s" CCE_COLOR_DEFAULT
                                                , ( ( *state ).state == CCE_GAME_STATE_EXECUTE_MOVE_PLAYER ) ? ( ( *state ).game == CCE_GAME_PLAYER_VERSUS_ENGINE ) ? "WHITE (player)"
                                                                                                                                                                    : ( ( *state ).board.side == WHITE ) ? "WHITE"
                                                                                                                                                                                                         : "BLACK"
                                                                                                             : ( ( *state ).game == CCE_GAME_PLAYER_VERSUS_ENGINE ) ? "BLACK (engine)"
                                                                                                                                                                    : ( ( *state ).board.side == WHITE ) ? "WHITE"
                                                                                                                                                                                                         : "BLACK"
                                                , string_move ( ( *state ).textbuffer
                                                              , ( *state ).move
                                                              ));
}

void
cce_render_splash_title
( void )
{
    state_t* state = ( *cce ).internal;
    ( *state ).textbuffer_offs += string_format ( ( *state ).textbuffer + ( *state ).textbuffer_offs
                                                , CCE_COLOR_INFO
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
}

void
cce_render_splash_end
( void )
{
    state_t* state = ( *cce ).internal;
    ( *state ).textbuffer_offs += string_format ( ( *state ).textbuffer + ( *state ).textbuffer_offs
                                                , CCE_COLOR_INFO
                                                  "\n\t=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-="
                                                  "\n\t~                                                 ~"
                                                  "\n\t~                                                 ~"
                                                  "\n\t~               Thanks for playing!               ~"
                                                  "\n\t~                                                 ~"
                                                  "\n\t~                                                 ~"
                                                  "\n\t=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-="
                                                  "\n\n\n" ANSI_CC_RESET
                                                );
}

void
cce_render_list_commands
( void )
{
    state_t* state = ( *cce ).internal;
    ( *state ).textbuffer_offs += string_format ( ( *state ).textbuffer + ( *state ).textbuffer_offs
                                                , CCE_COLOR_HINT
                                                  "\n\t                                                   "
                                                  "\n\t         =-=-=- AVAILABLE COMMANDS -=-=-=          "
                                                  "\n\t                                                   "
                                                  "\n\t  %c :      List available commands.               "
                                                  "\n\t  %c :      List available moves.                  "
                                                  "\n\t  %c :      Choose random valid move.              "
                                                  "\n                                                     "
                                                  "\n\t  %c :      Quit the application.                  "
                                                  "\n\t                                                   "
                                                  "\n\t         =-=-=-                    -=-=-=          "
                                                  "\n\n"
                                                , CCE_KEY_SIGNAL_COMMAND_HELP
                                                , CCE_KEY_SIGNAL_COMMAND_LIST_MOVES
                                                , CCE_KEY_SIGNAL_COMMAND_CHOOSE_RANDOM_MOVE
                                                , CCE_KEY_SIGNAL_COMMAND_QUIT
                                                );
}

void
cce_render_list_moves
( void )
{
    state_t* state = ( *cce ).internal;
    ( *state ).textbuffer_offs += string_format ( ( *state ).textbuffer + ( *state ).textbuffer_offs
                                                , CCE_COLOR_HINT "%s"
                                                , string_moves ( ( *state ).textbuffer + ( *state ).textbuffer_offs
                                                               , &( *state ).moves
                                                               ));
}

void
cce_render_board
( void )
{
    state_t* state = ( *cce ).internal;

    ( *state ).textbuffer_offs += string_format ( ( *state ).textbuffer + ( *state ).textbuffer_offs
                                                , CCE_COLOR_INFO "\n\tBOARD: %s\n"
                                                , fen_from_board ( ( *state ).textbuffer + ( *state ).textbuffer_offs
                                                                 , &( *state ).board
                                                                 ));

    u8 i = 0;
    u8 r = 0;
    u8 f = 0;

    while ( r < 8 )
    {
        ( *state ).textbuffer_offs += string_format ( ( *state ).textbuffer + ( *state ).textbuffer_offs
                                                    , CCE_COLOR_INFO "\n\t\t  "
                                                    );
        while ( f < 8 )
        {
            if ( !f )
            {
                ( *state ).textbuffer_offs += string_format ( ( *state ).textbuffer + ( *state ).textbuffer_offs
                                                            , "%u   "
                                                            , 8 - r
                                                            );
            }

            PIECE piece = EMPTY_SQ; 
            for ( PIECE piece_ = P; piece_ < 12; ++piece_ )
            {
                if ( bit ( ( *state ).board.pieces[ piece_ ]
                         , SQUAREINDX ( r , f )
                         ))
                {
                    piece = piece_;
                }
            }
            
            const bool blackchr = piece >= p && piece <= k;
            
            // Render Unicode or ASCII piece, depending on what the platform-
            // layer console supports.
            const char* s_piece;
            #if PLATFORM_WINDOWS == 1
                s_piece = ( blackchr ) ? piecewchr ( piece - p )
                                       : piecewchr ( piece )
                                       ;
            #else
                s_piece = ( blackchr ) ? piecewchr ( piece )
                                       : piecewchr ( p + piece )
                                       ;
            #endif
        
            ( *state ).textbuffer_offs += string_format ( ( *state ).textbuffer + ( *state ).textbuffer_offs
                                                        , CCE_COLOR_DEFAULT "%s %s " CCE_COLOR_DEFAULT
                                                        , even ( i ) ? ( blackchr ) ? ANSI_CC2 ( ANSI_CC_BG_RED , ANSI_CC_FG_BLACK ) 
                                                                                    : ANSI_CC2 ( ANSI_CC_BG_RED , ANSI_CC_FG_WHITE )
                                                                     : ( blackchr ) ? ANSI_CC2 ( ANSI_CC_BG_DARK_RED , ANSI_CC_FG_BLACK ) 
                                                                                    : ANSI_CC2 ( ANSI_CC_BG_DARK_RED , ANSI_CC_FG_WHITE )
                                                        , s_piece
                                                        );

            f += 1;
            i += 1;
        }

        f = 0;
        r += 1;
        i += 1;
    }
    
    ( *state ).textbuffer_offs += string_format ( ( *state ).textbuffer + ( *state ).textbuffer_offs
                                                , CCE_COLOR_INFO "\n\n\t\t      "
                                                );

    r = 0;
    while ( r < 8 )
    {
        ( *state ).textbuffer_offs += string_format ( ( *state ).textbuffer + ( *state ).textbuffer_offs
                                                    , " %c "
                                                    , 'A' + r
                                                    );
        r += 1;
    }

    ( *state ).textbuffer_offs += string_format ( ( *state ).textbuffer + ( *state ).textbuffer_offs
                                                , "\n\n\n\t\t  Side:         %s\n\t\t  En passant:      %s\n\t\t  Castling:      %c%c%c%c\n\n"
                                                , ( ( *state ).board.side == WHITE ) ? "white"
                                                                                     : "black"
                                                , ( ( *state ).board.enpassant != NO_SQ ) ? string_square ( ( *state ).board.enpassant )
                                                                                          : "no"
                                                , ( ( *state ).board.castle & CASTLE_WK ) ? 'K' : '-'
                                                , ( ( *state ).board.castle & CASTLE_WQ ) ? 'Q' : '-'
                                                , ( ( *state ).board.castle & CASTLE_BK ) ? 'k' : '-'
                                                , ( ( *state ).board.castle & CASTLE_BQ ) ? 'q' : '-'
                                                );
}

void
cce_render_move
( void )
{
    state_t* state = ( *cce ).internal;
    const char* s_side = ( ( *state ).game == CCE_GAME_PLAYER_VERSUS_ENGINE ) ? ( ( *state ).board.side != WHITE ) ? "WHITE (player)"
                                                                                                                   : "BLACK (engine)"
                                                                              : ( ( *state ).board.side != WHITE ) ? "WHITE"
                                                                                                                   : "BLACK"
                                                                                                                   ;
    if ( move_decode_capture ( ( *state ).move ) )
    {
        if ( move_decode_promotion ( ( *state ).move ) )
        {
            ( *state ).textbuffer_offs += string_format ( ( *state ).textbuffer + ( *state ).textbuffer_offs
                                                        , CCE_COLOR_PLUS "\n\n\n\t%s: %s ON %s CAPTURED %s ON %s. PROMOTED TO %s."
                                                        , s_side
                                                        , piecewchr ( move_decode_piece ( ( *state ).move ) )
                                                        , string_square ( move_decode_src ( ( *state ).move ) )
                                                        , piecewchr ( ( *state ).board.capture )
                                                        , string_square ( move_decode_dst ( ( *state ).move ) )
                                                        , piecewchr ( move_decode_promotion ( ( *state ).move ) )
                                                        );
        }
        else
        {
            ( *state ).textbuffer_offs += string_format ( ( *state ).textbuffer + ( *state ).textbuffer_offs
                                                        , CCE_COLOR_PLUS "\n\n\n\t%s: %s ON %s CAPTURED %s ON %s."
                                                        , s_side
                                                        , piecewchr ( move_decode_piece ( ( *state ).move ) )
                                                        , string_square ( move_decode_src ( ( *state ).move ) )
                                                        , piecewchr ( ( *state ).board.capture )
                                                        , string_square ( move_decode_dst ( ( *state ).move ) )
                                                        );
        }
    }
    else
    {
        if ( move_decode_promotion ( ( *state ).move ) )
        {
            ( *state ).textbuffer_offs += string_format ( ( *state ).textbuffer + ( *state ).textbuffer_offs
                                                        , CCE_COLOR_PLUS "\n\n\n\t%s: %s ON %s TO %s. PROMOTED TO %s."
                                                        , s_side
                                                        , piecewchr ( move_decode_piece ( ( *state ).move ) )
                                                        , string_square ( move_decode_src ( ( *state ).move ) )
                                                        , string_square ( move_decode_dst ( ( *state ).move ) )
                                                        , piecewchr ( move_decode_promotion ( ( *state ).move ) )
                                                        );
        }
        else
        {
            ( *state ).textbuffer_offs += string_format ( ( *state ).textbuffer + ( *state ).textbuffer_offs
                                                        , CCE_COLOR_PLUS "\n\n\n\t%s: %s ON %s TO %s."
                                                        , s_side
                                                        , piecewchr ( move_decode_piece ( ( *state ).move ) )
                                                        , string_square ( move_decode_src ( ( *state ).move ) )
                                                        , string_square ( move_decode_dst ( ( *state ).move ) )
                                                        );
        }
    }
}

void
cce_render_buffer
( void )
{
    state_t* state = ( *cce ).internal;
    ( *state ).textbuffer_offs += string_format ( ( *state ).textbuffer + ( *state ).textbuffer_offs
                                                , CCE_COLOR_DEFAULT "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
                                                );
}
