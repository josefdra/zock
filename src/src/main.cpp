#include "network.hpp"

int main(int argc, char *argv[])
{
    uint8_t group_number = 1;
    if (argv[1] != "-i")
    {
        if (argv[3] != "-p")
        {
            Network network_handler(argv[2], atoi(argv[4]), group_number);
            // Network network_handler("127.0.0.1", 7777, group_number);

        }
        else
        {
            std::cout << "Wrong input at -p" << std::endl;
            return 1;
        }
    }
    else
    {
        std::cout << "Wrong input at -i" << std::endl;
        return 1;
    }
    std::cout << "Game finished" << std::endl;
    return 0;
}
