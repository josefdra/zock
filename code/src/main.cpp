#include "map.hpp"
#include "transition_gen.hpp"

int main()
{
    Map map;
    map.read_hash_map("../../maps/boeseMap03.map");
    map.print_map();
    map.process_moves();
    //transition_generate(map);
    return 1;
}
