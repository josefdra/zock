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
int evaluate_board(uint8_t, Player &, std::vector<char> &, Map &, std::vector<Player> &);

#endif // ALGORITHMS_HPP
