#ifndef PROCESS_MOVES_H
#define PROCESS_MOVES_H

#include "map.hpp"
#include "player.hpp"
#include "helper.hpp"

void check_coordinate(uint16_t, Map &, Player &, bool);
void change_players(Map &, char, char);
void execute_move(uint16_t coord, Player &, Map &);
void check_moves(Map &, Player &);
void paint_cells(std::unordered_set<uint16_t> &, unsigned char, Map &);

#endif // PROCESS_MOVES_H