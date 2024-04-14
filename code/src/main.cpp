#include "game.hpp"

int main()
{
    Game game("../../maps/norm12x12.map");
    // game.m_players[1].print_frontiers(game.m_map);
    game.m_map.print_map();
    // game.run();
    while (1)
    {
    }
    return 1;
}