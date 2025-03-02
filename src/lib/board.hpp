#ifndef BOARD_HPP
#define BOARD_HPP

#define MIN_NUM_OF_FIELDS 64
#define MAX_NUM_OF_FIELDS 2501
#define NUM_OF_BOARD_SETS 6
#define MINUS 0
#define EMPTY 1
#define I 2
#define C 3
#define B 4
#define X 5

#include <stdint.h>
#include <bitset>
#include <vector>
#include <array>
#include <string>

class Initializer;

enum Colors
{
    red = 1,
    blue,
    green,
    yellow,
    cyan,
    magenta,
    black,
    orange
};

class Board
{
public:
    Board(Initializer &);
    Board(Board &, uint16_t, uint8_t);
    ~Board();

    void set_our_player(uint8_t);
    void set_coord(uint16_t);
    void set_spec(uint8_t);
    void set_overwrite_stones(uint8_t, uint16_t);
    void set_bombs(uint8_t, uint16_t);
    void set_overwrite_move(uint8_t);
    void set_final_state();
    void set_bonus_field();
    void set_choice_field();
    uint8_t get_our_player();
    uint8_t get_player_count();
    uint16_t get_num_of_fields();
    uint16_t get_num_of_not_minus_fields();
    uint8_t get_width();
    uint8_t get_height();
    uint16_t get_overwrite_stones(uint8_t);
    uint16_t get_bombs(uint8_t);
    uint16_t get_coord();
    uint8_t get_spec();
    int get_evaluation();
    uint8_t get_num_of_communities();
    void increment_overwrite_stones(uint8_t);
    void increment_bombs(uint8_t);
    void decrement_overwrite_stones(uint8_t);
    void decrement_bombs(uint8_t);
    void decrement_not_minus_fields();
    bool has_overwrite_stones(uint8_t);
    bool is_overwrite_move(uint8_t);
    bool is_final_state();
    bool check_bonus_field();
    bool check_choice_field();
    void reset_overwrite_moves();
    void reset_bonus_field();
    void reset_choice_field();
    void one_dimension_2_second_dimension(uint16_t, uint8_t &, uint8_t &);
    uint16_t two_dimension_2_one_dimension(uint8_t, uint8_t);
    std::string get_color_string(Colors);
    void print_upper_outlines();
    bool print_board_sets(uint16_t);
    void print(uint8_t, bool);
    void print_bitset(std::bitset<MAX_NUM_OF_FIELDS> &);
    void reset_valid_moves(uint8_t);
    std::bitset<MAX_NUM_OF_FIELDS> get_total_moves(uint8_t);
    void calc_occupied_percentage(uint16_t);
    void calculate_scaling_factor(uint16_t);

    // boards[0] = - board
    // boards[1] = empty board (0, i, c, b)
    // boards[2] = i board
    // boards[3] = c board
    // boards[4] = b board
    // boards[5] = x board
    std::array<std::bitset<MAX_NUM_OF_FIELDS>, NUM_OF_BOARD_SETS> board_sets;
    std::vector<std::bitset<MAX_NUM_OF_FIELDS>> player_sets;
    std::vector<std::vector<std::bitset<MAX_NUM_OF_FIELDS>>> valid_moves;
    // first 8 sets for 1 to 8 walls next to field

    std::vector<int> static_evaluation;
    std::bitset<MAX_NUM_OF_FIELDS> fixed_protected_fields;
    std::vector<std::bitset<MAX_NUM_OF_FIELDS>> protected_fields;
    std::vector<std::bitset<MAX_NUM_OF_FIELDS>> before_protected_fields;
    std::bitset<MAX_NUM_OF_FIELDS> corners_and_walls;

    std::vector<uint16_t> overwrite_stones;
    std::vector<uint16_t> bombs;

    std::vector<std::bitset<MAX_NUM_OF_FIELDS>> communities;
    std::vector<std::bitset<MAX_NUM_OF_FIELDS>> frames;

    std::vector<uint8_t> num_of_players_in_community;

    std::vector<std::tuple<uint16_t, uint16_t>> start_end_communities;
    std::vector<std::tuple<uint16_t, uint16_t>> start_end_frames;

    std::vector<bool> disqualified;

    std::bitset<MAX_NUM_OF_FIELDS> before_bonus_fields;
    std::bitset<MAX_NUM_OF_FIELDS> before_choice_fields;

    uint16_t occupied_fields = 0;
    uint8_t occupied_percentage = 0;
    double scaling_factor = 0;
    std::vector<uint32_t> special_moves;
    std::array<uint16_t, 3> special_coords;

private:
    uint8_t m_our_player;
    uint8_t m_player_count;
    uint16_t m_num_of_fields;
    uint16_t m_num_of_not_minus_fields;
    uint8_t m_width;
    uint8_t m_height;
    uint16_t m_coord;
    uint8_t m_spec;
    std::vector<bool> m_overwrite_move;
    int evaluation;
    bool final_state;
    bool m_bonus_field;
    bool m_choice_field;
};

#endif // BOARD_HPP