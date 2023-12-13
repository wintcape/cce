/**
 * @file platform.h
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Platform-specific interface for the chess engine.
 */
#ifndef CHESS_PLATFORM_H
#define CHESS_PLATFORM_H

#include "common.h"

#include "platform/platform.h"

// Console color code indices for text rendering.
#define CCE_COLOR_DEFAULT   ANSI_CC ( ANSI_CC_NONE )
#define CCE_COLOR_HINT      ANSI_CC ( ANSI_CC_FG_GRAY )
#define CCE_COLOR_ALERT     ANSI_CC ( ANSI_CC_FG_RED )
#define CCE_COLOR_HIGHLIGHT ANSI_CC ( ANSI_CC_BG_DARK_RED )
#define CCE_COLOR_PLUS      ANSI_CC ( ANSI_CC_FG_DARK_GREEN )
#define CCE_COLOR_INFO      ANSI_CC ( ANSI_CC_FG_DARK_MAGENTA )

#endif  // CHESS_PLATFORM_H
