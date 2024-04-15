#ifndef GAME_H
#define GAME_H

#include <random>

#include "player.hpp"
#include "map.hpp"
#include "process_moves.hpp"
#include "algorithms.hpp"

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