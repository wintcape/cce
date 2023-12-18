/**
 * @author Matthew Weissel (null@mattweissel.info)
 * @file linux.c
 * @brief Implementation of the platform header for GNU/Linux-based
 * operating systems.
 * (see platform.h for additional details)
 */
#include "platform/platform.h"

#include "core/event.h"
#include "core/logger.h"
#include "core/input.h"

#include "container/array.h"

// Begin platform layer.
#if PLATFORM_LINUX == 1

#include <xcb/xcb.h>        // requires X11, xcb
#include <X11/keysym.h>
#include <X11/XKBlib.h>
#include <X11/Xlib.h>
#include <X11/Xlib-xcb.h>

#include <sys/time.h>

#include <termios.h>
#include <unistd.h>

#if _POSIX_C_SOURCE >= 199309L
#include <time.h>   // nanosleep
#else
#include <unistd.h> // usleep
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Type definition for platform subsystem state.
typedef struct
{
    Display*            display;
    xcb_connection_t*   connection;
    xcb_window_t        window;
    xcb_screen_t*       screen;
    xcb_atom_t          wm_protocols;
    xcb_atom_t          wm_delete_win;

    bool                windowed;
}
state_t;

// Global subsystem state.
static state_t* state;

/**
 * @brief Parses an X button code.
 * @param x_code The X code to parse.
 * @return A BUTTON corresponding to the code value.
 */
BUTTON
platform_parse_button
(   const u32 x_code
);

/**
 * @brief Parses an X key code.
 * @param x_code The X code to parse.
 * @return A KEY corresponding to the code value.
 */
KEY
platform_parse_key
(   const u32 x_code
);

/**
 * @brief Primary implementation of platform_console_write and
 * platform_console_write_error.
 * @param mesg The message to write.
 * @param file The system file to write to.
 */
void
_platform_console_write
(   const char* mesg
,   FILE*       file
);

