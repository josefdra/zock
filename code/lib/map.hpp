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

typedef std::chrono::high_resolution_clock h_res_clock;

class Map
{
public:
    Map();
    ~Map();
    void check_neighbours(uint16_t n);
    void read_hash_map(const std::string inputfile);
    void print_transitions();
    void print_map();
    void process_moves();
    void paint_cells(std::vector<std::vector<uint16_t>>&);
    struct hash_map_element
    {
        unsigned char symbol;
        std::array<uint16_t, 8> transitions;
        bool hasTransitions = false;
    };
    std::unordered_map<uint16_t, hash_map_element> all_map_moves;
    uint16_t height;
    uint16_t width;
    uint8_t spielerzahl;
    uint16_t ueberschreibsteine;
    uint16_t bomben;
    uint8_t staerke;

private:    
};

#endif // MAP_H