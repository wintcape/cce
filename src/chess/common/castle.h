/**
 * @file castle.h
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Defines castling rights.
 */
#ifndef CHESS_COMMON_CASTLE_H
#define CHESS_COMMON_CASTLE_H

typedef enum
{
    CASTLE_WK = 1
,   CASTLE_WQ = 2
,   CASTLE_BK = 4
,   CASTLE_BQ = 8
}
CASTLE;

#endif  // CHESS_COMMON_CASTLE_H