bool
platform_startup
(   u64*        memory_requirement
,   void*       state_
,   const bool  user_input
,   const bool  windowed
,   const char* wm_title
,   const char* wm_class
,   const i32   window_x
,   const i32   window_y
,   const i32   window_w
,   const i32   window_h
)
{
    *memory_requirement = sizeof ( state_t );

    if ( !state_ )
    {
        return true;
    }

    state = state_;
    platform_memory_clear ( state , sizeof ( state_t ) );

    ( *state ).windowed = windowed;

    // Windowless application.
    if ( !( *state ).windowed )
    {
        return true;
    }

    // Connect to X.
    ( *state ).display = XOpenDisplay ( NULL );

    // Disable key repeat.
   // XAutoRepeatOff ( ( *state ).display );

    // Retrieve the XCB connection.
    ( *state ).connection = XGetXCBConnection ( ( *state ).display );
    if ( xcb_connection_has_error ( ( *state ).connection ) )
    {
        LOGFATAL ( "platform_startup ("PLATFORM_STRING"): Failed to connect to X server via XCB." );
        return false;
    }

    // Initialize XCB.
    const struct xcb_setup_t* setup = xcb_get_setup ( ( *state ).connection );

    // Initialize XCB screen info.
    xcb_screen_iterator_t iterator = xcb_setup_roots_iterator ( setup );
    int screen_p = 0;
    for ( i32 i = screen_p ; i > 0 ; i-- )
    {
        xcb_screen_next ( &iterator );
    }
    ( *state ).screen = iterator.data;

    // Allocate an XID for the window to be created.
    ( *state ).window = xcb_generate_id ( ( *state ).connection );

    // Register event types.
    u32 event_mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
    
    // Register keyboard and mouse event listeners.
    u32 event_values = XCB_EVENT_MASK_BUTTON_PRESS
                     | XCB_EVENT_MASK_BUTTON_RELEASE
                     | XCB_EVENT_MASK_KEY_PRESS
                     | XCB_EVENT_MASK_KEY_RELEASE
                     | XCB_EVENT_MASK_EXPOSURE
                     | XCB_EVENT_MASK_POINTER_MOTION
                     | XCB_EVENT_MASK_STRUCTURE_NOTIFY
                     ;

    // Values to be sent over XCB.
    u32 value_list[] = { ( *( ( *state ).screen ) ).black_pixel // bg color
                       , event_values                           // listeners
                       };

    // Create the window.
    xcb_create_window ( ( *state ).connection
                      , XCB_COPY_FROM_PARENT
                      , ( *state ).window
                      , ( *( ( *state ).screen ) ).root
                      , window_x
                      , window_y
                      , window_w
                      , window_h
                      , 0
                      , XCB_WINDOW_CLASS_INPUT_OUTPUT
                      , ( *( ( *state ).screen ) ).root_visual
                      , event_mask
                      , value_list
                      );

    // Set the window properties (title, class, etc.).
    xcb_change_property ( ( *state ).connection
                        , XCB_PROP_MODE_REPLACE
                        , ( *state ).window
                        , XCB_ATOM_WM_NAME
                        , XCB_ATOM_STRING
                        , 8
                        , strlen ( wm_title )
                        , wm_title 
                        );
    xcb_change_property ( ( *state ).connection
                        , XCB_PROP_MODE_REPLACE
                        , ( *state ).window
                        , XCB_ATOM_WM_CLASS
                        , XCB_ATOM_STRING
                        , 8
                        , strlen ( wm_class )
                        , wm_class
                        );

    // Configure X to notify when the window manager attempts to delete the window.
    xcb_intern_atom_cookie_t wm_delete_cookie =
        xcb_intern_atom ( ( *state ).connection
                        , 0
                        , strlen ( "WM_DELETE_WINDOW" )
                        , "WM_DELETE_WINDOW"
                        );
    xcb_intern_atom_cookie_t wm_protocols_cookie =
        xcb_intern_atom ( ( *state ).connection
                        , 0
                        , strlen ( "WM_PROTOCOLS" )
                        , "WM_PROTOCOLS"
                        );
    xcb_intern_atom_reply_t* wm_delete_reply =
        xcb_intern_atom_reply ( ( *state ).connection
                              , wm_delete_cookie
                              , NULL
                              );
    xcb_intern_atom_reply_t* wm_protocols_reply =
        xcb_intern_atom_reply ( ( *state ).connection
                              , wm_protocols_cookie 
                              , NULL
                              );
    ( *state ).wm_delete_win = ( *wm_delete_reply ).atom;
    ( *state ).wm_protocols = ( *wm_protocols_reply ).atom;
    xcb_change_property ( ( *state ).connection
                        , XCB_PROP_MODE_REPLACE
                        , ( *state ).window
                        , ( *wm_protocols_reply ).atom
                        , 4
                        , 32
                        , 1
                        , &( *wm_delete_reply ).atom
                        );

    // Map the window to the screen.
    xcb_map_window ( ( *state ).connection , ( *state ).window );

    // Flush the stream.
    i32 stream_result = xcb_flush ( ( *state ).connection );
    if ( stream_result <= 0 )
    {
        LOGFATAL ( "platform_startup ("PLATFORM_STRING"): An error occurred when flushing the stream: %d"
                 , stream_result
                 );
        return false;
    }
    
    return true;
}

void
platform_shutdown
(   void* state_
)
{
    if ( !state )
    {
        return;
    }
   
    // Windowed application.
    if ( ( *state ).windowed )
    {
        // Turn on key repeats.
        // XAutoRepeatOn ( ( *state ).display );

        // Close the window.
        xcb_destroy_window ( ( *state ).connection , ( *state ).window );
    }

    state = 0;
}

