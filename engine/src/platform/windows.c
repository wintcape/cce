/**
 * @author Matthew Weissel (null@mattweissel.info)
 * @file windows.c
 * @brief Implementation of the platform header for Microsoft Windows
 * operating systems.
 * (see platform.h for additional details)
 */
#include "platform/platform.h"

#include "core/event.h"
#include "core/logger.h"
#include "core/input.h"

#include "container/array.h"

// Begin platform layer.
#if PLATFORM_WINDOWS == 1

#include <windows.h>
#include <windowsx.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Global system clock. Allows for clocks to function without having to call
// platform_start first (see core/clock.h).
static f64              platform_clock_frequency;
static LARGE_INTEGER    platform_clock_start_time;

// Type definition for platform subsystem state.
typedef struct
{
    HINSTANCE   h_instance;
    HWND        hwnd;

    bool        windowed;
}
state_t;

// Global subsystem state.
static state_t* state;

/**
 * @brief Process message callback function.
 */
LRESULT CALLBACK
platform_process_message
(   HWND    hwnd
,   u32     mesg
,   WPARAM  w_param
,   LPARAM  l_param
);

/**
 * @brief Initializes the system clock.
 */
void
platform_clock_init
( void );

/**
 * @brief Primary implementation of platform_console_write and
 * platform_console_write_error.
 * @param mesg The message to write.
 * @param file The file to write to.
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
,   const i32   window_x_
,   const i32   window_y_
,   const i32   window_w_
,   const i32   window_h_
)
{
    *memory_requirement = sizeof ( state_t );

    if ( !state_ )
    {
        return true;
    }

    state = state_;
    platform_memory_clear ( state , sizeof ( state_t ) );
    
    ( *state ).h_instance = GetModuleHandleA ( 0 );
    ( *state ).windowed = windowed;
    
    // Initialize clock.
    platform_clock_init();

    // Windowless application.
    if ( !( *state ).windowed )
    {
        return true;
    }
    
    // Register window.
    WNDCLASSA wc;
    platform_memory_clear ( &wc , sizeof ( wc ) );
    wc.style = CS_DBLCLKS;
    wc.lpfnWndProc = platform_process_message;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = ( *state ).h_instance;
    wc.hIcon = LoadIcon ( ( *state ).h_instance , IDI_APPLICATION );
    wc.hCursor = LoadCursor ( NULL , IDC_ARROW );
    wc.hbrBackground = NULL;
    wc.lpszClassName = wm_class;
    if ( !RegisterClassA ( &wc ) )
    {
        MessageBoxA ( 0
                    , "Window registration failed."
                    , "ERROR"
                    , MB_ICONEXCLAMATION | MB_OK
                    );
        LOGFATAL ( "platform_startup ("PLATFORM_STRING"): Failed to register the application window." );
        return false;
    }

    // Configure window dimensions.
    u32 window_x = window_x_;
    u32 window_y = window_y_;
    u32 window_w = window_w_;
    u32 window_h = window_h_;
    
    // Configure window style.
    u32 window_style = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION;
    window_style |= WS_MAXIMIZEBOX;
    window_style |= WS_MINIMIZEBOX;
    window_style |= WS_THICKFRAME;
    u32 window_ex_style = WS_EX_APPWINDOW;
    
    // Configure window border.
    RECT border = { 0, 0, 0, 0 };
    AdjustWindowRectEx ( &border
                       , window_style
                       , 0
                       , window_ex_style
                       );
    window_x += border.left;
    window_y += border.top;
    window_w += border.right - border.left;
    window_h += border.bottom - border.top;

    // Create window.
    HWND window = CreateWindowExA ( window_ex_style
                                  , wm_class
                                  , wm_title
                                  , window_style
                                  , window_x
                                  , window_y
                                  , window_w
                                  , window_h
                                  , 0
                                  , 0
                                  , ( *state ).h_instance
                                  , 0
                                  );
    if ( !window )
    {
        MessageBoxA ( NULL
                    , "Window creation failed."
                    , "ERROR"
                    , MB_ICONEXCLAMATION | MB_OK
                    );
        LOGFATAL ( "platform_startup ("PLATFORM_STRING"): Failed to create the application window." );
        return false;
    }
    ( *state ).hwnd = window;

    // Show the window.
    i32 show_flags = user_input ?
    /*  Default  */               SW_SHOW : SW_SHOWNOACTIVATE
    /*  Start minimized  */    // SW_MINIMIZE : SW_SHOWMINNOACTIVE
    /*  Start maximized  */    // SW_SHOWMAXIMIZED : SW_MAXIMIZE
                                ;

    ShowWindow ( ( *state ).hwnd
               , show_flags
               );

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

    if ( ( *state ).windowed )
    {
        if ( ( *state ).hwnd )
        {
            DestroyWindow ( ( *state ).hwnd );
            ( *state ).hwnd = 0;
        }
    }

    state = 0;
}

