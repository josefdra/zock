#ifndef CHECK_VALID_MOVES_H
#define CHECK_VALID_MOVES_H

#include <array>

#include "map.hpp"

void process_moves(Map&);
bool check_empty_fields(unsigned char);
bool check_players(unsigned char);
void paint_cells(std::unordered_set<uint16_t>&, unsigned char, Map&);

#endif // CHECK_VALID_MOVES_H