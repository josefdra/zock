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

class Player;

class Map
{
public:
    Map(std::string);
    ~Map();
    void check_neighbours(uint16_t);
    void read_hash_map(const std::string);
    void print_map_with_transitions();
    void print_map_with_spectifications();
    void print_map();
    void setFieldValue(Player &);
    void check_corners();
    void check_before_protected_fields(std::vector<Player> &); 
    void check_borders();
    void check_before_borders();
    void check_before_before_borders();
    void check_special_fields();
    void check_before_special_fields();
    void check_before_before_special_fields();
    void print_frontier_scores(std::vector<Player> &);
    struct m_hash_map_element
    {
        unsigned char symbol;
        std::array<uint16_t, 8> transitions;
    };
    std::unordered_map<uint16_t, m_hash_map_element> m_symbol_and_transitions;
    uint16_t m_height;
    uint16_t m_width;
    uint16_t m_player_count;
    uint16_t m_strength;
    uint16_t m_initial_overwrite_stones;
    uint16_t m_initial_bombs;
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