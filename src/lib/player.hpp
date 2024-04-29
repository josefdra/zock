#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <stdint.h>
#include <unordered_set>
#include <unordered_map>
#include <string>
#include <iostream>
#include <vector>

#include "helper.hpp"
// forward definition to prevent include errors
class Map;

class Player
{
public:
    Player();
    ~Player();
    void init(uint16_t, uint16_t, char);
    bool has_overwrite_stones();
    bool has_bombs();
    unsigned char m_symbol;
    uint16_t m_overwrite_stones = 0;
    uint16_t m_bombs = 0;
    uint16_t m_points = 0;
    bool m_has_valid_moves = false;
    uint16_t m_frontier_score = 0;
    int m_board_value;
    std::unordered_map<uint16_t, std::vector<char>> m_valid_moves;

private:
};

#endif // PLAYER_HPP