#include "map.hpp"

Map::Map(){};
Map::~Map(){};

void Map::readmap(const std::string inputfile)
{
    std::ifstream inputFile(inputfile);
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
        uint16_t x1, y1, r1, x2, y2, r2 = 0;
        unsigned char muelleimer;
        mapfile >> x1 >> y1 >> r1 >> muelleimer >> muelleimer >> muelleimer >> x2 >> y2 >> r2;
        transitionen.emplace_back(x1, y1, r1, x2, y2, r2);
    }
    transitionen.pop_back();
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
    for (auto transition : transitionen)
    {
        uint16_t x1, y1, r1, x2, y2, r2;
        std::tie(x1, y1, r1, x2, y2, r2) = transition;

        std::cout << x1 << " " << y1 << " " << r1 << " <-> " << x2 << " " << y2 << " " << r2 << std::endl;
    }
}