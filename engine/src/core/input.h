/**
 * @file input.h
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Engine subsystem for user input.
 */
#ifndef INPUT_H
#define INPUT_H

#include "common.h"

// Type definition for button input.
typedef enum
{
    BUTTON_LEFT
,   BUTTON_RIGHT
,   BUTTON_CENTER

,   BUTTON_COUNT
}
BUTTON;

// Type definition for keyboard input (extension of ASCII charset).
#define DEFINE_KEY(name,code)   KEY_##name = code
typedef enum
{
    DEFINE_KEY ( BACKSPACE      , 0x008 )
,   DEFINE_KEY ( TAB            , 0x009 )
,   DEFINE_KEY ( ENTER          , 0x010 )
,   DEFINE_KEY ( SPACE          , 0x032 )
,   DEFINE_KEY ( DELETE         , 0x07F )

,   DEFINE_KEY ( SHIFT          , 0x100 )
,   DEFINE_KEY ( CONTROL        , 0x101 )
,   DEFINE_KEY ( PAUSE          , 0x103 )
,   DEFINE_KEY ( CAPITAL        , 0x104 )
 
,   DEFINE_KEY ( ESCAPE         , 0x11B )
 
,   DEFINE_KEY ( CONVERT        , 0x11C )
,   DEFINE_KEY ( NONCONVERT     , 0x11D )
,   DEFINE_KEY ( ACCEPT         , 0x11E )
,   DEFINE_KEY ( MODECHANGE     , 0x11F )
 
,   DEFINE_KEY ( PRIOR          , 0x121 )
,   DEFINE_KEY ( NEXT           , 0x122 )
,   DEFINE_KEY ( END            , 0x123 )
,   DEFINE_KEY ( HOME           , 0x124 )
,   DEFINE_KEY ( LEFT           , 0x125 )
,   DEFINE_KEY ( UP             , 0x126 )
,   DEFINE_KEY ( RIGHT          , 0x127 )
,   DEFINE_KEY ( DOWN           , 0x128 )
,   DEFINE_KEY ( SELECT         , 0x129 )
,   DEFINE_KEY ( PRINT          , 0x12A )
,   DEFINE_KEY ( EXECUTE        , 0x12B )
,   DEFINE_KEY ( SNAPSHOT       , 0x12C )
,   DEFINE_KEY ( INSERT         , 0x12D )
,   DEFINE_KEY ( HELP           , 0x12F )

,   DEFINE_KEY ( 0              , 0x130 )
,   DEFINE_KEY ( 1              , 0x131 )
,   DEFINE_KEY ( 2              , 0x132 )
,   DEFINE_KEY ( 3              , 0x133 )
,   DEFINE_KEY ( 4              , 0x134 )
,   DEFINE_KEY ( 5              , 0x135 )
,   DEFINE_KEY ( 6              , 0x136 )
,   DEFINE_KEY ( 7              , 0x137 )
,   DEFINE_KEY ( 8              , 0x138 )
,   DEFINE_KEY ( 9              , 0x139 )
 
,   DEFINE_KEY ( A              , 0x141 )
,   DEFINE_KEY ( B              , 0x142 )
,   DEFINE_KEY ( C              , 0x143 )
,   DEFINE_KEY ( D              , 0x144 )
,   DEFINE_KEY ( E              , 0x145 )
,   DEFINE_KEY ( F              , 0x146 )
,   DEFINE_KEY ( G              , 0x147 )
,   DEFINE_KEY ( H              , 0x148 )
,   DEFINE_KEY ( I              , 0x149 )
,   DEFINE_KEY ( J              , 0x14A )
,   DEFINE_KEY ( K              , 0x14B )
,   DEFINE_KEY ( L              , 0x14C )
,   DEFINE_KEY ( M              , 0x14D )
,   DEFINE_KEY ( N              , 0x14E )
,   DEFINE_KEY ( O              , 0x14F )
,   DEFINE_KEY ( P              , 0x150 )
,   DEFINE_KEY ( Q              , 0x151 )
,   DEFINE_KEY ( R              , 0x152 )
,   DEFINE_KEY ( S              , 0x153 )
,   DEFINE_KEY ( T              , 0x154 )
,   DEFINE_KEY ( U              , 0x155 )
,   DEFINE_KEY ( V              , 0x156 )
,   DEFINE_KEY ( W              , 0x157 )
,   DEFINE_KEY ( X              , 0x158 )
,   DEFINE_KEY ( Y              , 0x159 )
,   DEFINE_KEY ( Z              , 0x15A )
 
,   DEFINE_KEY ( LWIN           , 0x15B )
,   DEFINE_KEY ( RWIN           , 0x15C )
,   DEFINE_KEY ( APPS           , 0x15D )
 
,   DEFINE_KEY ( SLEEP          , 0x15F )
 
,   DEFINE_KEY ( NUMPAD0        , 0x160 )
,   DEFINE_KEY ( NUMPAD1        , 0x161 )
,   DEFINE_KEY ( NUMPAD2        , 0x162 )
,   DEFINE_KEY ( NUMPAD3        , 0x163 )
,   DEFINE_KEY ( NUMPAD4        , 0x164 )
,   DEFINE_KEY ( NUMPAD5        , 0x165 )
,   DEFINE_KEY ( NUMPAD6        , 0x166 )
,   DEFINE_KEY ( NUMPAD7        , 0x167 )
,   DEFINE_KEY ( NUMPAD8        , 0x168 )
,   DEFINE_KEY ( NUMPAD9        , 0x169 )

,   DEFINE_KEY ( MULTIPLY       , 0x16A )
,   DEFINE_KEY ( ADD            , 0x16B )
,   DEFINE_KEY ( SEPARATOR      , 0x16C )
,   DEFINE_KEY ( SUBTRACT       , 0x16D )
,   DEFINE_KEY ( DECIMAL        , 0x16E )
,   DEFINE_KEY ( DIVIDE         , 0x16F )

,   DEFINE_KEY ( F1             , 0x170 )
,   DEFINE_KEY ( F2             , 0x171 )
,   DEFINE_KEY ( F3             , 0x172 )
,   DEFINE_KEY ( F4             , 0x173 )
,   DEFINE_KEY ( F5             , 0x174 )
,   DEFINE_KEY ( F6             , 0x175 )
,   DEFINE_KEY ( F7             , 0x176 )
,   DEFINE_KEY ( F8             , 0x177 )
,   DEFINE_KEY ( F9             , 0x178 )
,   DEFINE_KEY ( F10            , 0x179 )
,   DEFINE_KEY ( F11            , 0x17A )
,   DEFINE_KEY ( F12            , 0x17B )
,   DEFINE_KEY ( F13            , 0x17C )
,   DEFINE_KEY ( F14            , 0x17D )
,   DEFINE_KEY ( F15            , 0x17E )
,   DEFINE_KEY ( F16            , 0x17F )
,   DEFINE_KEY ( F17            , 0x180 )
,   DEFINE_KEY ( F18            , 0x181 )
,   DEFINE_KEY ( F19            , 0x182 )
,   DEFINE_KEY ( F20            , 0x183 )
,   DEFINE_KEY ( F21            , 0x184 )
,   DEFINE_KEY ( F22            , 0x185 )
,   DEFINE_KEY ( F23            , 0x186 )
,   DEFINE_KEY ( F24            , 0x187 )
 
,   DEFINE_KEY ( NUMLOCK        , 0x190 )
,   DEFINE_KEY ( SCROLL         , 0x191 )
 
,   DEFINE_KEY ( NUMPAD_EQUAL   , 0x192 )
 
,   DEFINE_KEY ( LSHIFT         , 0x1A0 )
,   DEFINE_KEY ( RSHIFT         , 0x1A1 )
,   DEFINE_KEY ( LCTRL          , 0x1A2 )
,   DEFINE_KEY ( RCTRL          , 0x1A3 )
,   DEFINE_KEY ( LALT           , 0x1A4 )
,   DEFINE_KEY ( RALT           , 0x1A5 )
 
,   DEFINE_KEY ( SEMICOLON      , 0x1BA )
,   DEFINE_KEY ( PLUS           , 0x1BB )
,   DEFINE_KEY ( COMMA          , 0x1BC )
,   DEFINE_KEY ( MINUS          , 0x1BD )
,   DEFINE_KEY ( PERIOD         , 0x1BE )
,   DEFINE_KEY ( SLASH          , 0x1BF )
,   DEFINE_KEY ( GRAVE          , 0x1C0 )

,   KEY_COUNT
}
KEY;

