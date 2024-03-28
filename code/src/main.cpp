#include "map.hpp"
#include "transition_gen.hpp"

int main(){
    Map map;
    map.read_hash_map("../../maps/simple2Pl.map");
    map.print_map();
    generate(std::ref(map));
    return 1;
}