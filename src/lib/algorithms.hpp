#ifndef ALGORITHMS_HPP
#define ALGORITHMS_HPP

#include <chrono>
#include <iostream>
#include <algorithm>

#include "map.hpp"
#include "player.hpp"
#include "process_moves.hpp"
#include "game.hpp"
#include "helper.hpp"

class Game;

void get_frontier_score(Player &, std::vector<char> &, Map &);
int evaluate_board(uint8_t, Player &, Map &, std::vector<char> &);
int minimaxOrParanoidWithPruning(Game &, uint8_t, int, int, bool, std::vector<char> &, uint8_t &, uint8_t &, Player &, uint16_t &);
uint8_t checkForSpecial(char &);

#endif // ALGORITHMS_HPP
