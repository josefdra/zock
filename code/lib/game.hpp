#ifndef GAME_H
#define GAME_H

#include <random>
#include <chrono>

#include "map.hpp"
#include "player.hpp"
#include "process_moves.hpp"
#include "helper.hpp"

class Game
{
public:
    Game(const std::string);
    ~Game();
    std::string m_map_name;
    Map m_map;
    std::vector<Player> m_players;
    void run();
    void move(uint16_t);
    void determine_winner();

private:
};

#endif // GAME_H