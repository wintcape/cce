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
 * @brief Parses a Windows button code.
 * @param code The Windows button code to parse.
 * @return A BUTTON corresponding to the code value.
 */
BUTTON
platform_parse_button
(   const u32 code
);

/**
 * @brief Parses a Windows virtual key code.
 * @param vk_code_l The virtual key code to parse (1).
 * @param vk_code_w The virtual key code to parse (2).
 * @return A KEY corresponding to the code value.
 */
KEY
platform_parse_key
(   const u16 vk_code_l
,   const u16 vk_code_w
);

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

KEY
platform_console_read_key
( void )
{ return 0; }

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
            KEY key = platform_parse_key ( w_param , l_param );
            if ( key != KEY_COUNT )
            {
                input_process_key ( key , pressed );
            }
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
            BUTTON button = platform_parse_button ( mesg );
            if ( button != BUTTON_COUNT )
            {
                input_process_button ( button , pressed );
            }
        }
        break;
    }// END switch.

    return DefWindowProcA ( hwnd , mesg , w_param , l_param );
}

BUTTON
platform_parse_button
(   const u32 code
)
{
    switch ( code )
    {
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
            return BUTTON_LEFT;
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
            return BUTTON_CENTER;
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
            return BUTTON_RIGHT;
        
        default:
            return BUTTON_COUNT;
    }
}

