#include "map.hpp"
#include "helper.hpp"
#include "player.hpp"

/**
 * @brief map.cpp is responsible for reading in the map information and the correct output as well as calculating the correct neighbourhood relationships
 *
 */

Map::Map(std::string map_name)
{
    read_hash_map(map_name);
};
Map::~Map(){};

/**
 * @brief Calculation of all possible transitions and their coordinates depending on their direction
 *
 * @param n current coordinate
 */
void Map::check_neighbours(uint16_t n)
{
    n > m_width &&m_symbol_and_transitions[n - m_width].symbol != '-' ? m_symbol_and_transitions[n].transitions[0] = (n - m_width) * 10 : m_symbol_and_transitions[n].transitions[0] = 0;
    n % m_width != 0 && n > m_width &&m_symbol_and_transitions[n - m_width + 1].symbol != '-' ? m_symbol_and_transitions[n].transitions[1] = (n - m_width + 1) * 10 + 1 : m_symbol_and_transitions[n].transitions[1] = 0;
    n % m_width != 0 && m_symbol_and_transitions[n + 1].symbol != '-' ? m_symbol_and_transitions[n].transitions[2] = (n + 1) * 10 + 2 : m_symbol_and_transitions[n].transitions[2] = 0;
    n % m_width != 0 && n <= m_width *(m_height - 1) && m_symbol_and_transitions[n + m_width + 1].symbol != '-' ? m_symbol_and_transitions[n].transitions[3] = (n + m_width + 1) * 10 + 3 : m_symbol_and_transitions[n].transitions[3] = 0;
    n <= m_width *(m_height - 1) && m_symbol_and_transitions[n + m_width].symbol != '-' ? m_symbol_and_transitions[n].transitions[4] = (n + m_width) * 10 + 4 : m_symbol_and_transitions[n].transitions[4] = 0;
    n % m_width != 1 && n <= m_width *(m_height - 1) && m_symbol_and_transitions[n + m_width - 1].symbol != '-' ? m_symbol_and_transitions[n].transitions[5] = (n + m_width - 1) * 10 + 5 : m_symbol_and_transitions[n].transitions[5] = 0;
    n % m_width != 1 && m_symbol_and_transitions[n - 1].symbol != '-' ? m_symbol_and_transitions[n].transitions[6] = (n - 1) * 10 + 6 : m_symbol_and_transitions[n].transitions[6] = 0;
    n % m_width != 1 && n > m_width &&m_symbol_and_transitions[n - m_width - 1].symbol != '-' ? m_symbol_and_transitions[n].transitions[7] = (n - m_width - 1) * 10 + 7 : m_symbol_and_transitions[n].transitions[7] = 0;
}

/**
 * @brief reads the input and sets all the information required for the game map
 *
 * @param inputfile mapfile to read
 */
void Map::read_hash_map(const std::string map_name)
{
    std::ifstream inputFile(map_name);
    std::stringstream mapfile;
    m_hash_map_element elem;
    mapfile << inputFile.rdbuf();
    // 65000 is set to check for end of file
    mapfile << '\n'
            << 65000;
    inputFile.close();
    mapfile >> m_player_count >> m_initial_overwrite_stones >> m_initial_bombs >> m_strength >> m_height >> m_width;
    // every coordinate gets a symbol and it's neighbours are being set
    for (int i = 1; i < (m_width * m_height + 1); i++)
    {
        mapfile >> elem.symbol;
        elem.transitions = {0, 0, 0, 0, 0, 0, 0, 0};
        m_symbol_and_transitions[i] = elem;
    }
    for (int i = 1; i < (m_width * m_height + 1); i++)
    {
        if (m_symbol_and_transitions[i].symbol != '-')
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
            pos1 = (x1) + (y1)*m_width;
            pos2 = (x2) + (y2)*m_width;
            pos1r = pos1 * 10 + ((r1 + 4) % 8);
            pos2r = pos2 * 10 + ((r2 + 4) % 8);
            m_symbol_and_transitions[pos1].transitions[r1] = pos2r;
            m_symbol_and_transitions[pos2].transitions[r2] = pos1r;
        }
    }
}

/**
 * @brief prints the map with transitions
 */
