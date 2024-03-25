#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>
#include <array>

class Map
{
public:
    Map();
    ~Map();
    void readmap();
    void printmap();

private:
    unsigned short spielerzahl;
    unsigned short ueberschreibsteine;
    unsigned short bomben;
    unsigned short staerke;
    unsigned short bretthoehe;
    unsigned short brettbreite;
    std::vector<std::vector<unsigned char>> map;
    std::vector<std::array<unsigned short, 6>> transitions;
};
