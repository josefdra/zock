#ifndef MAP_HPP
#define MAP_HPP

#define NUM_OF_DIRECTIONS 8
#define MAX_NUM_OF_FIELDS 2501
#define COORD_TO_DIR_OFFSET 10
#define UP 0
#define UPPER_RIGHT 1
#define RIGHT 2
#define LOWER_RIGHT 3
#define DOWN 4
#define LOWER_LEFT 5
#define LEFT 6
#define UPPER_LEFT 7

#include <stdint.h>
#include <sstream>
#include <vector>
#include <iostream>
#include <bitset>
#include <array>
#include <unordered_set>
#include <iomanip>

class Board;

class Map
{
public:
    Map();
    ~Map();
    void check_neighbours(uint16_t);
    void set_symbol(uint16_t, unsigned char);
    void set_transition(uint16_t, uint8_t, uint16_t);
    void set_player_number(uint8_t);
    uint8_t get_player_number();
    char get_symbol(uint16_t);
    uint16_t get_transition(uint16_t, uint8_t);
    uint8_t get_direction(uint16_t, uint8_t);
    uint16_t get_height();
    uint16_t get_width();
    uint16_t get_player_count();
    uint16_t get_strength();
    uint16_t get_num_of_fields();
    uint16_t get_initial_overwrite_stones();
    uint16_t get_initial_bombs();
    std::vector<uint16_t> get_transitions();
    uint8_t get_reverse_direction(uint8_t);
    void read_map(std::stringstream);
    void one_dimension_2_second_dimension(uint16_t, uint8_t &, uint8_t &);
    uint16_t two_dimension_2_one_dimension(uint8_t, uint8_t);
    bool check_players(char);
    void set_values(Board &, uint16_t);
    void init_wall_values(Board &);
    bool get_walls(Board &, std::bitset<MAX_NUM_OF_FIELDS> &);
    std::bitset<MAX_NUM_OF_FIELDS> get_inside_of_walls(Board &, std::bitset<MAX_NUM_OF_FIELDS> &, uint16_t);
    bool set_player_border_sets(Board &, std::bitset<MAX_NUM_OF_FIELDS>);
    void init_evaluation(Board &);
    void expand_community(Board &, std::bitset<MAX_NUM_OF_FIELDS> &, uint16_t, std::bitset<MAX_NUM_OF_FIELDS> &);
    void init_frames(Board &);
    void remove_double_communities(Board &);
    void init_communities(Board &);
    Board init_boards_and_players();

private:
    std::vector<char> m_numbers;
    std::vector<uint16_t> m_transitions;
    uint16_t m_height;
    uint16_t m_width;
    uint16_t m_player_count;
    uint16_t m_strength;
    uint16_t m_initial_overwrite_stones;
    uint16_t m_initial_bombs;
    uint16_t m_num_of_fields;
    uint8_t m_player_number;
};

#endif // MAP_HPP