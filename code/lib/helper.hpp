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
void print_corners(Map &);

#endif // HELPER_H