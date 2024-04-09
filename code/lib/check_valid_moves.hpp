#ifndef CHECK_VALID_MOVES_H
#define CHECK_VALID_MOVES_H

#include "map.hpp"

void print_tuple_vector(std::vector<std::tuple<uint16_t, uint16_t>>&);
bool check_empty_fields(unsigned char);
bool check_players(unsigned char);
void check_coordinate(uint16_t coord, std::vector<std::tuple<uint16_t, uint16_t>>& valid_moves, Map &map, bool paint);
void process_moves(Map &);
void paint_cells(std::unordered_set<uint16_t> &, unsigned char, Map &);

#endif // CHECK_VALID_MOVES_H