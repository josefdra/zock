#ifndef PROCESS_MOVES_HPP
#define PROCESS_MOVES_HPP

#include "map.hpp"
#include "player.hpp"
#include "helper.hpp"

void check_coordinate(uint16_t, Map &, Player &);
void change_players(Map &, char, char);
void execute_inversion(Map &m, char p);
void color(uint16_t c, char s, Map &);
void execute_move(uint16_t, uint8_t, Player &, Map &);
void execute_bomb(uint16_t, Map &, Player &);
void check_moves(Map &, Player &);

#endif // PROCESS_MOVES_HPP