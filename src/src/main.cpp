#include <cstring>
#include <fstream>
#include <iostream>
#include <string>

#include "logging.hpp"
#include "game.hpp"
#include "network.hpp"

void init_print(bool quietMode, const char *ip, int port, bool sorting)
{
    LOG_INFO("Group01 | Dräxl, Koch, Kuhn");
    if (sorting)
        LOG_INFO("Move ordering active");

    else
        LOG_INFO("Move ordering disabled");

    if (quietMode)
        LOG_INFO("Quiet mode active");

    if (ip != nullptr)
        LOG_INFO(std::string("IP-Adress: ") + ip);

    if (port != -1)
        LOG_INFO("Port: " + std::to_string(port));

    if (quietMode)
    {
        std::ofstream devnull;
        devnull.open("/dev/null");
        std::cout.rdbuf(devnull.rdbuf());
    }
}

/// @brief argparser which reacts to set parameters
/// @param argc
/// @param argv
/// @param ip ip we're connected to
/// @param port port we're connected to
/// @param sorting disables sorting if it's parameter is set - default is enabled
/// @param quietMode enables quietmode if it's parameter is set - default is disabled
/// @return
bool read_args(int argc, char *argv[], const char *&ip, int &port, bool &sorting, bool &quietMode)
{
    for (int i = 1; i < argc; i++)
    {
        std::string arg(argv[i]);
        if (arg.find('-') == 0)
        {
            std::string key = arg.substr(1);
            if (key == "i" && (argv[i + 1] == std::string("localhost")))
                ++i;

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
                sorting = false;

            else if (key == "q")
                quietMode = false;

            else if (key == "h")
            {
                LOG_INFO("Parameter:\n"
                         "-i <ip>: Enter server ip\n"
                         "-p <port>: Enter server port\n"
                         "-n: Disable move sorting\n"
                         "-q: Disable console output (quiet mode)\n"
                         "-h: Print help\n"
                         "Standard values:\n"
                         "Move sorting: " +
                         std::string(sorting ? "enabled" : "disabled") + "\n"
                                                                         "Quiet mode: " +
                         std::string(quietMode ? "enabled" : "disabled"));
                return true;
            }
        }
    }
    return false;
}

int main(int argc, char *argv[])
{
    const char *ip = "127.0.0.1\0";
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

    LOG_INFO("Terminated without errors");
    return 0;
}
