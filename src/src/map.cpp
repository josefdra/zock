#include "map.hpp"
#include "helper.hpp"
#include "player.hpp"

/**
 * @brief map.cpp is responsible for reading in the map information and the correct output as well as calculating the correct neighbourhood relationships
 *
 */

Map::Map(){
    m_constant_board_values = std::vector<int>(m_num_of_fields);
    m_variable_board_values = std::vector<int>(m_num_of_fields);
};

Map::~Map(){};

/**
 * @brief Calculation of all possible transitions and their coordinates depending on their direction
 *
 * @param n current coordinate
 */
void Map::check_neighbours(uint16_t c)
{
    if (c > m_width && get_symbol(c - m_width) != '-') // checks if there is a field above
    {
        set_transition(c, 0, (c - m_width) * 10);
    }
    if (c % m_width != 0 && c > m_width && get_symbol(c - m_width + 1) != '-') // checks if there is a field top right
    {
        set_transition(c, 1, (c - m_width + 1) * 10 + 1);
    }
    if (c % m_width != 0 && get_symbol(c + 1) != '-') // checks if there is a field to the right
    {
        set_transition(c, 2, (c + 1) * 10 + 2);
    }
    if (c % m_width != 0 && c <= m_width * (m_height - 1) && get_symbol(c + m_width + 1) != '-') // checks if there is a field bottom right
    {
        set_transition(c, 3, (c + m_width + 1) * 10 + 3);
    }
    if (c <= m_width * (m_height - 1) && get_symbol(c + m_width) != '-') // checks if there is a field below
    {
        set_transition(c, 4, (c + m_width) * 10 + 4);
    }
    if (c % m_width != 1 && c <= m_width * (m_height - 1) && get_symbol(c + m_width - 1) != '-') // check if there is a field bottom left
    {
        set_transition(c, 5, (c + m_width - 1) * 10 + 5);
    }
    if (c % m_width != 1 && get_symbol(c - 1) != '-') // checks if there is a field to the left
    {
        set_transition(c, 6, (c - 1) * 10 + 6);
    }
    if (c % m_width != 1 && c > m_width && get_symbol(c - m_width - 1) != '-') // checks if there is a field top left
    {
        set_transition(c, 7, (c - m_width - 1) * 10 + 7);
    }
}

void Map::set_symbol(uint16_t c, unsigned char s)
{
    m_symbols[c] = s;
}

char Map::get_symbol(uint16_t c)
{
    return m_symbols[c];
}

void Map::set_transition(uint16_t c, uint8_t d, uint16_t t)
{
    m_transitions[(c - 1) * 8 + d] = t;
}

uint16_t Map::get_transition(uint16_t c, uint8_t d)
{
    return m_transitions[(c - 1) * 8 + d] / 10;
}

uint8_t Map::get_direction(uint16_t c, uint8_t d)
{
    return m_transitions[(c - 1) * 8 + d] % 10;
}

void Map::calculate_board_values()
{
    std::vector<std::unordered_set<uint16_t>> board_values;
    std::unordered_set<uint16_t> temp;
    uint8_t counter = 0;
    do
    {
        temp.clear();
        // Walls
        if (counter == 0)
        {
            for (uint16_t c = 1; c < m_num_of_fields; c++)
            {
                for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d++)
                {
                    if (get_transition(c, d) == 0 && m_symbols[c] != '-')
                    {
                        temp.insert(c);
                        break;
                    }
                }
            }
        }
        // One step inside from walls
        else if (counter == 1)
        {
            for (auto &c : board_values[counter - 1])
            {
                for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d++)
                {
                    uint16_t trans = get_transition(c, (d + 4) % NUM_OF_DIRECTIONS);
                    if (get_transition(c, d) == 0 && board_values[counter - 1].find(trans) == board_values[counter - 1].end() && trans != 0)
                    {
                        temp.insert(trans);
                    }
                }
            }
        }
        // One more step from previous step
        else
        {
            for (auto &c : board_values[counter - 1])
            {
                for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d++)
                {
                    uint16_t trans = get_transition(c, (d + 4) % NUM_OF_DIRECTIONS);
                    if (board_values[counter - 2].find(get_transition(c, d)) != board_values[counter - 2].end() && board_values[counter - 1].find(trans) == board_values[counter - 1].end() && board_values[counter - 2].find(trans) == board_values[counter - 2].end())
                    {
                        temp.insert(trans);
                    }
                }
            }
        }
        board_values.push_back(temp);
        counter++;
    } while (temp.size() > 0);
    print_map();
    uint8_t a = board_values.size() - 2;
    for (auto &set : board_values)
    {
        for (auto &c : set)
        {
            if (a == board_values.size() - 2)
            {
                m_constant_board_values[c] = a * 10;
            }
            else if (a == board_values.size() - 3)
            {
                m_constant_board_values[c] = a * -10;
            }
            else
            {
                m_constant_board_values[c] = a * 10;
            }
        }
        a--;
    }
    std::array<int, 9> wall_values{0, 1, 2, 3, 8, 6, 4, 2, 0};
    for (auto &c : board_values[0])
    {
        uint8_t counter = 0;
        for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d++)
        {
            if (get_transition(c, d) == 0)
            {
                counter++;
            }
        }
        m_constant_board_values[c] *= wall_values[counter];
    }
    for (auto &c : board_values[1])
    {
        uint8_t counter = 0;
        for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d++)
        {
            if (board_values[0].find(get_transition(c, d)) != board_values[0].end())
            {
                counter++;
            }
        }
        m_constant_board_values[c] *= wall_values[counter];
    }
}

