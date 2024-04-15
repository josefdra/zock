#ifndef MAP_H
#define MAP_H

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
    struct m_hash_map_element
    {
        unsigned char symbol;
        std::array<uint16_t, 8> transitions;
    };
    std::unordered_map<uint16_t, m_hash_map_element> m_symbol_and_transitions;
    // @todo eventuell zu private ändern und getter, bzw. setter hinzufügen
    uint16_t m_height;
    uint16_t m_width;
    uint16_t m_player_count;
    uint16_t m_strength;
    uint16_t m_initial_overwrite_stones;
    uint16_t m_initial_bombs;

private:
};

#endif // MAP_H