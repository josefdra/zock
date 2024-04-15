#include "game.hpp"

int main()
{
    Game game("../../maps/norm12x12test.map");
    game.m_players[1].print_frontiers(game.m_map);
    game.m_map.print_frontier_scores(game.m_players);
    game.m_map.print_map();
    check_moves(game.m_map, game.m_players[1]);
    game.m_players[1].get_moves_score(game.m_map);
    game.run();
    while (1)
    {
    }
    return 1;
}