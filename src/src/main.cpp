#include "network.hpp"
#include <cstdlib>

#ifdef TESTING
int main(int argc, char *argv[])
{
    std::cout << "starting with testing configuration" << std::endl;
    if (argc > 1)
    {
        std::string filename = "/home/josefdra/ZOCK/g01/automated_testing/client_binary/logs/game_" + std::string(argv[2]) + "_client_" + std::string(argv[1]) + "_position_" + std::string(argv[3]) + ".txt";

        freopen(filename.c_str(), "w", stdout);

        Network network_handler("127.0.0.1", atoi(argv[7]), 1, atoi(argv[4]), atoi(argv[5]), atoi(argv[6]));
        std::cout << "Game finished" << std::endl;
        fclose(stdout);
        if (network_handler.m_game.m_winner != 2)
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
#else
int main(int argc, char *argv[])
{
    const char *ip = "127.0.0.1";
    int port = 7777;
    uint8_t group_number = 1;
    if (argc > 1)
    {
        if (std::strcmp(argv[1], "-i") == 0)
        {
            if (argc > 2)
            {
                if (std::strcmp(argv[2], "localhost") != 0 && std::strcmp(argv[2], "127.0.0.1") != 0)
                {
                    ip = argv[2];
                }
            }
            else
            {
                std::cout << "-i argument missing" << std::endl;
            }
        }
        if (argc > 3)
        {
            if (std::strcmp(argv[3], "-p") == 0)
            {
                if (argc > 4)
                {
                    port = atoi(argv[4]);
                }
                else
                {
                    std::cout << "-p argument missing" << std::endl;
                }
            }
        }
        else if (std::strcmp(argv[1], "-p") == 0)
        {
            if (argc > 2)
            {
                port = atoi(argv[2]);
                if (argc > 3)
                {
                    if (std::strcmp(argv[3], "-i") == 0)
                    {
                        if (argc > 4)
                        {
                            if (std::strcmp(argv[2], "localhost") != 0 && std::strcmp(argv[2], "127.0.0.1") != 0)
                            {
                                ip = argv[4];
                            }
                        }
                        else
                        {
                            std::cout << "-i argument missing" << std::endl;
                        }
                    }
                }
            }
            else
            {
                std::cout << "-p argument missing" << std::endl;
            }
        }
    }
    else
    {
        std::cout << "No valid input. Starting with standard configuration" << std::endl;
    }
    std::cout << "connecting to: ip(" << ip << "), port(" << port << ")" << std::endl;
    Network network_handler(ip, port, group_number, 1, 1, 1);
    std::cout << "Game finished" << std::endl;
    return 0;
}
#endif