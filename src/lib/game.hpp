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
    Game(const std::string &, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);
    ~Game();
    void init_map(const std::string &);
    void init_players(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);
    bool run();
    uint16_t get_turn(uint8_t &, Player &);
    uint8_t check_winner();
    Map m_map;
    std::vector<Player> m_players;
    uint8_t m_player_number;
    uint8_t m_winner = 0;
    uint8_t m_choice_value = 0;
    uint8_t m_spec = 0;
    uint8_t m_pos;
};

#endif // GAME_HPP