bool
platform_pump_messages
( void )
{
    xcb_generic_event_t* event;
    xcb_client_message_event_t* cm;

    bool quit = false;

    // Event handling.
    for (;;)
    {
        // Poll until no event is returned.
        event = xcb_poll_for_event ( ( *state ).connection );
        if ( !event )
        {
            break;
        }
        
        // Input events.
        switch ( ( *event ).response_type & ~0x80 )
        {
            // Key press and release.
            case XCB_KEY_PRESS:
            case XCB_KEY_RELEASE:
            {
                xcb_key_press_event_t* event_ = ( xcb_key_press_event_t* ) event;
                xcb_keycode_t code = ( *event_ ).detail;
                KeySym keysym = XkbKeycodeToKeysym ( ( *state ).display
                                                   , ( KeyCode ) code
                                                   , 0
                                                   , code & ShiftMask ? 1
                                                                      : 0
                                                   );

                // Pass the key to the input subsystem for processing.
                KEY key = platform_parse_key ( keysym );
                if ( key != KEY_COUNT )
                {
                    input_process_key ( key
                                      , ( *event ).response_type == XCB_KEY_PRESS
                                      );
                }
            }
            break;

            // Mouse press and release.
            case XCB_BUTTON_PRESS:
            case XCB_BUTTON_RELEASE:
            {
                xcb_button_press_event_t* event_ = ( xcb_button_press_event_t* ) event;
                xcb_button_t code = ( *event_ ).detail;

                // Pass the button to the input subsystem for processing.
                BUTTON button = platform_parse_button ( code );
                if ( button != BUTTON_COUNT )
                {
                    input_process_button ( button
                                         , ( *event ).response_type == XCB_BUTTON_PRESS
                                         );
                }
            }
            break;

            // Mouse movement.
            case XCB_MOTION_NOTIFY:
            {
                xcb_motion_notify_event_t* event_ = ( xcb_motion_notify_event_t* ) event;

                // Pass the mouse position to the input subsystem for processing.
                input_process_mouse_move ( ( *event_ ).event_x
                                         , ( *event_ ).event_y
                                         );
            }
            break;

            // Window move or resize.
            case XCB_CONFIGURE_NOTIFY:
            {
                xcb_configure_notify_event_t* event_ = ( xcb_configure_notify_event_t* ) event;

                // Fire the event.
                event_context_t ctx;
                ctx.data.u16[ 0 ] = ( *event_ ).width;
                ctx.data.u16[ 1 ] = ( *event_ ).height;
                event_fire ( EVENT_CODE_RESIZE , 0 , ctx );
            }
            break;
            
            // Window close.
            case XCB_CLIENT_MESSAGE:
            {
                cm = ( xcb_client_message_event_t* ) event;
                if ( ( *cm ).data.data32[ 0 ] == ( *state ).wm_delete_win )
                {
                    quit = true;
                }
            }
            break;

            // Other.
            default:
            {}
            break;
        }
        
        free ( event );
    }

    return !quit;
}

void*
platform_memory_allocate
(   u64         size
,   const bool  aligned
)
{
    return malloc ( size );
}

void
platform_memory_free
(   void*       blk
,   const bool  aligned
)
{
    free ( blk );
}

void*
platform_memory_clear
(   void*   blk
,   u64     size
)
{
    return memset ( blk , 0 , size );
}

void*
platform_memory_set
(   void*       blk
,   const i32   value
,   u64         size
)
{
    return memset ( blk , value , size );
}

void*
platform_memory_copy
(   void*       dst
,   const void* src
,   u64         size
)
{
    return memcpy ( dst , src , size );
}

void*
platform_memory_move
(   void*       dst
,   const void* src
,   u64         size
)
{
    return memmove ( dst , src , size );
}

void
platform_console_write
(   const char* mesg
)
{
    _platform_console_write ( mesg , stdout );
}

void
platform_console_write_error
(   const char* mesg
)
{
    _platform_console_write ( mesg , stderr );
}

KEY
platform_console_read_key
( void )
{
    KEY key = KEY_COUNT:

    // Configure terminal for non-canonical input.
    struct termios tty;
    struct termios tty_;
    tcgetattr ( STDIN_FILENO , &tty );
    tty_ = tty;
    tty_.c_lflag &= ~( ICANON | ECHO );
    tcsetattr ( STDIN_FILENO , TCSANOW , &tty_ );
    fflush ( stdout );  // In case echo functionality desired.
    
    // Read the key from the input stream.
    // May be up to four bytes to handle special keys.
    char in[ 4 ];
    i8 written;
    while ( ( written = read ( STDIN_FILENO , in , sizeof ( in ) ) ) > 0 )
    {
        for ( u8 i = 0; i < written; ++i )
        {
            // End of transmission.
            if ( in[ i ] == 4 )
            {
                key = KEY_COUNT;
                goto platform_console_read_key_end;
            }

            // ANSI escape sequence.
            if ( in[ i ] == '\033' )
            {
                if ( written > i + 1 )
                {
                    key = 0;    // To be implemented.
                    goto platform_console_read_key_end;
                }
                else
                {
                    fd_set set;
                    struct timeval timeout;
                    FD_ZERO ( &set );
                    FD_SET ( STDIN_FILENO , &set );
                    timeout.tv_sec = 0;
                    timeout.tv_usec = 0;

                    const int result = select ( 1 , &set , 0 , 0 , &timeout );
                    if ( result == -1 )
                    {
                        key = KEY_COUNT;
                        goto platform_console_read_key_end;
                    }
                    
                    // <Esc> key.
                    if ( result != 1 )
                    {
                        key = KEY_ESCAPE;
                        goto platform_console_read_key_end;
                    }

                    else
                    {
                        key = 0;    // To be implemented.
                        goto platform_console_read_key_end;
                    }
                }
            }

            // Otherwise, just take the first byte as an ASCII value.
            else
            {
                // Backspace key is mapped to ASCII DELETE (for some reason).
                key = ( *in == KEY_DELETE ) ? KEY_BACKSPACE : *in;
                goto platform_console_read_key_end;
            }
        }
    }

    platform_console_read_key_end:
        tcsetattr ( STDIN_FILENO , TCSANOW , &tty );
        fflush ( stdout );  // In case echo functionality desired.
        return key;
}

