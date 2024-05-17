#ifndef MAP_HPP
#define MAP_HPP

#define NUM_OF_DIRECTIONS 8

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <tuple>
#include <cstdint>
#include <unordered_map>
#include <chrono>
#include <iomanip>
#include <unordered_set>
#include <cstring>

// forward definition to prevent include errors
class Player;

class Map
{
public:
    Map();
    ~Map();
    void init_mults(uint8_t, uint8_t, uint8_t);
    void check_neighbours(uint16_t);
    void set_symbol(uint16_t, unsigned char);
    char get_symbol(uint16_t);
    void set_transition(uint16_t, uint8_t, uint16_t);
    uint16_t get_transition(uint16_t, uint8_t);
    uint8_t get_direction(uint16_t, uint8_t);
    void calculate_board_values();
    void read_hash_map(std::stringstream &);
    void print_map_with_transitions();
    void print_map_with_spectifications();
    void print_map();
    std::vector<char> m_symbols;
    std::vector<uint16_t> m_transitions;
    uint8_t m_player_number;
    uint16_t m_height;
    uint16_t m_width;
    uint16_t m_player_count;
    uint16_t m_strength;
    uint16_t m_radius_size;
    uint16_t m_initial_overwrite_stones;
    uint16_t m_initial_bombs;
    uint16_t m_num_of_fields;
    uint8_t m_mobility_multiplicator;
    uint8_t m_corners_and_special_multiplicator;
    uint8_t m_stone_multiplicator;
    std::unordered_map<uint16_t, int16_t> m_moves_scores;
    std::vector<int> m_constant_board_values;
    std::vector<int> m_variable_board_values;
};

#endif // MAP_HPP