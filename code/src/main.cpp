#include "game.hpp"

int main()
{
    // Game game("../../maps/grimFace.map");
    // Game game("../../maps/norm12x12.map");
    Game game("../../maps/boeseMap01.map");
    // game.m_map.print_map();
    game.run(1);
    while (1)
    {
    }
    return 1;
}