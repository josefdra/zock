#ifndef HELPER_HPP
#define HELPER_HPP

#include <functional>
#include <iostream>
#include <chrono>
#include <random>
#include <algorithm>
#include <tuple>
#include <vector>

#include "map.hpp"

enum Colors
{
    black = 1,
    red,
    green,
    yellow,
    blue,
    magenta,
    cyan,
    dark_black,
};

typedef std::chrono::high_resolution_clock h_res_clock;

std::string getColorString(Colors);
bool check_empty_fields(unsigned char);
bool check_players(unsigned char);
bool check_special(unsigned char);
uint16_t check_frontier(Map &, uint16_t);

#endif // HELPER_HPP