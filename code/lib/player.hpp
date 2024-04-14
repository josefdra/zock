#ifndef PLAYER_H
#define PLAYER_H

#include <stdint.h>
#include <unordered_set>
#include <unordered_map>
#include <string>
#include <iostream>
#include <vector>

class Player
{
public:
    Player(char, uint16_t, uint16_t);
    ~Player();
    bool has_overwrite_stones();
    bool has_bombs();
    void print_valid_moves(uint16_t);
    std::unordered_map<uint16_t, std::tuple<uint16_t, std::unordered_set<uint16_t>>> m_valid_moves;
    bool m_has_valid_moves = false;
    char m_symbol;
    uint16_t m_overwrite_stones = 0;
    uint16_t m_bombs = 0;
    uint16_t m_points = 0;
    std::unordered_set<uint16_t> m_player_corners;

private:
};

#endif // PLAYER_H