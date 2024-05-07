#include "network.hpp"

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