f64
platform_get_absolute_time
( void )
{
    struct timespec time;
    clock_gettime ( CLOCK_MONOTONIC , &time );
    return time.tv_sec + time.tv_nsec * 0.000000001;
}

void
platform_sleep
(   u64 ms
)
{
#if _POSIX_C_SOURCE >= 199309L
    struct timespec time;
    time.tv_sec = ms / 1000;
    time.tv_nsec = ( ms % 1000 ) * 1000 * 1000;
    nanosleep ( &time , 0 );
#else
    if ( ms >= 1000 )
    {
        sleep ( ms / 1000 );
    }
    usleep ( ( ms % 1000 ) * 1000 );
#endif
}

void
_platform_console_write
(   const char* mesg
,   FILE*       file
)
{
    fprintf ( file , "%s" ANSI_CC_RESET , mesg );
}

BUTTON
platform_parse_button
(   const u32 x_code
)
{
    switch ( x_code )
    {
        case XCB_BUTTON_INDEX_1:
            return BUTTON_LEFT;
        case XCB_BUTTON_INDEX_2:
            return BUTTON_CENTER;
        case XCB_BUTTON_INDEX_3:
            return BUTTON_RIGHT;
        
        default:
            return BUTTON_COUNT;
    }
}

KEY
platform_parse_key
(   const u32 x_code
)
{
    switch ( x_code )
    {
        case XK_BackSpace:
            return KEY_BACKSPACE;
        case XK_Return:
            return KEY_ENTER;
        case XK_Tab:
            return KEY_TAB;
        case XK_Pause:
            return KEY_PAUSE;
        case XK_Caps_Lock:
            return KEY_CAPITAL;
        case XK_Escape:
            return KEY_ESCAPE;
        // case :
        //     return KEY_CONVERT;      // Not supported.
        // case :
        //     return KEY_NONCONVERT;   // Not supported.
        // case :
        //     return KEY_ACCEPT;       // Not supported.
        case XK_Mode_switch:
            return KEY_MODECHANGE;
        case XK_space:
            return KEY_SPACE;
        case XK_Prior:
            return KEY_PRIOR;
        case XK_Next:
            return KEY_NEXT;
        case XK_End:
            return KEY_END;
        case XK_Home:
            return KEY_HOME;
        case XK_Left:
            return KEY_LEFT;
        case XK_Up:
            return KEY_UP;
        case XK_Right:
            return KEY_RIGHT;
        case XK_Down:
            return KEY_DOWN;
        case XK_Select:
            return KEY_SELECT;
        case XK_Print:
            return KEY_PRINT;
        case XK_Execute:
            return KEY_EXECUTE;
        // case :
        //     return KEY_SNAPSHOT; // Not supported.
        case XK_Insert:
            return KEY_INSERT;
        case XK_Delete:
            return KEY_DELETE;
        case XK_Help:
            return KEY_HELP;
        case XK_Meta_L:
            return KEY_LWIN;
        case XK_Meta_R:
            return KEY_RWIN;
        // case :
        //     return KEY_APPS;     // Not supported.
        // case :
        //     return KEY_SLEEP;    // Not supported.
        case XK_KP_0:
            return KEY_NUMPAD0;
        case XK_KP_1:
            return KEY_NUMPAD1;
        case XK_KP_2:
            return KEY_NUMPAD2;
        case XK_KP_3:
            return KEY_NUMPAD3;
        case XK_KP_4:
            return KEY_NUMPAD4;
        case XK_KP_5:
            return KEY_NUMPAD5;
        case XK_KP_6:
            return KEY_NUMPAD6;
        case XK_KP_7:
            return KEY_NUMPAD7;
        case XK_KP_8:
            return KEY_NUMPAD8;
        case XK_KP_9:
            return KEY_NUMPAD9;
        case XK_multiply:
            return KEY_MULTIPLY;
        case XK_KP_Add:
            return KEY_ADD;
        case XK_KP_Separator:
            return KEY_SEPARATOR;
        case XK_KP_Subtract:
            return KEY_SUBTRACT;
        case XK_KP_Decimal:
            return KEY_DECIMAL;
        case XK_KP_Divide:
            return KEY_DIVIDE;
        case XK_F1:
            return KEY_F1;
        case XK_F2:
            return KEY_F2;
        case XK_F3:
            return KEY_F3;
        case XK_F4:
            return KEY_F4;
        case XK_F5:
            return KEY_F5;
        case XK_F6:
            return KEY_F6;
        case XK_F7:
            return KEY_F7;
        case XK_F8:
            return KEY_F8;
        case XK_F9:
            return KEY_F9;
        case XK_F10:
            return KEY_F10;
        case XK_F11:
            return KEY_F11;
        case XK_F12:
            return KEY_F12;
        case XK_F13:
            return KEY_F13;
        case XK_F14:
            return KEY_F14;
        case XK_F15:
            return KEY_F15;
        case XK_F16:
            return KEY_F16;
        case XK_F17:
            return KEY_F17;
        case XK_F18:
            return KEY_F18;
        case XK_F19:
            return KEY_F19;
        case XK_F20:
            return KEY_F20;
        case XK_F21:
            return KEY_F21;
        case XK_F22:
            return KEY_F22;
        case XK_F23:
            return KEY_F23;
        case XK_F24:
            return KEY_F24;
        case XK_Num_Lock:
            return KEY_NUMLOCK;
        case XK_Scroll_Lock:
            return KEY_SCROLL;
        case XK_KP_Equal:
            return KEY_NUMPAD_EQUAL;
        case XK_Shift_L:
            return KEY_LSHIFT;
        case XK_Shift_R:
            return KEY_RSHIFT;
        case XK_Control_L:
            return KEY_LCTRL;
        case XK_Control_R:
            return KEY_RCTRL;
         case XK_Alt_L:
            return KEY_LALT;
        case XK_Alt_R:
            return KEY_RALT;
        case XK_semicolon:
            return KEY_SEMICOLON;
        case XK_plus:
            return KEY_PLUS;
        case XK_comma:
            return KEY_COMMA;
        case XK_minus:
            return KEY_MINUS;
        case XK_period:
            return KEY_PERIOD;
        case XK_slash:
            return KEY_SLASH;
        case XK_grave:
            return KEY_GRAVE;
        case XK_0:
            return KEY_0;
        case XK_1:
            return KEY_1;
        case XK_2:
            return KEY_2;
        case XK_3:
            return KEY_3;
        case XK_4:
            return KEY_4;
        case XK_5:
            return KEY_5;
        case XK_6:
            return KEY_6;
        case XK_7:
            return KEY_7;
        case XK_8:
            return KEY_8;
        case XK_9:
            return KEY_9;
        case XK_a:
        case XK_A:
            return KEY_A;
        case XK_b:
        case XK_B:
            return KEY_B;
        case XK_c:
        case XK_C:
            return KEY_C;
        case XK_d:
        case XK_D:
            return KEY_D;
        case XK_e:
        case XK_E:
            return KEY_E;
        case XK_f:
        case XK_F:
            return KEY_F;
        case XK_g:
        case XK_G:
            return KEY_G;
        case XK_h:
        case XK_H:
            return KEY_H;
        case XK_i:
        case XK_I:
            return KEY_I;
        case XK_j:
        case XK_J:
            return KEY_J;
        case XK_k:
        case XK_K:
            return KEY_K;
        case XK_l:
        case XK_L:
            return KEY_L;
        case XK_m:
        case XK_M:
            return KEY_M;
        case XK_n:
        case XK_N:
            return KEY_N;
        case XK_o:
        case XK_O:
            return KEY_O;
        case XK_p:
        case XK_P:
            return KEY_P;
        case XK_q:
        case XK_Q:
            return KEY_Q;
        case XK_r:
        case XK_R:
            return KEY_R;
        case XK_s:
        case XK_S:
            return KEY_S;
        case XK_t:
        case XK_T:
            return KEY_T;
        case XK_u:
        case XK_U:
            return KEY_U;
        case XK_v:
        case XK_V:
            return KEY_V;
        case XK_w:
        case XK_W:
            return KEY_W;
        case XK_x:
        case XK_X:
            return KEY_X;
        case XK_y:
        case XK_Y:
            return KEY_Y;
        case XK_z:
        case XK_Z:
            return KEY_Z;

        default:
            return KEY_COUNT;
    }
}

#endif  // End platform layer.
