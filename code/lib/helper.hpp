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

enum EvalOfField
{
    empty = -1,
    corner = 30,
    aroundCorner = -20,
    bomb = 20,
    inversion = 10,
    choice = 20,
    enemy = -20,
    aroundSpecial = -20
};

typedef std::chrono::high_resolution_clock h_res_clock;

std::string getColorString(Colors);
bool check_empty_fields(unsigned char);
bool check_players(unsigned char);
uint16_t check_frontier(Map &, uint16_t);
void print_corners(Map &);
EvalOfField evalFieldSymbol(char);

#endif // HELPER_H