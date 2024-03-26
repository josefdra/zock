#ifndef MAP_H
#define MAP_H

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <tuple>

class Map
{
public:
    Map();
    ~Map();
    void readmap(const std::string inputfile);
    void printmap();

private:
    uint8_t spielerzahl;
    uint16_t ueberschreibsteine;
    uint16_t bomben;
    uint8_t staerke;
    uint16_t bretthoehe;
    uint16_t brettbreite;
    std::vector<std::vector<unsigned char>> map;
    std::vector<std::tuple<uint16_t, uint16_t, uint16_t, uint16_t, uint16_t, uint16_t>> transitionen;
};

#endif // MAP_H