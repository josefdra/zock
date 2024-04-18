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
// forward definition to prevent include errors
class Player;

class Map
{
public:
    Map(std::string);
    ~Map();
    void check_neighbours(uint16_t);
    void set_symbol(uint16_t, unsigned char);
    unsigned char get_symbol(uint16_t);
    void set_transition(uint16_t, uint8_t, uint16_t);
    uint16_t get_transition(uint16_t, uint8_t);
    uint8_t get_direction(uint16_t, uint8_t);
    void read_hash_map(const std::string);
    void print_map_with_transitions();
    void print_map_with_spectifications();
    void print_map();
    void setFieldValue(Player &);
    void check_corners_borders_special_fields();
    void check_before_protected_fields(std::vector<Player> &);
    void check_before_borders();
    void check_before_before_borders();
    void check_before_special_fields();
    void check_before_before_special_fields();
    void print_m_frontier_scores(std::vector<Player> &);
    std::unordered_map<uint16_t, unsigned char> m_symbols;
    std::unordered_map<uint16_t, uint64_t> m_lower_transitions;
    std::unordered_map<uint16_t, uint64_t> m_upper_transitions;
    uint16_t m_height;
    uint16_t m_width;
    uint16_t m_player_count;
    uint16_t m_strength;
    uint16_t m_initial_overwrite_stones;
    uint16_t m_initial_bombs;
    uint16_t m_num_of_fields;
    std::unordered_set<uint16_t> m_corners;
    std::unordered_set<uint16_t> m_protected_fields;
    std::unordered_set<uint16_t> m_before_protected_fields;
    std::unordered_set<uint16_t> m_borders;
    std::unordered_set<uint16_t> m_before_borders;
    std::unordered_set<uint16_t> m_before_before_borders;
    std::unordered_set<uint16_t> m_special_fields;
    std::unordered_set<uint16_t> m_before_special_fields;
    std::unordered_set<uint16_t> m_before_before_special_fields;
};

#endif // MAP_HPP