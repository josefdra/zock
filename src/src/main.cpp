#include "network.hpp"

int main(int argc, char *argv[])
{
    std::string filename = "./logs/" + std::string(argv[1]) + ".txt";

    freopen(filename.c_str(), "w", stdout);

    Network network_handler("127.0.0.1", 7777, 1);
    network_handler.run_game();
    std::cout << "Game finished" << std::endl;
    return 0;
}
