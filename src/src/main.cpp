#include "network.hpp"
#include <cstdlib>

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
                if (argc > 3)
                {
                    if (std::strcmp(argv[3], "-p") == 0)
                    {
                        if (argc > 4)
                        {
                            if (std::strcmp(argv[4], "7777") != 0)
                            {
                                port = atoi(argv[4]);
                            }
                        }
                        else
                        {
                            std::cout << "-p argument missing" << std::endl;
                        }
                    }
                }
            }
            else
            {
                std::cout << "-i argument missing" << std::endl;
            }
        }
        {
            if (argc > 2)
            {
                if (std::strcmp(argv[2], "7777") != 0)
                {
                    port = atoi(argv[2]);
                }
                if (argc > 3)
                {
                    if (std::strcmp(argv[3], "-i") == 0)
                    {
                        if (argc > 4)
                        {
                            if (std::strcmp(argv[4], "localhost") != 0 && std::strcmp(argv[4], "127.0.0.1") != 0)
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
        {
            std::cout << "No valid input. Starting with standard configuration" << std::endl;
        }
    }
    else
    {
        std::cout << "No valid input. Starting with standard configuration" << std::endl;
    }
    return 0;
}

/*
int main(int argc, char *argv[])
{
    if (argc > 1)
    {
        std::string filename = "/home/josefdra/ZOCK/g01/automated_testing/client_binary/logs/game_" + std::string(argv[2]) + "_client_" + std::string(argv[1]) + ".txt";

        freopen(filename.c_str(), "w", stdout);

        Network network_handler("127.0.0.1", 7777, 1);
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
*/