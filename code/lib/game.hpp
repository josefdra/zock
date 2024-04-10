#ifndef GAME_H
#define GAME_H

#include "player.hpp"
#include "map.hpp"
#include "process_moves.hpp"

class Game
{
public:
    Game(const std::string);
    ~Game();
    std::string m_map_name;
    Map m_map;
    Player m_player1;
    Player m_player2;
    Player m_player3;
    Player m_player4;
    Player m_player5;
    Player m_player6;
    Player m_player7;
    Player m_player8;
    void run();

private:
};

#endif // GAME_H