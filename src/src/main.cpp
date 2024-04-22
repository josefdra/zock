#include "client_connection.hpp"
#include "game.hpp"

int main()
{
    // MAP NAME MUST BE THE SAME AS SERVER MAP
    Game game("../../maps/original.map");
    game.run_network_game();

    while (1)
    {
    }
    return 1;
}
