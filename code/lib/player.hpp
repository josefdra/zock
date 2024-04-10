#ifndef PLAYER_H
#define PLAYER_H

#include <stdint.h>
#include <unordered_set>
#include <unordered_map>
#include <string>

class Player
{
public:
    Player(char, uint16_t, uint16_t);
    ~Player();
    bool has_valid_moves();
    void set_valid_moves(bool);
    bool has_overwrite_stones();
    bool has_bombs();
    void decrement_overwrite_stone();
    void decrement_bombs();
    char get_symbol();
    std::unordered_map<uint16_t, std::tuple<uint16_t, std::unordered_set<uint16_t>>> m_valid_moves;

private:
    bool m_has_valid_moves = false;
    char m_symbol;
    uint16_t m_overwrite_stones = 0;
    uint16_t m_bombs = 0;
};

#endif // PLAYER_H