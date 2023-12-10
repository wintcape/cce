/**
 * @file platform.h
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Platform-specific interface for the chess engine.
 */
#ifndef CHESS_PLATFORM_H
#define CHESS_PLATFORM_H

#include "platform/platform.h"

// Console color code indices for text rendering.
#define CCE_COLOR_DEFAULT   LOG_LEVEL_COUNT
#define CCE_COLOR_HINT      ( LOG_LEVEL_COUNT+1 )
#define CCE_COLOR_ALERT     LOG_ERROR
#define CCE_COLOR_HIGHLIGHT LOG_FATAL
#define CCE_COLOR_PLUS      LOG_INFO
#define CCE_COLOR_INFO      9

#endif  // CHESS_PLATFORM_H
