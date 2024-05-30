#ifndef BOMB_BOARD_HPP
#define BOMB_BOARD_HPP

#include <vector>
#include <bitset>
#include <stdint.h>

#include "board.hpp"

class Map;
class MoveBoard;

class BombBoard : public Board
{
public:
    BombBoard(Map &);
    BombBoard(const BombBoard& other);
    BombBoard(BombBoard&& other) noexcept;
    BombBoard& operator=(const BombBoard& other);
    BombBoard& operator=(BombBoard&& other) noexcept;
    ~BombBoard();

    void set_transition(uint16_t, uint8_t, uint16_t);
    void set_zero_transition(uint16_t);
    void set_values(MoveBoard &);
    uint8_t get_strength();
    uint16_t get_transition(uint16_t, uint8_t);
    uint8_t get_direction(uint16_t, uint8_t);
    std::bitset<20000> &get_transitions_to_remove(uint16_t);
    std::bitset<2501> &get_fields_to_remove(uint16_t);
    void decrement_bombs(uint8_t);
    void print();
    void get_bomb_coords(uint16_t, uint16_t, uint8_t);
    void init_bomb_phase_boards();

private:
    std::vector<std::bitset<2501>> m_fields_to_remove;
    std::vector<std::bitset<20000>> m_transitions_to_remove;
    std::vector<uint16_t> m_transitions;
    uint8_t m_strength;
};

#endif // BOMB_BOARD_HPP