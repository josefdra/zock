#ifndef BOARD_HPP
#define BOARD_HPP

#include <stdint.h>
#include <bitset>
#include <vector>
#include <array>
#include <string>

enum Colors
{
    red = 1,
    blue,
    green,
    yellow,
    cyan,
    magenta,
    dark_black,
    orange
};

class Map;

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
    uint8_t get_player_count();
    uint16_t get_num_of_fields();
    uint8_t get_width();
    uint8_t get_height();
    uint16_t get_overwrite_stones(uint8_t);
    uint16_t get_bombs(uint8_t);
    uint16_t get_coord();
    uint8_t get_spec();
    int get_evaluation();
    void increment_overwrite_stones(uint8_t);
    void increment_bombs(uint8_t);
    void decrement_overwrite_stones(uint8_t);
    void decrement_bombs(uint8_t);
    bool has_overwrite_stones(uint8_t);
    bool is_overwrite_move(uint8_t);
    void reset_overwrite_moves();
    void one_dimension_2_second_dimension(uint16_t, uint8_t &, uint8_t &);
    uint16_t two_dimension_2_one_dimension(uint8_t, uint8_t);
    std::string get_color_string(Colors);
    void print_upper_outlines();
    bool print_board_sets(uint16_t);
    void print(uint8_t, bool);
    void print_bitset(std::bitset<2501> &);

    // boards[0] = - board
    // boards[1] = empty board (0, i, c, b)
    // boards[2] = i board
    // boards[3] = c board
    // boards[4] = b board
    // boards[5] = x board
    // boards[6] = border around occupied fields
    std::vector<std::bitset<2501>> board_sets;
    std::vector<std::bitset<2501>> player_sets;
    std::vector<std::bitset<2501>> valid_moves;
    // first 8 sets for 1 to 8 walls next to field
    std::array<std::bitset<2501>, 8> wall_sets;
    std::vector<std::bitset<2501>> border_sets;
    std::vector<std::bitset<2501>> protected_fields;

    std::vector<uint16_t> overwrite_stones;
    std::vector<uint16_t> bombs;

private:
    uint8_t m_player_count;
    uint16_t m_num_of_fields;
    uint8_t m_width;
    uint8_t m_height;
    uint16_t m_coord;
    uint8_t m_spec;
    std::vector<bool> m_overwrite_move;
    int evaluation;
};

#endif // BOARD_HPP