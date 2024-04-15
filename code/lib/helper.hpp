#ifndef HELPER_H
#define HELPER_H

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
    red = 1,
    blue,
    green,
    orange,
    magenta,
    yellow,
    dark_black,
    cyan,
};

typedef std::chrono::high_resolution_clock h_res_clock;

std::string getColorString(Colors);
bool check_empty_fields(unsigned char);
bool check_players(unsigned char);
uint16_t check_frontier(Map &, uint16_t);
void print_corners(Map &);

#endif // HELPER_H