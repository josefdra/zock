#include "map.hpp"

Map::Map(){};
Map::~Map(){};

void Map::readmap()
{
    std::ifstream inputFile("C:\\Users\\Dell\\Desktop\\Dateien\\Studium\\ZOCK\\g01\\maps\\simple2PL.map");
    std::stringstream mapfile;
    mapfile << inputFile.rdbuf();
    mapfile >> spielerzahl >> ueberschreibsteine >> bomben >> staerke >> bretthoehe >> brettbreite;
    unsigned char temp;
    for (int y = 0; y < bretthoehe; y++)
    {
        std::vector<unsigned char> lines;
        for (int x = 0; x < brettbreite; x++)
        {
            mapfile >> temp;
            lines.push_back(temp);
        }
        map.push_back(lines);
    }
    while (mapfile)
    {
        std::array<unsigned short, 6> transition;
        unsigned char throwaway;
        mapfile >> transition[0] >> transition[1] >> transition[2] >> throwaway >> throwaway >> throwaway >> transition[3] >> transition[4] >> transition[5];
        transitions.push_back(transition);
    }
    transitions.pop_back();
}

void Map::printmap()
{
    std::cout << spielerzahl << std::endl;
    std::cout << ueberschreibsteine << std::endl;
    std::cout << bomben << " " << staerke << std::endl;
    std::cout << bretthoehe << " " << brettbreite << std::endl;
    for (auto line : map)
    {
        for (auto value : line)
        {
            std::cout << value << " ";
        }
        std::cout << std::endl;
    }
    for (auto transition : transitions)
    {
        std::cout << transition[0] << " " << transition[1] << " " << transition[2] << " <-> " << transition[3] << " " << transition[4] << " " << transition[5] << std::endl;
    }
}