#ifndef PROCESS_MOVES_HPP
#define PROCESS_MOVES_HPP

#include "map.hpp"
#include "player.hpp"
#include "helper.hpp"

void change_players(std::vector<char> &, char, char);
void execute_inversion(std::vector<char> &, std::vector<char> &, Map &);
void color(uint16_t c, char s, Map &);
void execute_move(uint16_t, uint8_t, Player &, Map &);
void execute_bomb(uint16_t, Map &, Player &);
std::vector<char> temp_color(uint16_t, char, Map &, std::vector<char> &);
void calculate_valid_moves(Map &, Player &, std::vector<char> &);

#endif // PROCESS_MOVES_HPP