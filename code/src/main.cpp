#include "map.hpp"
#include "transition_gen.hpp"
#include "check_valid_moves.hpp"

int main()
{
    Map map;
    // map.read_hash_map("../../maps/simple2Pl.map");
    // transition_generate(map);
    // std::random_device rd;
    // std::mt19937 gen(rd());
    // std::uniform_int_distribution<> dis(1, (map.spielerzahl + 1));
    // uint16_t players_turn = dis(gen);
    // std::string input;
    // uint16_t x, y, xy;
    // while (1)
    // {
    //     map.print_map();
    //     std::cout << "Player " << players_turn << " it's your turn. Please enter coordinates to place stone (Format: 1 3): ";
    //     getline(std::cin, input);
    //     std::stringstream ss(input);
    //     ss >> x >> y;
    //     if (ss.fail())
    //     {
    //         std::cout << "Error: Invalid input. Please enter two coordinates separated by a space." << std::endl;
    //     }
    //     xy = (x + 1) + y * map.width;
    //     check_valid_moves(map, xy);
    //     players_turn = (players_turn + 1) % map.spielerzahl;
    // }
    // map.print_map();
    // map.print_map_with_transitions();
    while (1)
    {
        map.read_hash_map("../../maps/boeseMap03.map");
        map.print_map();
        process_moves(map);
    }
    // transition_generate(map);
    return 1;
}
