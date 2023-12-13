/**
 * @file ansicc.h
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Defines an alias for each ANSI color code.
 */
#ifndef ANSI_CC_H
#define ANSI_CC_H

/**
 * @brief Defines an alias for an ANSI color code with one attribute.
 */
#define ANSI_CC(color)          "\033["color"m"

/**
 * @brief Defines an alias for an ANSI color code with two attributes.
 */
#define ANSI_CC2(color,attr)    "\033["attr";"color"m"

/**
 * @brief Defines an alias for the 'reset color' ANSI color code.
 */
#define ANSI_CC_RESET           ANSI_CC ( ANSI_CC_NONE )

/**
 * @brief Attribute codes.
 */

#define ANSI_CC_NONE            "0"
#define ANSI_CC_BOLD            "1"
#define ANSI_CC_UNDERLINE       "2"

#define ANSI_CC_FG_BLACK        "30"
#define ANSI_CC_FG_WHITE        "97"
#define ANSI_CC_FG_DARK_GRAY    "90"
#define ANSI_CC_FG_GRAY         "37"
#define ANSI_CC_FG_RED          "91"
#define ANSI_CC_FG_GREEN        "92"
#define ANSI_CC_FG_YELLOW       "93"
#define ANSI_CC_FG_BLUE         "94"
#define ANSI_CC_FG_MAGENTA      "95"
#define ANSI_CC_FG_CYAN         "96"
#define ANSI_CC_FG_DARK_RED     "31"
#define ANSI_CC_FG_DARK_GREEN   "32"
#define ANSI_CC_FG_DARK_YELLOW  "33"
#define ANSI_CC_FG_DARK_BLUE    "34"
#define ANSI_CC_FG_DARK_MAGENTA "35"
#define ANSI_CC_FG_DARK_CYAN    "36"

#define ANSI_CC_BG_BLACK        "40"
#define ANSI_CC_BG_WHITE        "107"
#define ANSI_CC_BG_OFF_BLACK    "100"
#define ANSI_CC_BG_OFF_WHITE    "47"
#define ANSI_CC_BG_RED          "101"
#define ANSI_CC_BG_GREEN        "102"
#define ANSI_CC_BG_YELLOW       "103"
#define ANSI_CC_BG_BLUE         "104"
#define ANSI_CC_BG_MAGENTA      "105"
#define ANSI_CC_BG_CYAN         "106"
#define ANSI_CC_BG_DARK_RED     "41"
#define ANSI_CC_BG_DARK_GREEN   "42"
#define ANSI_CC_BG_DARK_YELLOW  "43"
#define ANSI_CC_BG_DARK_BLUE    "44"
#define ANSI_CC_BG_DARK_MAGENTA "45"
#define ANSI_CC_BG_DARK_CYAN    "46"

#endif  // ANSI_CC_H
