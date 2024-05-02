#ifndef GAME_HPP
#define GAME_HPP

#include <random>
#include <chrono>

#include "map.hpp"
#include "player.hpp"
#include "algorithms.hpp"
#include "helper.hpp"
#include "process_moves.hpp"

class Game
{
public:
    Game();
    ~Game();
    void init_map(std::stringstream &);
    void init_players();
    uint16_t get_turn(uint8_t &, uint8_t &, uint8_t &);
    uint16_t get_bomb_throw();   
    void check_winner();
    // end coord          //special field(0-4)       //coords of the way

    Map m_map;
    std::vector<Player> m_players;
    uint8_t m_player_number;
    uint8_t m_winner = 0;
};

#endif // GAME_HPP
