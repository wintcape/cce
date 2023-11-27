/**
 * @file attack.h
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Defines operations for pregenerating bitboard attack tables.
 */
#ifndef CHESS_ATTACK_H
#define CHESS_ATTACK_H

#include "chess/common.h"

/**
 * @brief Pregenerates attack tables.
 * @param attacks A buffer to write the pregenerated attack tables to.
 * @return false if no buffer provided, true otherwise.
 */
bool
attacks_init
(   attacks_t* attacks
);

#endif  // CHESS_ATTACK_H
