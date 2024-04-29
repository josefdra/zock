#ifndef ALGORITHMS_HPP
#define ALGORITHMS_HPP

#include <algorithm>
#include "map.hpp"
#include "player.hpp"
#include "process_moves.hpp"
#include "game.hpp"
#include "helper.hpp"
#include <chrono>

class Game;

int minimaxOrParanoiaWithPruning(uint16_t, uint8_t, int, int, bool, Map &, Game &, uint8_t &, uint8_t &, Player &, uint16_t &);
int getBestPosition(uint8_t &, Map &, uint8_t &, uint8_t &, Game &);
uint8_t checkForSpecial(char &);

#endif // ALGORITHMS_HPP