/**
 * @brief Initializes the input subsystem. Call once to read the memory
 * requirement. Call again passing in a state pointer.
 * @param memory_requirement Output buffer to read memory requirement.
 * @param state Pass 0 to read memory requirement. Otherwise, pass a buffer.
 * @return false on error, true otherwise.
 */
bool
input_startup
(   u64*    memory_requirement
,   void*   state
);

/**
 * @brief Terminates the input subsystem.
 * @param state .
 */
void
input_shutdown
(   void* state
);

/**
 * @brief Update function.
 * @param dt Time elapsed since previous invocation.
 */
void
input_update
(   f64 dt
);

/**
 * @brief Queries current button status.
 * @param button The button to test.
 * @return true if button is down, false otherwise.
 */
bool
input_query_button_down
(   BUTTON button
);

/**
 * @brief Queries current button status.
 * @param button The button to test.
 * @return true if button is up, false otherwise.
 */
bool
input_query_button_up
(   BUTTON button
);

/**
 * @brief Queries previous button status.
 * @param button The button to test.
 * @return true if button was down, false otherwise.
 */
bool
input_query_button_down_prev
(   BUTTON button
);

/**
 * @brief Queries previous button status.
 * @param button The button to test.
 * @return true if button was up, false otherwise.
 */
bool
input_query_button_up_prev
(   BUTTON button
);

