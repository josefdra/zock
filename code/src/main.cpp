#include "map.hpp"

int main(){
    Map map;
    map.readmap("../../maps/simple2Pl.map");
    map.printmap();
    return 1;
}