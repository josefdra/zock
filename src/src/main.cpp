#include "network.hpp"
#include <cstdlib>

int main(int argc, char *argv[])
{
    if (argc > 1)
    {
        std::string filename = "/home/josefdra/ZOCK/g01/automated_testing/client_binary/logs/game_" + std::string(argv[2]) + "_client_" + std::string(argv[1]) + ".txt";

        freopen(filename.c_str(), "w", stdout);

        Network network_handler("127.0.0.1", 7777, 1);
        std::cout << "Game finished" << std::endl;
        fclose(stdout);
        // change to != 2
        if (network_handler.m_game.m_winner != 1)
        {
            std::string command = "rm " + filename;
            system(command.c_str());
        }
        // lost
        if (network_handler.m_game.m_winner == 0)
        {
            return 0;
        }
        // won
        else if (network_handler.m_game.m_winner == 1)
        {
            return 1;
        }
        // disqulified
        else if (network_handler.m_game.m_winner == 2)
        {
            return 2;
        }
    }
    // undefined
    else
    {
        return 3;
    }
}