void Map::print_map_with_transitions()
{
    std::cout << m_player_count << std::endl;
    std::cout << m_initial_overwrite_stones << std::endl;
    std::cout << m_initial_bombs << " " << m_strength << std::endl;
    std::cout << m_height << " " << m_width << std::endl;
    std::cout << std::endl;
    for (int y = 0; y < m_height; y++)
    {
        for (int n = 1; n < m_width + 1; n++)
        {
            int x = m_width * y + n;
            std::cout << std::setw(3) << m_symbol_and_transitions[x].transitions[7] << " ";
            std::cout << std::setw(3) << m_symbol_and_transitions[x].transitions[0] << " ";
            std::cout << std::setw(3) << m_symbol_and_transitions[x].transitions[1] << " ";
            std::cout << "  ";
        }
        std::cout << std::endl;
        for (int n = 1; n < m_width + 1; n++)
        {
            int x = m_width * y + n;
            std::cout << std::setw(3) << m_symbol_and_transitions[x].transitions[6] << " ";
            if (check_players(m_symbol_and_transitions[x].symbol))
            {
                std::cout << getColorString(Colors((m_symbol_and_transitions[x].symbol - '0'))) << std::setw(3) << m_symbol_and_transitions[x].symbol << " "
                          << "\e[0m";
            }
            else
            {
                std::cout << std::setw(3) << m_symbol_and_transitions[x].symbol << " ";
            }
            std::cout << std::setw(3) << m_symbol_and_transitions[x].transitions[2] << " ";
            std::cout << "  ";
        }
        std::cout << std::endl;
        for (int n = 1; n < m_width + 1; n++)
        {
            int x = m_width * y + n;
            std::cout << std::setw(3) << m_symbol_and_transitions[x].transitions[5] << " ";
            std::cout << std::setw(3) << m_symbol_and_transitions[x].transitions[4] << " ";
            std::cout << std::setw(3) << m_symbol_and_transitions[x].transitions[3] << " ";
            std::cout << "  ";
        }
        std::cout << std::endl;
        std::cout << std::endl;
    }
}

/**
 * @brief prints the map without transitions but with spectifications
 */
void Map::print_map_with_spectifications()
{
    std::cout << m_player_count << std::endl;
    std::cout << m_initial_overwrite_stones << std::endl;
    std::cout << m_initial_bombs << " " << m_strength << std::endl;
    std::cout << m_height << " " << m_width << std::endl;
    for (int i = 1; i < (m_width * m_height + 1); i++)
    {
        if (check_players(m_symbol_and_transitions[i].symbol))
        {
            std::cout << getColorString(Colors((m_symbol_and_transitions[i].symbol - '0'))) << std::setw(3) << m_symbol_and_transitions[i].symbol << " "
                      << "\e[0m";
        }
        else
        {
            std::cout << std::setw(3) << m_symbol_and_transitions[i].symbol << " ";
        }
        if (i % m_width == 0)
        {
            std::cout << std::endl;
        }
    }
}

/**
 * @brief prints the map without transitions and without specifications
 */
void Map::print_map()
{
    for (int i = 1; i < (m_width * m_height + 1); i++)
    {
        if (check_players(m_symbol_and_transitions[i].symbol))
        {
            std::cout << getColorString(Colors((m_symbol_and_transitions[i].symbol - '0'))) << std::setw(3) << m_symbol_and_transitions[i].symbol << " "
                      << "\e[0m";
        }
        else
        {
            std::cout << std::setw(3) << m_symbol_and_transitions[i].symbol << " ";
        }
        if (i % m_width == 0)
        {
            std::cout << std::endl;
        }
    }
    std::cout << std::endl;
}

void Map::check_corners(std::vector<Player> &p)
{
    m_map_corners.clear();
    for (auto &player : p)
    {
        player.m_player_corners.clear();
        for (uint16_t i = 1; i < (m_width * m_height + 1); i++)
        {
            if (check_empty_fields(m_symbol_and_transitions[i].symbol) || m_symbol_and_transitions[i].symbol == player.m_symbol)
            {
                bool corner = false;
                std::vector<uint16_t> transitions;
                for (uint16_t j = 0; j < 8; j++)
                {
                    if (m_symbol_and_transitions[i].transitions[j] != 0 && m_symbol_and_transitions[m_symbol_and_transitions[i].transitions[j] / 10].symbol != player.m_symbol)
                    {
                        transitions.push_back(j);
                    }
                }
                if (transitions.size() < 5)
                {
                    corner = true;
                    for (auto &t : transitions)
                    {
                        if (m_symbol_and_transitions[i].transitions[(t + 4) % 8] != 0 && m_symbol_and_transitions[m_symbol_and_transitions[i].transitions[(t + 4) % 8] / 10].symbol != player.m_symbol)
                        {
                            corner = false;
                        }
                    }
                }
                if (corner)
                {
                    player.m_player_corners.insert(i);
                    m_map_corners.insert(i);
                }
            }
        }
    }
}

void Map::check_before_corners(std::vector<Player> &p)
{
    check_corners(p);
    m_map_before_corners.clear();
    for (auto &coord : m_map_corners)
    {
        for (uint16_t i = 0; i < NUM_OF_DIRECTIONS; i++)
        {
            if (check_empty_fields(m_symbol_and_transitions[m_symbol_and_transitions[coord].transitions[i] / 10].symbol))
            {
                m_map_before_corners.insert(m_symbol_and_transitions[coord].transitions[i] / 10);
            }
        }
    }
}

void Map::check_before_before_corners(std::vector<Player> &p)
{
    check_before_corners(p);
    m_map_before_before_corners.clear();
    for (auto &coord : m_map_before_corners)
    {
        for (uint16_t i = 0; i < NUM_OF_DIRECTIONS; i++)
        {
            if (check_empty_fields(m_symbol_and_transitions[m_symbol_and_transitions[coord].transitions[i] / 10].symbol))
            {
                m_map_before_before_corners.insert(m_symbol_and_transitions[coord].transitions[i] / 10);
            }
        }
    }
}