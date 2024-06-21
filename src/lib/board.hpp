#ifndef BOARD_HPP
#define BOARD_HPP

#define MAX_NUM_OF_FIELDS 2501
#define NUM_OF_BOARD_SETS 6
#define NUM_OF_WALL_SETS 8
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

class Map;

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
    Board(Map &);
    Board(Board &, uint16_t, uint8_t);
    ~Board();

    void set_coord(uint16_t);
    void set_spec(uint8_t);
    void set_overwrite_stones(uint8_t, uint16_t);
    void set_bombs(uint8_t, uint16_t);
    void set_overwrite_move(uint8_t);
    void set_final_state();
    uint8_t get_player_count();
    uint16_t get_num_of_fields();
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
    bool has_overwrite_stones(uint8_t);
    bool is_overwrite_move(uint8_t);
    bool is_final_state();
    void reset_overwrite_moves();
    void one_dimension_2_second_dimension(uint16_t, uint8_t &, uint8_t &);
    uint16_t two_dimension_2_one_dimension(uint8_t, uint8_t);
    std::string get_color_string(Colors);
    void print_upper_outlines();
    bool print_board_sets(uint16_t);
    void print(uint8_t, bool);
    void print_bitset(std::bitset<MAX_NUM_OF_FIELDS> &);
    void reset_valid_moves(uint8_t);
    std::bitset<MAX_NUM_OF_FIELDS> get_total_moves(uint8_t);
    void calculate_offsets();

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
    std::array<std::bitset<MAX_NUM_OF_FIELDS>, NUM_OF_WALL_SETS> wall_sets;
    std::vector<std::bitset<MAX_NUM_OF_FIELDS>> border_sets;

    std::vector<uint16_t> overwrite_stones;
    std::vector<uint16_t> bombs;

    std::vector<std::bitset<MAX_NUM_OF_FIELDS>> communities;
    std::vector<std::bitset<MAX_NUM_OF_FIELDS>> frames;

    std::vector<std::tuple<uint16_t, uint16_t>> start_end_communities;
    std::vector<std::tuple<uint16_t, uint16_t>> start_end_frames;

    std::vector<uint8_t> communities_offset;
    std::vector<uint8_t> frames_offset;

    std::vector<bool> disqualified;

private:
    uint8_t m_player_count;
    uint16_t m_num_of_fields;
    uint8_t m_width;
    uint8_t m_height;
    uint16_t m_coord;
    uint8_t m_spec;
    std::vector<bool> m_overwrite_move;
    int evaluation;
    bool final_state;
};

#endif // BOARD_HPP