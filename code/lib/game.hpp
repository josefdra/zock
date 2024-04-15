#ifndef GAME_HPP
#define GAME_HPP

#include <random>
#include <chrono>

#include "map.hpp"
#include "player.hpp"
#include "process_moves.hpp"
#include "algorithms.hpp"
#include "helper.hpp"

class Game
{
public:
    Game(const std::string);
    ~Game();
    std::string m_map_name;
    Map m_map;
    std::vector<Player> m_players;
    void determine_winner();
    void calculate_map_value();
    void evaluate_board();
    void run();
    void move(uint16_t);
};

#endif // GAME_HPP