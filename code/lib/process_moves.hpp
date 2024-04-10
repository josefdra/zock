#ifndef PROCESS_MOVES_H
#define PROCESS_MOVES_H

#include "map.hpp"
#include "player.hpp"
#include "helper.hpp"

void print_valid_moves(std::unordered_map<uint16_t, std::tuple<uint16_t, std::unordered_set<uint16_t>>> &, uint16_t);
void check_coordinate(uint16_t, Map &, Player &, bool);
void change_players(Map &, char, char);
void execute_move(uint16_t coord, Player &, Map &);
void check_moves(Map &, Player &);
void paint_cells(std::unordered_set<uint16_t> &, unsigned char, Map &);

#endif // PROCESS_MOVES_H