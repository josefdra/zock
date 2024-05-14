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

        Network network_handler("127.0.0.1", 7777, 1, atoi(argv[4]), atoi(argv[5]), atoi(argv[6]));
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
    bool sorting = true;
    bool quietMode = false;
    for (int i = 1; i < argc; ++i)
    {
        if (std::strcmp(argv[i], "-n") == 0)
        {
            sorting = false;
        }
        else if (std::strcmp(argv[i], "-q") == 0)
        {
            quietMode = true;
        }
        else if (std::strcmp(argv[i], "-h") == 0)
        {
            std::cout << "Parameter:\n"
                         "-i <ip>: Eingabe IP von Server\n"
                         "-p <port: Eingabe Port von Server\n"
                         "-n: Zugsortierung abschalten\n"
                         "-q: alle Console-Ausgaben unterdrücken (quiet mode)\n"
                         "-h: Ausgabe der Hilfe\n"
                         "Standardwerte:\n" 
                         "Zugsortierung: "
                      << (sorting ? "aktiviert" : "deaktiviert") << "\n"
                                                                    "Quiet mode: "
                      << (quietMode ? "aktiviert" : "deaktiviert") << std::endl;
            return 0;
        }
        else if (std::strcmp(argv[i], "-i") == 0)
        {
            if (i + 1 < argc)
            {
                if (std::strcmp(argv[i + 1], "-p") != 0 && std::strcmp(argv[i + 1], "-n") != 0 && std::strcmp(argv[i + 1], "-q") != 0 && std::strcmp(argv[i + 1], "-h") != 0)
                {
                    ip = argv[i + 1];
                    ++i;
                }
                else
                {
                    std::cout << "Invalid value for -i" << std::endl;
                }
            }
            else
            {
                std::cout << "IP address missing after -i" << std::endl;
            }
        }
        else if (std::strcmp(argv[i], "-p") == 0)
        {
            if (i + 1 < argc)
            {
                if (std::strcmp(argv[i + 1], "-i") != 0 && std::strcmp(argv[i + 1], "-n") != 0 && std::strcmp(argv[i + 1], "-q") != 0 && std::strcmp(argv[i + 1], "-h") != 0)
                {
                    port = std::atoi(argv[i + 1]);
                    ++i;
                }
                else
                {
                    std::cout << "Invalid value for -p" << std::endl;
                }
            }
            else
            {
                std::cout << "Port number missing after -p" << std::endl;
            }
        }
        else
        {
            std::cout << "Unknown parameter: " << argv[i] << std::endl;
        }
    }

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
    std::cout << "connecting to: ip(" << ip << "), port(" << port << ")" << std::endl;
    Network network_handler(ip, port, group_number, 1, 8, 9, sorting);
    std::cout << "Game finished" << std::endl;
    return 0;
}
#endif