/**
 * @brief Queries the current mouse cursor position and copies the result into
 * two output buffers.
 * @param x The x coordinate output buffer.
 * @param y The y coordinate output buffer.
 */
void
input_query_mouse_position
(   i32* x
,   i32* y
);

/**
 * @brief Queries the previous mouse cursor position and copies the result into
 * two output buffers.
 * @param x The x coordinate output buffer.
 * @param y The y coordinate output buffer.
 */
void
input_query_mouse_position_prev
(   i32* x
,   i32* y
);

/**
 * @brief Queries the current key status.
 * @param key The key to test.
 * @return true if key is down, false otherwise.
 */
bool
input_query_key_down
(   KEY key
);

/**
 * @brief Queries the current key status.
 * @param key The key to test.
 * @return true if key is up, false otherwise.
 */
bool
input_query_key_up
(   KEY key
);

/**
 * @brief Queries the previous key status.
 * @param key The key to test.
 * @return true if key was down, false otherwise.
 */
bool
input_query_key_down_prev
(   KEY key
);

/**
 * @brief Queries the previous key status.
 * @param key The key to test.
 * @return true if key is up, false otherwise.
 */
bool
input_query_key_up_prev
(   KEY key
);

/**
 * @brief Processes one button press.
 * @param button The button to process.
 * @param pressed Is button pressed? Y/N
 */
void
input_process_button
(   BUTTON  button
,   bool    pressed
);

/**
 * @brief Processes mouse cursor movement.
 * @param x .
 * @param y .
 */
void
input_process_mouse_move
(   i16 x
,   i16 y
);

/**
 * @brief Processes mouse wheel movement.
 * @param dz .
 */
void
input_process_mouse_wheel
(   i8 dz
);

/**
 * @brief Processes one key press.
 * @param key The key to process.
 * @param pressed Is key pressed? Y/N
 */
void
input_process_key
(   KEY     key
,   bool    down
);

#endif  // INPUT_H