KEY
platform_parse_key
(   const u16 vk_code_w
,   const u16 vk_code_l
)
{
    const bool extended = ( HIWORD ( l_param ) & KF_EXTENDED ) == KF_EXTENDED;
    if ( vk_code_w == VK_MENU )
    {
        return ( extended ) ? KEY_RALT : KEY_LALT;
    }
    else if ( vk_code_w == VK_SHIFT )
    {
        const u32 left = MapVirtualKey ( VK_LSHIFT , MAPVK_VK_TO_VSC );
        const u32 scancode = ( vk_code_l & ( 0xFF << 16 ) ) >> 16;
        return ( scancode == left )  ? KEY_LSHIFT : KEY_RSHIFT;
    }
    else if ( vk_code_w == VK_CONTROL )
    {
        return ( extended ) ? KEY_RCTRL : KEY_LCTRL;
    }

    switch ( vk_code_w )
    {
        case VK_BACK:
            return KEY_BACKSPACE;
        case VK_RETURN:
            return KEY_ENTER;
        case VK_TAB:
            return KEY_TAB;
        case VK_PAUSE:
            return KEY_PAUSE;
        case VK_CAPITAL:
            return KEY_CAPITAL;
        case VK_ESCAPE:
            return KEY_ESCAPE;
        case VK_CONVERT:
            return KEY_CONVERT;
        case VK_NONCONVERT:
            return KEY_NONCONVERT;
        case VK_ACCEPT:
            return KEY_ACCEPT;
        case VK_MODECHANGE:
            return KEY_MODECHANGE;
        case VK_SPACE:
            return KEY_SPACE;
        case VK_PRIOR:
            return KEY_PRIOR;
        case VK_NEXT:
            return KEY_NEXT;
        case VK_END:
            return KEY_END;
        case VK_HOME:
            return KEY_HOME;
        case VK_LEFT:
            return KEY_LEFT;
        case VK_UP:
            return KEY_UP;
        case VK_RIGHT:
            return KEY_RIGHT;
        case VK_DOWN:
            return KEY_DOWN;
        case VK_SELECT:
            return KEY_SELECT;
        case VK_PRINT:
            return KEY_PRINT;
        case VK_EXECUTE:
            return KEY_EXECUTE;
        case VK_SNAPSHOT:
             return KEY_SNAPSHOT;
        case VK_INSERT:
            return KEY_INSERT;
        case VK_DELETE:
            return KEY_DELETE;
        case VK_HELP:
            return KEY_HELP;
        case VK_LWIN:
            return KEY_LWIN;
        case VK_RWIN:
            return KEY_RWIN;
        case VK_APPS:
            return KEY_APPS;
        case VK_SLEEP:
            return KEY_SLEEP;
        case VK_NUMPAD0:
            return KEY_NUMPAD0;
        case VK_NUMPAD1:
            return KEY_NUMPAD1;
        case VK_NUMPAD2:
            return KEY_NUMPAD2;
        case VK_NUMPAD3:
            return KEY_NUMPAD3;
        case VK_NUMPAD4:
            return KEY_NUMPAD4;
        case VK_NUMPAD5:
            return KEY_NUMPAD5;
        case VK_NUMPAD6:
            return KEY_NUMPAD6;
        case VK_NUMPAD7:
            return KEY_NUMPAD7;
        case VK_NUMPAD8:
            return KEY_NUMPAD8;
        case VK_NUMPAD9:
            return KEY_NUMPAD9;
        case VK_MULTIPLY:
            return KEY_MULTIPLY;
        case VK_ADD:
            return KEY_ADD;
        case VK_SEPARATOR:
            return KEY_SEPARATOR;
        case VK_SUBTRACT:
            return KEY_SUBTRACT;
        case VK_DECIMAL:
            return KEY_DECIMAL;
        case VK_DIVIDE:
            return KEY_DIVIDE;
        case VK_F1:
            return KEY_F1;
        case VK_F2:
            return KEY_F2;
        case VK_F3:
            return KEY_F3;
        case VK_F4:
            return KEY_F4;
        case VK_F5:
            return KEY_F5;
        case VK_F6:
            return KEY_F6;
        case VK_F7:
            return KEY_F7;
        case VK_F8:
            return KEY_F8;
        case VK_F9:
            return KEY_F9;
        case VK_F10:
            return KEY_F10;
        case VK_F11:
            return KEY_F11;
        case VK_F12:
            return KEY_F12;
        case VK_F13:
            return KEY_F13;
        case VK_F14:
            return KEY_F14;
        case VK_F15:
            return KEY_F15;
        case VK_F16:
            return KEY_F16;
        case VK_F17:
            return KEY_F17;
        case VK_F18:
            return KEY_F18;
        case VK_F19:
            return KEY_F19;
        case VK_F20:
            return KEY_F20;
        case VK_F21:
            return KEY_F21;
        case VK_F22:
            return KEY_F22;
        case VK_F23:
            return KEY_F23;
        case VK_F24:
            return KEY_F24;
        case VK_NUMLOCK:
            return KEY_NUMLOCK;
        case VK_SCROLL:
            return KEY_SCROLL;
        // case :
        //    return KEY_NUMPAD_EQUAL;  // Not supported.
        // case :
        //     return KEY_SEMICOLON;    // Not supported.
        // case :
        //     return KEY_PLUS;         // Not supported.
        // case :
        //     return KEY_COMMA;        // Not supported.
        // case :
        //     return KEY_MINUS;        // Not supported.
        // case :
        //     return KEY_PERIOD;       // Not supported.
        // case :
        //     return KEY_SLASH;        // Not supported.
        // case :
        //     return KEY_GRAVE;        // Not supported.
        case 0x30:
            return KEY_0;
        case 0x31:
            return KEY_1;
        case 0x32:
            return KEY_2;
        case 0x33:
            return KEY_3;
        case 0x34:
            return KEY_4;
        case 0x35:
            return KEY_5;
        case 0x36:
            return KEY_6;
        case 0x37:
            return KEY_7;
        case 0x38:
            return KEY_8;
        case 0x39:
            return KEY_9;
        case 0x41:
            return KEY_A;
        case 0x42:
            return KEY_B;
        case 0x43:
            return KEY_C;
        case 0x44:
            return KEY_D;
        case 0x45:
            return KEY_E;
        case 0x46:
            return KEY_F;
        case 0x47:
            return KEY_G;
        case 0x48:
            return KEY_H;
        case 0x49:
            return KEY_I;
        case 0x4A:
            return KEY_J;
        case 0x4B:
            return KEY_K;
        case 0x4C:
            return KEY_L;
        case 0x4D:
            return KEY_M;
        case 0x4E:
            return KEY_N;
        case 0x4F:
            return KEY_O;
        case 0x50:
            return KEY_P;
        case 0x51:
            return KEY_Q;
        case 0x52:
            return KEY_R;
        case 0x53:
            return KEY_S;
        case 0x54:
            return KEY_T;
        case 0x55:
            return KEY_U;
        case 0x56:
            return KEY_V;
        case 0x57:
            return KEY_W;
        case 0x58:
            return KEY_X;
        case 0x59:
            return KEY_Y;
        case 0x5A:
            return KEY_Z;

        default:
            return KEY_COUNT;
    }
}

#endif  // End platform layer.
