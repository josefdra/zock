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

char get_symbol(std::vector<char> &, uint16_t);
int minimaxOrParanoidWithPruning(uint8_t, int, int, bool, std::vector<char> &, Map &, uint8_t &, uint8_t &, Player &, uint16_t &);
uint8_t checkForSpecial(char &);

#endif // ALGORITHMS_HPP
