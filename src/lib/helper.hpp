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
#include "algorithms.hpp"

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
EvalOfField evalFieldSymbol(unsigned char);
bool check_special(unsigned char);
uint16_t check_frontier(Map &, uint16_t);
void one_dimension_2_second_dimension(uint16_t &, uint8_t &, uint8_t &, Map &);
void two_dimension_2_one_dimension(uint16_t &, uint8_t &, uint8_t &, Map &);
uint8_t checkForSpecial(char &);
bool affectsMyPlayer(Player &, Player &, Map &);

#endif // HELPER_HPP