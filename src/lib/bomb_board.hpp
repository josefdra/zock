#ifndef BOMB_BOARD_HPP
#define BOMB_BOARD_HPP

#include <vector>
#include <bitset>
#include <stdint.h>

class Board;
class Map;

enum Colors_Bomb
{
    m_red = 1,
    m_blue,
    m_green,
    m_yellow,
    m_cyan,
    m_magenta,
    m_dark_black,
    m_orange
};

class BombBoard
{
public:
    BombBoard();
    ~BombBoard();

    void init_bomb_board(Board &, Map &);
    void set_coord(uint16_t);
    void set_transition(uint16_t, uint8_t, uint16_t);
    void set_zero_transition(uint16_t);
    uint8_t get_player_count();
    uint16_t get_num_of_fields();
    uint8_t get_width();
    uint8_t get_height();
    uint16_t get_bombs(uint8_t);
    uint16_t get_coord();
    int get_evaluation();
    uint16_t get_transition(uint16_t, uint8_t);
    uint8_t get_direction(uint16_t, uint8_t);
    void decrement_bombs(uint8_t);
    void one_dimension_2_second_dimension(uint16_t, uint8_t &, uint8_t &);
    uint16_t two_dimension_2_one_dimension(uint8_t, uint8_t);
    std::string get_color_string(Colors_Bomb);
    void print_upper_outlines();
    bool print_bomb_board_sets(uint16_t);
    void print(uint8_t, bool);
    void print_bitset(std::bitset<2501> &);
    void get_bomb_coords(uint16_t, uint16_t, uint8_t);
    uint8_t get_strength();
    void init_bomb_phase_boards();

    void print_transitions(std::bitset<20000> &);

    std::vector<std::bitset<2501>> board_sets;
    std::vector<std::bitset<2501>> player_sets;
    std::vector<std::bitset<2501>> valid_moves;

    std::vector<std::bitset<2501>> fields_to_remove;
    std::vector<std::bitset<20000>> transitions_to_remove;

    std::vector<uint16_t> bombs;
    std::vector<bool> disqualified;

private:
    std::vector<uint16_t> m_transitions;
    uint8_t m_player_count;
    uint16_t m_num_of_fields;
    uint8_t m_width;
    uint8_t m_height;
    uint16_t m_coord;
    uint8_t m_strength;
    int evaluation;
};

#endif // BOMB_BOARD_HPP