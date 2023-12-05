/**
 * @file castle.h
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Defines castling rights.
 */
#ifndef CHESS_CASTLE_H
#define CHESS_CASTLE_H

static const u8 castling_rights[ 64 ] = {  7 , 15 , 15 , 15 ,  3 , 15 , 15 , 11
                                        , 15 , 15 , 15 , 15 , 15 , 15 , 15 , 15
                                        , 15 , 15 , 15 , 15 , 15 , 15 , 15 , 15
                                        , 15 , 15 , 15 , 15 , 15 , 15 , 15 , 15
                                        , 15 , 15 , 15 , 15 , 15 , 15 , 15 , 15
                                        , 15 , 15 , 15 , 15 , 15 , 15 , 15 , 15
                                        , 15 , 15 , 15 , 15 , 15 , 15 , 15 , 15
                                        , 13 , 15 , 15 , 15 , 12 , 15 , 15 , 14
                                        };

#endif  // CHESS_CASTLE_H
