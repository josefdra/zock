#ifndef PLAYER_H
#define PLAYER_H

#include <stdint.h>
#include <unordered_set>
#include <unordered_map>

class Player
{
public:
    Player(char);
    ~Player();
    void set_bombs_and_stones(uint16_t, uint16_t);
    bool has_overwrite_stones();
    bool has_bombs();
    void decrement_overwrite_stone();
    void decrement_bombs();
    char m_player_symbol;
    std::unordered_map<uint16_t, std::tuple<uint16_t, std::unordered_set<uint16_t>>> m_valid_moves;

private:
    uint16_t m_overwrite_stones = 0;
    uint16_t m_bombs = 0;
};

#endif // PLAYER_H