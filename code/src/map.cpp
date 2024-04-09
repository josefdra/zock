#include "map.hpp"

/**
 * @brief map.cpp is responsible for reading in the map information and the correct output as well as calculating the correct neighbourhood relationships
 *
 */

Map::Map(){};
Map::~Map(){};

/**
 * @brief Calculation of all possible transitions and their coordinates depending on their direction
 *
 * @param n current coordinate
 */
void Map::check_neighbours(uint16_t n)
{
    n > width &&all_map_moves[n - width].symbol != '-' ? all_map_moves[n].transitions[0] = (n - width) * 10 + 4 : all_map_moves[n].transitions[0] = 0;
    n % width != 0 && n > width &&all_map_moves[n - width + 1].symbol != '-' ? all_map_moves[n].transitions[1] = (n - width + 1) * 10 + 5 : all_map_moves[n].transitions[1] = 0;
    n % width != 0 && all_map_moves[n + 1].symbol != '-' ? all_map_moves[n].transitions[2] = (n + 1) * 10 + 6 : all_map_moves[n].transitions[2] = 0;
    n % width != 0 && n <= width *(height - 1) && all_map_moves[n + width + 1].symbol != '-' ? all_map_moves[n].transitions[3] = (n + width + 1) * 10 + 7 : all_map_moves[n].transitions[3] = 0;
    n <= width *(height - 1) && all_map_moves[n + width].symbol != '-' ? all_map_moves[n].transitions[4] = (n + width) * 10 + 0 : all_map_moves[n].transitions[4] = 0;
    n % width != 1 && n <= width *(height - 1) && all_map_moves[n + width - 1].symbol != '-' ? all_map_moves[n].transitions[5] = (n + width - 1) * 10 + 1 : all_map_moves[n].transitions[5] = 0;
    n % width != 1 && all_map_moves[n - 1].symbol != '-' ? all_map_moves[n].transitions[6] = (n - 1) * 10 + 2 : all_map_moves[n].transitions[6] = 0;
    n % width != 1 && n > width &&all_map_moves[n - width - 1].symbol != '-' ? all_map_moves[n].transitions[7] = (n - width - 1) * 10 + 3 : all_map_moves[n].transitions[7] = 0;
}

/**
 * @brief reads the input and sets all the information required for the game map
 *
 * @param inputfile mapfile to read
 */
void Map::read_hash_map(const std::string inputfile)
{
    h_res_clock::time_point start_time = h_res_clock::now();
    std::ifstream inputFile(inputfile);
    std::stringstream mapfile;
    mapfile << inputFile.rdbuf();
    // 65000 is set to check for end of file
    mapfile << 65000;
    inputFile.close();
    mapfile >> spielerzahl >> ueberschreibsteine >> bomben >> staerke >> height >> width;
    hash_map_element elem;
    // every coordinate gets a symbol and it's neighbours are being set
    for (int i = 1; i < (width * height + 1); i++)
    {
        mapfile >> elem.symbol;
        elem.transitions = {0, 0, 0, 0, 0, 0, 0, 0};
        all_map_moves[i] = elem;
    }
    for (int i = 1; i < (width * height + 1); i++)
    {
        if (all_map_moves[i].symbol != '-')
        {
            check_neighbours(i);
        }
    }
    while (mapfile)
    {
        // special transitions are being set in same format to appear as neighbours
        uint16_t x1, y1, r1, x2, y2, r2, pos1, pos2, pos1r, pos2r = 0;
        unsigned char trash;
        mapfile >> x1 >> y1 >> r1 >> trash >> trash >> trash >> x2 >> y2 >> r2;
        // check if eof is reached
        if (x1 != 65000)
        {
            x1++;
            x2++;
            pos1 = (x1) + (y1)*width;
            pos2 = (x2) + (y2)*width;
            pos1r = pos1 * 10 + r1;
            pos2r = pos2 * 10 + r2;
            all_map_moves[pos1].transitions[r1] = pos2r;
            all_map_moves[pos2].transitions[r2] = pos1r;
        }
    }
    h_res_clock::time_point end_time = h_res_clock::now();
    std::chrono::duration<double, std::micro> elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    std::cout << "Elapsed time (read_hash_map): " << elapsed_time.count() << " microseconds" << std::endl;
}

/**
 * @brief prints every transition and the character of each coordinate
 */
void Map::print_transitions()
{
    std::cout << std::endl;
    for (int y = 0; y < height; y++)
    {
        for (int n = 1; n < width + 1; n++)
        {
            int x = width * y + n;
            std::cout << std::setw(3) << all_map_moves[x].transitions[7] << " ";
            std::cout << std::setw(3) << all_map_moves[x].transitions[0] << " ";
            std::cout << std::setw(3) << all_map_moves[x].transitions[1] << " ";
            std::cout << "  ";
        }
        std::cout << std::endl;
        for (int n = 1; n < width + 1; n++)
        {
            int x = width * y + n;
            std::cout << std::setw(3) << all_map_moves[x].transitions[6] << " ";
            std::cout << std::setw(3) << all_map_moves[x].symbol << " ";
            std::cout << std::setw(3) << all_map_moves[x].transitions[2] << " ";
            std::cout << "  ";
        }
        std::cout << std::endl;
        for (int n = 1; n < width + 1; n++)
        {
            int x = width * y + n;
            std::cout << std::setw(3) << all_map_moves[x].transitions[5] << " ";
            std::cout << std::setw(3) << all_map_moves[x].transitions[4] << " ";
            std::cout << std::setw(3) << all_map_moves[x].transitions[3] << " ";
            std::cout << "  ";
        }
        std::cout << std::endl;
        std::cout << std::endl;
    }
}

/**
 * @brief prints the map with transitions
 */
void Map::print_map_with_transitions()
{
    std::cout << spielerzahl << std::endl;
    std::cout << ueberschreibsteine << std::endl;
    std::cout << bomben << " " << staerke << std::endl;
    std::cout << height << " " << width << std::endl;
    for (int i = 1; i < (width * height + 1); i++)
    {
        std::cout << all_map_moves[i].symbol << " ";
        if (i % width == 0)
        {
            std::cout << std::endl;
        }
    }
    print_transitions();
}

/**
 * @brief prints the map without transitions
 */
void Map::print_map()
{
    std::cout << spielerzahl << std::endl;
    std::cout << ueberschreibsteine << std::endl;
    std::cout << bomben << " " << staerke << std::endl;
    std::cout << height << " " << width << std::endl;
    for (int i = 1; i < (width * height + 1); i++)
    {
        std::cout << all_map_moves[i].symbol << " ";
        if (i % width == 0)
        {
            std::cout << std::endl;
        }
    }
}