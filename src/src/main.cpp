#include <cstring>
#include <fstream>
#include <iostream>
#include <string>

#include "game.hpp"
#include "network.hpp"

void init_print(bool quietMode, const char *ip, int port, bool sorting)
{
    std::cout << "Gruppe01 | Dräxl, Koch, Kuhn" << std::endl;
    if (sorting)
    {
        std::cout << "Zugsortierung aktiviert" << std::endl;
    }
    else
    {
        std::cout << "Zugsortierung deaktiviert" << std::endl;
    }
    if (quietMode)
    {
        std::cout << "Quiet mode aktiviert" << std::endl;
    }
    if (ip != nullptr)
    {
        std::cout << "IP-Adresse: " << ip << std::endl;
    }
    if (port != -1)
    {
        std::cout << "Port: " << port << std::endl;
    }
    if (quietMode)
    {
        std::ofstream devnull;
        devnull.open("/dev/null");
        std::cout.rdbuf(devnull.rdbuf());
    }
}

bool read_args(int argc, char *argv[], const char *&ip, int &port, bool &sorting, bool &quietMode)
{
    for (int i = 1; i < argc; i++)
    {
        std::string arg(argv[i]);
        if (arg.find('-') == 0)
        {
            std::string key = arg.substr(1);
            if (key == "i" && (argv[i + 1] == std::string("localhost")))
            {
                ++i;
            }
            else if (key == "i" && !(argv[i + 1] == std::string("localhost")))
            {
                ip = argv[i + 1];
                ++i;
            }
            else if (key == "p")
            {
                port = std::atoi(argv[i + 1]);
                ++i;
            }
            else if (key == "n")
            {
                sorting = false;
            }
            else if (key == "q")
            {
                quietMode = false;
            }
            else if (key == "h")
            {
                std::cout << "Parameter:\n"
                             "-i <ip>: Enter server ip\n"
                             "-p <port>: Enter server port\n"
                             "-n: Disable move sorting\n"
                             "-q: Disable console output (quiet mode)\n"
                             "-h: Print help\n"
                             "Standard values:\n"
                             "Move sorting: "
                          << (sorting ? "enabled" : "disabled") << "\n"
                                                                   "Quiet mode: "
                          << (quietMode ? "enabled" : "disabled") << std::endl;
                return true;
            }
        }
    }
    return false;
}

int main(int argc, char *argv[])
{
    const char *ip = "127.0.0.1";
    int port = 7777;
    bool sorting = true;
    bool quietMode = false;

    if (read_args(argc, argv, ip, port, sorting, quietMode))
        return 0;
    init_print(quietMode, ip, port, sorting);

    Network net(ip, port);
    if (!net.init())
        return 1;
    Game game;
    game.run(net, sorting);

    return 0;
}