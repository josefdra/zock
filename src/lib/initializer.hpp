#ifndef INITIALIZER_HPP
#define INITIALIZER_HPP

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
#define THREE_WALLS_VALUE 500
#define FOUR_WALLS_VALUE 3500
#define FIVE_WALLS_VALUE 2500
#define BEFORE_THREE_WALLS_VALUE -200
#define BEFORE_FOUR_WALLS_VALUE -1400
#define BEFORE_FIVE_WALLS_VALUE -1000
#define BEFORE_BEFORE_THREE_WALLS_VALUE 10
#define BEFORE_BEFORE_FOUR_WALLS_VALUE 70
#define BEFORE_BEFORE_FIVE_WALLS_VALUE 50
#define THREE_WALLS 0
#define FOUR_WALLS 1
#define FIVE_WALLS 2

#include <stdint.h>
#include <sstream>
#include <vector>
#include <iostream>
#include <bitset>
#include <array>
#include <unordered_set>
#include <iomanip>
#include <algorithm>
#include <random>

class Board;

class Initializer
{
public:
    Initializer();
    ~Initializer();
    void check_neighbours(uint16_t);
    void set_symbol(uint16_t, unsigned char);
    void set_transition(uint16_t, uint8_t, uint16_t);
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
    void expand_coord(uint16_t, uint8_t, uint16_t);
    void init_next_coords();
    void read_map(std::stringstream);
    void one_dimension_2_second_dimension(uint16_t, uint8_t &, uint8_t &);
    uint16_t two_dimension_2_one_dimension(uint8_t, uint8_t);
    bool check_players(char);
    void set_values(Board &, uint16_t);
    void init_wall_values(Board &, std::bitset<MAX_NUM_OF_FIELDS> &);
    void init_before_wall_values(Board &);
    void init_before_before_wall_values(Board &);
    bool get_walls(Board &, std::bitset<MAX_NUM_OF_FIELDS> &);
    void check_if_protected_field(Board &, uint8_t, uint16_t);
    void expand_protected_fields(Board &, uint8_t);
    void expand_community(Board &, std::bitset<MAX_NUM_OF_FIELDS> &, uint16_t, std::bitset<MAX_NUM_OF_FIELDS> &);
    void init_frames(Board &);
    void remove_double_communities(Board &);
    void init_players_in_communities_count(Board &);
    void init_communities(Board &);
    void init_static_evaluation(Board &);
    void calculate_before_protected_fields(Board &);
    void check_if_special_map(Board &);
    Board init_boards_and_players();
    void generate_transitions();
    void init_possible_fields();
    uint16_t get_sum_possible_fields();

    std::array<std::bitset<MAX_NUM_OF_FIELDS>, 3> wall_sets;
    std::array<std::bitset<MAX_NUM_OF_FIELDS>, 3> before_wall_sets;
    std::array<std::bitset<MAX_NUM_OF_FIELDS>, 3> before_before_wall_sets;

    std::bitset<MAX_NUM_OF_FIELDS> corners_and_walls;

    std::vector<std::vector<uint16_t>> next_coords;

    std::bitset<MAX_NUM_OF_FIELDS * NUM_OF_DIRECTIONS> checked_coords;

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
    uint16_t m_possible_fields = 0;
};

#endif // INITIALIZER_HPP