/**
 * @brief reads the input and sets all the information required for the game map
 *
 * @param inputfile mapfile to read
 */
void Map::read_hash_map(std::stringstream &mapfile)
{
    m_symbols.push_back(0);
    m_transitions.push_back(0);
    char temp;
    mapfile >> m_player_count >> m_initial_overwrite_stones >> m_initial_bombs >> m_strength >> m_height >> m_width;
    m_num_of_fields = m_height * m_width + 1;
    // every coordinate gets a symbol and it's neighbours are being set
    for (int c = 1; c < m_num_of_fields; c++)
    {
        for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d++)
        {
            m_transitions.push_back(0);
            set_transition(c, d, 0);
        }
        mapfile >> temp;
        m_symbols.push_back(0);
        set_symbol(c, temp);
    }
    for (int c = 1; c < m_num_of_fields; c++)
    {
        if (get_symbol(c) != '-')
        {
            check_neighbours(c);
        }
    }
    uint16_t x1, y1, r1, x2, y2, r2, pos1, pos2, pos1r, pos2r;
    while (mapfile >> x1)
    {
        // special transitions are being set in same format to appear as neighbours
        unsigned char trash;
        mapfile >> y1 >> r1 >> trash >> trash >> trash >> x2 >> y2 >> r2;
        x1++;
        x2++;
        pos1 = (x1) + (y1)*m_width;
        pos2 = (x2) + (y2)*m_width;
        pos1r = pos1 * 10 + ((r1 + 4) % 8);
        pos2r = pos2 * 10 + ((r2 + 4) % 8);
        set_transition(pos1, r1, pos2r);
        set_transition(pos2, r2, pos1r);
    }
    calculate_board_values();
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
    for (uint8_t y = 0; y < m_height; y++)
    {
        for (uint8_t x = 1; x < m_width + 1; x++)
        {
            uint16_t c = m_width * y + x;
            std::cout << std::setw(3) << get_transition(c, 7) << " ";
            std::cout << std::setw(3) << get_transition(c, 0) << " ";
            std::cout << std::setw(3) << get_transition(c, 1) << " ";
            std::cout << "  ";
        }
        std::cout << std::endl;
        for (uint8_t x = 1; x < m_width + 1; x++)
        {
            uint16_t c = m_width * y + x;
            std::cout << std::setw(3) << get_transition(c, 6) << " ";
            if (check_players(get_symbol(c)))
            {
                std::cout << getColorString(Colors((get_symbol(c) - '0'))) << std::setw(3) << get_symbol(c) << " "
                          << "\e[0m";
            }
            else
            {
                std::cout << std::setw(3) << get_symbol(c) << " ";
            }
            std::cout << std::setw(3) << get_transition(c, 2) << " ";
            std::cout << "  ";
        }
        std::cout << std::endl;
        for (uint8_t x = 1; x < m_width + 1; x++)
        {
            uint16_t c = m_width * y + x;
            std::cout << std::setw(3) << get_transition(c, 5) << " ";
            std::cout << std::setw(3) << get_transition(c, 4) << " ";
            std::cout << std::setw(3) << get_transition(c, 3) << " ";
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
    for (uint16_t c = 1; c < m_num_of_fields; c++)
    {
        if (check_players(get_symbol(c)))
        {
            std::cout << getColorString(Colors((get_symbol(c) - '0'))) << std::setw(3) << get_symbol(c) << " "
                      << "\e[0m";
        }
        else
        {
            std::cout << std::setw(3) << get_symbol(c) << " ";
        }
        if (c % m_width == 0)
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
    for (uint16_t c = 1; c < m_num_of_fields; c++)
    {
        if (check_players(get_symbol(c)))
        {
            std::cout << getColorString(Colors((get_symbol(c) - '0'))) << get_symbol(c) << " "
                      << "\e[0m";
        }
        else
        {
            std::cout << get_symbol(c) << " ";
        }
        if (c % m_width == 0)
        {
            std::cout << std::endl;
        }
    }
    std::cout << std::endl;
}

/*
void Map::print_m_frontier_scores(std::vector<Player> &p)
{
    std::cout << "Frontier_scores:" << std::endl
              << std::endl;
    for (auto &player : p)
    {
        player.get_frontier_score(*this);
        std::cout << getColorString(Colors(player.m_symbol - '0')) << "Player " << player.m_symbol << ": " << player.m_frontier_score << "\e[0m" << std::endl;
    }
    std::cout << std::endl;
}
*/