bool
platform_pump_messages
( void )
{
    MSG msg;
    while ( PeekMessageA ( &msg , NULL , 0 , 0 , PM_REMOVE ) )
    {
        TranslateMessage ( &msg );
        DispatchMessageA ( &msg );
    }
    return true;
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

f64
platform_get_absolute_time
( void )
{    
    if ( !platform_clock_frequency )
    {
        platform_clock_init ();
    }
    
    LARGE_INTEGER time;
    QueryPerformanceCounter ( &time );
    return ( (f64) time.QuadPart ) * platform_clock_frequency;
}

void
platform_sleep
(   u64 ms
)
{
    Sleep ( ms );
}

void
_platform_console_write
(   const char* mesg
,   FILE*       file
)
{
    fprintf ( file , "%s" ANSI_CC_RESET , mesg );
}

void
platform_clock_init
( void )
{
    LARGE_INTEGER f;
    QueryPerformanceFrequency( &f );
    platform_clock_frequency = 1.0 / ( (f64) f.QuadPart );
    QueryPerformanceCounter( &platform_clock_start_time );
}


LRESULT CALLBACK
platform_process_message
(   HWND    hwnd
,   u32     mesg
,   WPARAM  w_param
,   LPARAM  l_param
)
{
    // Event handling.
    switch ( mesg )
    {
        case WM_ERASEBKGND:
        {
            return true;
        }

        case WM_CLOSE:
        {
            event_context_t ctx = {};
            event_fire ( EVENT_CODE_APPLICATION_QUIT , 0 , ctx );
            return false;
        }

        case WM_DESTROY:
        {
            PostQuitMessage ( 0 );
            return false;
        }

        case WM_SIZE:
        {
            RECT r;
            GetClientRect ( hwnd, &r );
            const u32 w = r.right - r.left;
            const u32 h = r.bottom - r.top;

            event_context_t ctx;
            ctx.data.u16[ 0 ] = (u16) w;
            ctx.data.u16[ 1 ] = (u16) h;
            event_fire ( EVENT_CODE_RESIZE , 0 , ctx );
        }
        break;

        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYUP:
        {
            const bool pressed = mesg == WM_KEYDOWN || mesg == WM_SYSKEYDOWN;
            const bool extended = ( HIWORD ( l_param ) & KF_EXTENDED ) == KF_EXTENDED;
            
            KEY key = (u16) w_param;
            if ( w_param == VK_MENU )
            {
                key = extended ? KEY_RALT : KEY_LALT;
            }
            else if ( w_param == VK_SHIFT )
            {
                const u32 left = MapVirtualKey ( VK_LSHIFT , MAPVK_VK_TO_VSC );
                const u32 scancode = ( l_param & ( 0xFF << 16 ) ) >> 16;
                key = ( scancode == left )  ? KEY_LSHIFT : KEY_RSHIFT;
            } else if ( w_param == VK_CONTROL )
            {
                key = extended ? KEY_RCTRL : KEY_LCTRL;
            }

            input_process_key ( key , pressed );

            return false;
        }

        case WM_MOUSEMOVE:
        {
            const i32 x = GET_X_LPARAM ( l_param );
            const i32 y = GET_Y_LPARAM ( l_param );
            input_process_mouse_move ( x , y );
        }
        break;

        case WM_MOUSEWHEEL:
        {
            i32 dz = GET_WHEEL_DELTA_WPARAM ( w_param );
            if ( dz )
            {
                dz = ( dz < 0 ) ? -1 : 1;
                input_process_mouse_wheel ( dz );
            }
        }
        break;

        case WM_LBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_MBUTTONUP:
        case WM_RBUTTONUP:
        {
            const bool pressed = mesg == WM_LBUTTONDOWN
                              || mesg == WM_RBUTTONDOWN
                              || mesg == WM_MBUTTONDOWN
                              ;
            BUTTON button = BUTTON_COUNT;

            switch ( mesg )
            {
                case WM_LBUTTONDOWN:
                case WM_LBUTTONUP:
                {
                    button = BUTTON_LEFT;
                }
                break;

                case WM_MBUTTONDOWN:
                case WM_MBUTTONUP:
                {
                    button = BUTTON_CENTER;
                }
                break;

                case WM_RBUTTONDOWN:
                case WM_RBUTTONUP:
                {
                    button = BUTTON_RIGHT;
                }
                break;
            }

            if ( button != BUTTON_COUNT )
            {
                input_process_button ( button , pressed );
            }
        }
        break;
    }// END switch.

    return DefWindowProcA ( hwnd , mesg , w_param , l_param );
}

#endif  // End platform layer.
