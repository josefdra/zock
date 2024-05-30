#ifndef MOVE_BOARD_HPP
#define MOVE_BOARD_HPP

#include <vector>
#include <bitset>
#include <stdint.h>
#include <array>
#include <iostream>

#include "board.hpp"

class MoveBoard : public Board
{
public:
    MoveBoard(MoveBoard &, uint16_t, uint8_t);
    MoveBoard(const MoveBoard &);
    MoveBoard(const Board &);
    MoveBoard(MoveBoard &&) noexcept;
    MoveBoard &operator=(const MoveBoard &);
    MoveBoard &operator=(MoveBoard &&) noexcept;
    ~MoveBoard();

    void set_spec(uint8_t);
    void set_overwrite_stones(uint8_t, uint16_t);
    void set_bombs(uint8_t, uint16_t);
    void set_overwrite_move(uint8_t);
    uint16_t get_overwrite_stones(uint8_t);
    uint8_t get_spec();
    uint8_t get_border_set_size();
    std::bitset<2501> &get_valid_moves(uint8_t);
    std::bitset<2501> &get_wall_set(uint8_t);
    std::bitset<2501> &get_border_set(uint8_t);
    std::vector<std::bitset<2501>> &get_border_sets();
    void increment_overwrite_stones(uint8_t);
    void increment_bombs(uint8_t);
    void decrement_overwrite_stones(uint8_t);
    void decrement_bombs(uint8_t);
    bool has_overwrite_stones(uint8_t);
    bool is_overwrite_move(uint8_t);
    void reset_overwrite_moves();
    void print(uint8_t);

private:
    uint8_t m_spec;
    std::vector<bool> m_overwrite_move;
    std::vector<std::bitset<2501>> m_valid_moves;

    // first 8 sets for 1 to 8 walls next to field
    std::array<std::bitset<2501>, 8> m_wall_sets;
    std::vector<std::bitset<2501>> m_border_sets;
    std::vector<std::bitset<2501>> m_protected_fields;
};

#endif // MOVE_BOARD_HPP