#include "map.hpp"
#include "transition_gen.hpp"

int main()
{
    Map map;
    map.read_hash_map("../../maps/evenMoreTransitions.map");
    map.print_map();
    tranition_generate(map);
    export_hash_map(map);
    return 1;
}