/**
 * @file score.h
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Defines literals to score potential moves (see also, negamax.h).
 */
#ifndef CHESS_SCORE_H
#define CHESS_SCORE_H

#include "chess/common.h"

// Defines a material score for each piece.
static const i32 material_scores[] = { [ P ] = 100
                                     , [ N ] = 300
                                     , [ B ] = 350
                                     , [ R ] = 500
                                     , [ Q ] = 1000
                                     , [ K ] = 10000
                                     , [ p ] = -100
                                     , [ n ] = -300
                                     , [ b ] = -350
                                     , [ r ] = -500
                                     , [ q ] = -1000
                                     , [ k ] = -10000
                                     };

// Defines a positional score table for each piece.
static const i32 pawn_positional_scores[] = { 90 ,  90 ,  90 ,  90 ,  90 ,  90 ,  90 ,  90
                                            , 30 ,  30 ,  30 ,  40 ,  40 ,  30 ,  30 ,  30
                                            , 20 ,  20 ,  20 ,  30 ,  30 ,  30 ,  20 ,  20
                                            , 10 ,  10 ,  10 ,  20 ,  20 ,  10 ,  10 ,  10
                                            ,  5 ,   5 ,  10 ,  20 ,  20 ,   5 ,   5 ,   5
                                            ,  0 ,   0 ,   0 ,   5 ,   5 ,   0 ,   0 ,   0
                                            ,  0 ,   0 ,   0 , -10 , -10 ,   0 ,   0 ,   0
                                            ,  0 ,   0 ,   0 ,   0 ,   0 ,   0 ,   0 ,   0
                                            };
static const i32 knight_positional_scores[] = { -5 ,   0 ,   0 ,   0 ,   0 ,   0 ,   0 ,  -5
                                              , -5 ,   0 ,   0 ,  10 ,  10 ,   0 ,   0 ,  -5
                                              , -5 ,   5 ,  20 ,  20 ,  20 ,  20 ,   5 ,  -5
                                              , -5 ,  10 ,  20 ,  30 ,  30 ,  20 ,  10 ,  -5
                                              , -5 ,  10 ,  20 ,  30 ,  30 ,  20 ,  10 ,  -5
                                              , -5 ,   5 ,  20 ,  10 ,  10 ,  20 ,   5 ,  -5
                                              , -5 ,   0 ,   0 ,   0 ,   0 ,   0 ,   0 ,  -5
                                              , -5 , -10 ,   0 ,   0 ,   0 ,   0 , -10 ,  -5
                                              };
static const i32 bishop_positional_scores[] = {  0 ,   0 ,   0 ,   0 ,   0 ,   0 ,   0 ,   0
                                              ,  0 ,   0 ,   0 ,   0 ,   0 ,   0 ,   0 ,   0
                                              ,  0 ,   0 ,   0 ,  10 ,  10 ,   0 ,   0 ,   0
                                              ,  0 ,   0 ,  10 ,  20 ,  20 ,  10 ,   0 ,   0
                                              ,  0 ,   0 ,  10 ,  20 ,  20 ,  10 ,   0 ,   0
                                              ,  0 ,  10 ,   0 ,   0 ,   0 ,   0 ,  10 ,   0
                                              ,  0 ,  30 ,   0 ,   0 ,   0 ,   0 ,  30 ,   0
                                              ,  0 ,   0 , -10 ,   0 ,   0 , -10 ,   0 ,   0
                                              };
static const i32 rook_positional_scores[] = { 50 ,  50 ,  50 ,  50 ,  50 ,  50 ,  50 ,  50
                                            , 50 ,  50 ,  50 ,  50 ,  50 ,  50 ,  50 ,  50
                                            ,  0 ,   0 ,  10 ,  20 ,  20 ,  10 ,   0 ,   0
                                            ,  0 ,   0 ,  10 ,  20 ,  20 ,  10 ,   0 ,   0
                                            ,  0 ,   0 ,  10 ,  20 ,  20 ,  10 ,   0 ,   0
                                            ,  0 ,   0 ,  10 ,  20 ,  20 ,  10 ,   0 ,   0
                                            ,  0 ,   0 ,  10 ,  20 ,  20 ,  10 ,   0 ,   0
                                            ,  0 ,   0 ,   0 ,  20 ,  20 ,   0 ,   0 ,   0
                                            };
static const i32 king_positional_scores[] = {  0 ,   0 ,   0 ,   0 ,   0 ,   0 ,   0 ,   0
                                            ,  0 ,   0 ,   5 ,   5 ,   5 ,   5 ,   0 ,   0
                                            ,  0 ,   5 ,   5 ,  10 ,  10 ,   5 ,   5 ,   0
                                            ,  0 ,   5 ,  10 ,  20 ,  20 ,  10 ,   5 ,   0
                                            ,  0 ,   5 ,  10 ,  20 ,  20 ,  10 ,   5 ,   0
                                            ,  0 ,   0 ,   5 ,  10 ,  10 ,   5 ,   0 ,   0
                                            ,  0 ,   5 ,   5 ,  -5 ,  -5 ,   0 ,   5 ,   0
                                            ,  0 ,   0 ,   5 ,   0 , -15 ,   0 ,  10 ,   0
                                            };

// Defines the mirror score table indices for calculating the opposite side's
// score.
static const SQUARE mirror_position[ 128 ] = { A1 , B1 , C1 , D1 , E1 , F1 , G1 , H1
	                                         , A2 , B2 , C2 , D2 , E2 , F2 , G2 , H2
	   	                                     , A3 , B3 , C3 , D3 , E3 , F3 , G3 , H3
	                                         , A4 , B4 , C4 , D4 , E4 , F4 , G4 , H4
	                                         , A5 , B5 , C5 , D5 , E5 , F5 , G5 , H5
	                                         , A6 , B6 , C6 , D6 , E6 , F6 , G6 , H6
	                                         , A7 , B7 , C7 , D7 , E7 , F7 , G7 , H7
	                                         , A8 , B8 , C8 , D8 , E8 , F8 , G8 , H8
	                                         };

#endif  // CHESS_SCORE_H