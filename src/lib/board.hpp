#ifndef BOARD_HPP
#define BOARD_HPP

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
    dark_black,
    orange
};

class Board
{
public:
    Board(Map &);
    Board(const Board &);
    Board(Board &&) noexcept;
    Board&operator=(const Board &);
    Board&operator=(Board &&) noexcept;
    ~Board();

    void set_coord(uint16_t);
    void set_disqualified(uint8_t);
    uint8_t get_player_count();
    uint16_t get_num_of_fields();
    uint8_t get_width();
    uint8_t get_height();
    uint16_t get_bombs(uint8_t);
    std::vector<uint16_t> &get_all_bombs();
    uint16_t get_coord();
    uint8_t get_player_num();
    int get_evaluation();
    std::vector<bool> &get_disqualified();
    std::bitset<2501> &get_board_set(uint8_t);
    std::array<std::bitset<2501>, 7> &get_board_sets();
    std::bitset<2501> &get_player_set(uint8_t);
    std::vector<std::bitset<2501>> &get_player_sets();
    bool is_disqualified(uint8_t);
    void one_dimension_2_second_dimension(uint16_t, uint8_t &, uint8_t &);
    uint16_t two_dimension_2_one_dimension(uint8_t, uint8_t);
    std::string get_color_string(Colors);
    void print_upper_outlines();
    bool print_m_board_sets(uint16_t);
    void print_bitset(std::bitset<2501> &);

protected:
    // boards[0] = - board
    // boards[1] = empty board (0, i, c, b)
    // boards[2] = i board
    // boards[3] = c board
    // boards[4] = b board
    // boards[5] = x board
    // boards[6] = border around occupied fields
    std::array<std::bitset<2501>, 7> m_board_sets;
    std::vector<std::bitset<2501>> m_player_sets;
    std::vector<uint16_t> m_overwrite_stones;
    std::vector<uint16_t> m_bombs;
    std::vector<bool> m_disqualified;
    uint8_t m_our_player;
    uint8_t m_player_count;
    uint16_t m_num_of_fields;
    uint8_t m_width;
    uint8_t m_height;
    uint16_t m_coord;
    int evaluation;
};

#endif // BOARD_HPP