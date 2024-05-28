#include "map.hpp"
#include "board.hpp"
#include "move_generator.hpp"

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
    m_numbers[c] = s;
}

void Map::set_transition(uint16_t c, uint8_t d, uint16_t t)
{
    m_transitions[(c - 1) * 8 + d] = t;
}

void Map::set_player_number(uint8_t n)
{
    player_number = n - 1;
}

uint8_t Map::get_player_number()
{
    return player_number;
}

char Map::get_symbol(uint16_t c)
{
    return m_numbers[c];
}

uint16_t Map::get_transition(uint16_t c, uint8_t d)
{
    return m_transitions[(c - 1) * 8 + d] / 10;
}

uint8_t Map::get_direction(uint16_t c, uint8_t d)
{
    return m_transitions[(c - 1) * 8 + d] % 10;
}

uint16_t Map::get_height()
{
    return m_height;
}

uint16_t Map::get_width()
{
    return m_width;
}

uint16_t Map::get_player_count()
{
    return m_player_count;
}

uint16_t Map::get_strength()
{
    return m_strength;
}

uint16_t Map::get_num_of_fields()
{
    return m_num_of_fields;
}

uint16_t Map::get_initial_overwrite_stones()
{
    return m_initial_overwrite_stones;
}

uint16_t Map::get_initial_bombs()
{
    return m_initial_bombs;
}

std::vector<uint16_t> Map::get_transitions()
{
    return m_transitions;
}

std::vector<std::bitset<20000>> Map::get_transitions_to_remove()
{
    return transitions_to_remove;
}

std::vector<std::bitset<2501>> Map::get_fields_to_remove()
{
    return fields_to_remove;
}

/**
 * @brief reads the input and sets all the information required for the game map
 *
 * @param inputfile mapfile to read
 */
void Map::read_map(std::stringstream mapfile)
{
    char temp;
    mapfile >> m_player_count >> m_initial_overwrite_stones >> m_initial_bombs >> m_strength >> m_height >> m_width;
    m_num_of_fields = m_height * m_width + 1;
    m_transitions = std::vector<uint16_t>((m_num_of_fields - 1) * 8 + 1, 0);
    m_numbers = std::vector<char>(m_num_of_fields, 0);
    // every coordinate gets a symbol and it's neighbours are being set
    for (int c = 1; c < m_num_of_fields; c++)
    {
        mapfile >> temp;
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
}

void Map::one_dimension_2_second_dimension(uint16_t _1D_coord, uint8_t &x, uint8_t &y)
{
    _1D_coord % m_width == 0 ? x = m_width - 1 : x = _1D_coord % m_width - 1;
    y = (_1D_coord - (x + 1)) / m_width;
}

uint16_t Map::two_dimension_2_one_dimension(uint8_t x, uint8_t y)
{
    return (x + 1 + y * m_width);
}

bool Map::check_players(char c)
{
    std::array<unsigned char, 8> player_symbols{'1', '2', '3', '4', '5', '6', '7', '8'};
    bool var = false;
    for (uint8_t i = 0; i < player_symbols.size(); i++)
    {
        if (c == player_symbols[i])
        {
            var = true;
        }
    }
    return var;
}

void Map::set_values(Board &board, uint16_t c)
{
    if (get_symbol(c) == '0')
    {
        board.board_sets[1].set(c);
    }
    else if (get_symbol(c) == '-')
    {
        board.board_sets[0].set(c);
    }
    else if (get_symbol(c) == 'i')
    {
        board.board_sets[1].set(c);
        board.board_sets[2].set(c);
    }
    else if (get_symbol(c) == 'c')
    {
        board.board_sets[1].set(c);
        board.board_sets[3].set(c);
    }
    else if (get_symbol(c) == 'b')
    {
        board.board_sets[1].set(c);
        board.board_sets[4].set(c);
    }
    else if (get_symbol(c) == 'x')
    {
        board.board_sets[5].set(c);
    }
    else if (check_players(get_symbol(c)))
    {
        board.player_sets[get_symbol(c) - '0' - 1].set(c);
    }
}

void Map::init_protected_fields(Board &board)
{
    for (uint8_t i = 0; i < board.get_player_count(); i++)
    {
        board.protected_fields[i] = board.player_sets[i] & (board.wall_sets[3] | board.wall_sets[4] | board.wall_sets[5] | board.wall_sets[6] | board.wall_sets[7]);
    }
}

void Map::init_wall_values(Board &board)
{
    for (uint16_t c = 1; c < board.get_num_of_fields(); c++)
    {
        if (board.border_sets[0].test(c))
        {
            uint8_t counter = 0;
            for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d++)
            {
                uint16_t next_coord = get_transition(c, d);
                if (next_coord == 0)
                {
                    counter++;
                }
            }
            board.wall_sets[counter - 1].set(c);
        }
    }
    init_protected_fields(board);
}

bool Map::get_walls(Board &board, std::bitset<2501> &checked)
{
    for (uint16_t c = 1; c < m_num_of_fields; c++)
    {
        for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d++)
        {
            if (get_transition(c, d) == 0 && !board.board_sets[0].test(c))
            {
                checked.set(c);
                break;
            }
        }
    }
    if (checked.count() == 0)
        return false;
    else
    {
        board.border_sets[0] = checked;
        return true;
    }
    void init_wall_values(Board & board);
}

std::bitset<2501> Map::get_inside_of_walls(Board &board, std::bitset<2501> &checked, uint16_t counter)
{
    std::bitset<2501> set;
    for (uint16_t c = 1; c < m_num_of_fields; c++)
    {
        if (board.border_sets[counter].test(c))
        {
            for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d++)
            {
                if (get_transition(c, d) != 0)
                {
                    if (checked.test(c))
                    {
                        uint16_t trans1 = get_transition(c, (d + 3) % NUM_OF_DIRECTIONS);
                        uint16_t trans2 = get_transition(c, (d + 4) % NUM_OF_DIRECTIONS);
                        uint16_t trans3 = get_transition(c, (d + 5) % NUM_OF_DIRECTIONS);
                        if (trans1 != 0 && !checked.test(trans1))
                        {
                            set.set(trans1);
                        }
                        if (trans2 != 0 && !checked.test(trans2))
                        {
                            set.set(trans2);
                        }
                        if (trans3 != 0 && !checked.test(trans3))
                        {
                            set.set(trans3);
                        }
                    }
                }
            }
        }
    }
    checked |= set;
    return set;
}

bool Map::set_player_border_sets(Board &board, std::bitset<2501> set)
{
    if (set.count() == 0)
        return false;
    board.border_sets.push_back(set);
    return true;
}

void Map::init_evaluation(Board &board)
{
    std::bitset<2501> checked;
    if (get_walls(board, checked))
    {
        uint16_t counter = 0;
        while (set_player_border_sets(board, get_inside_of_walls(board, checked, counter)))
            counter++;
    }
}

Board Map::init_boards_and_players()
{
    fields_to_remove = std::vector<std::bitset<2501>>(m_num_of_fields);
    transitions_to_remove = std::vector<std::bitset<20000>>(m_num_of_fields);
    Board ret_board(*this);
    for (uint16_t c = 1; c < m_num_of_fields; c++)
    {
        set_values(ret_board, c);
    }
    for (uint8_t i = 0; i < get_player_count(); i++)
    {
        ret_board.set_overwrite_stones(i, m_initial_overwrite_stones);
        ret_board.set_bombs(i, m_initial_bombs);
    }
    for (uint16_t c = 1; c < get_num_of_fields(); c++)
    {
        if (!ret_board.board_sets[1].test(c))
        {
            for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d++)
            {
                uint16_t next_coord = get_transition(c, d);
                if (next_coord != 0 && ret_board.board_sets[1].test(next_coord))
                {
                    ret_board.board_sets[6].set(next_coord);
                }
            }
        }
    }
    init_evaluation(ret_board);
    return ret_board;
}

void Map::print_bitset(std::bitset<2501> &bitset)
{
    for (uint16_t c = 1; c < m_num_of_fields; c++)
    {
        if (bitset.test(c))
        {
            std::cout << "\e[93m"
                      << "1 "
                      << "\033[0m";
        }
        else
        {
            std::cout << "0 ";
        }
        if (c % m_width == 0)
        {
            std::cout << std::endl;
        }
    }
    std::cout << std::endl;
}

void Map::print_transitions()
{
    std::cout << std::endl;
    for (int y = 0; y < m_height; y++)
    {
        for (int n = 1; n < m_width + 1; n++)
        {
            int x = m_width * y + n;
            std::cout << std::setw(3) << m_transitions[(x - 1) * 8 + 7] / 10 << " ";
            std::cout << std::setw(3) << m_transitions[(x - 1) * 8 + 0] / 10 << " ";
            std::cout << std::setw(3) << m_transitions[(x - 1) * 8 + 1] / 10 << " ";
            std::cout << "  ";
        }
        std::cout << std::endl;
        for (int n = 1; n < m_width + 1; n++)
        {
            int x = m_width * y + n;
            std::cout << std::setw(3) << m_transitions[(x - 1) * 8 + 6] / 10 << " ";
            std::cout << std::setw(3) << m_numbers[x] << " ";
            std::cout << std::setw(3) << m_transitions[(x - 1) * 8 + 2] / 10 << " ";
            std::cout << "  ";
        }
        std::cout << std::endl;
        for (int n = 1; n < m_width + 1; n++)
        {
            int x = m_width * y + n;
            std::cout << std::setw(3) << m_transitions[(x - 1) * 8 + 5] / 10 << " ";
            std::cout << std::setw(3) << m_transitions[(x - 1) * 8 + 4] / 10 << " ";
            std::cout << std::setw(3) << m_transitions[(x - 1) * 8 + 3] / 10 << " ";
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
    for (int i = 1; i < (m_width * m_height + 1); i++)
    {
        std::cout << m_numbers[i] << " ";
        if (i % m_width == 0)
        {
            std::cout << std::endl;
        }
    }
    print_transitions();
}

void Map::init_bomb_phase_boards()
{
    for (uint16_t c = 1; c < m_num_of_fields; c++)
    {
        if (m_strength == 0)
        {
            fields_to_remove[c].set(c);
            for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d++)
            {
                uint16_t temp_transition = get_transition(c, d);
                uint8_t temp_direction = get_direction(c, d);
                transitions_to_remove[c].set((temp_transition - 1) * 8 + (temp_direction + 4) % 8);
            }
        }
        else
        {
            std::unordered_set<uint16_t> bombed_stones;
            bombed_stones.insert(c);
            fields_to_remove[c].set(c);
            for (uint8_t s = 1; s <= m_strength; ++s)
            {
                std::vector<uint16_t> next_bombed_stones;
                for (auto &stone : bombed_stones)
                {
                    for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d++)
                    {
                        uint16_t transition = get_transition(stone, d);
                        if (transition != 0 && get_symbol(transition) != '-')
                        {
                            fields_to_remove[c].set(transition);
                            next_bombed_stones.push_back(transition);
                        }
                    }
                }
                bombed_stones.insert(next_bombed_stones.begin(), next_bombed_stones.end());
            }

            // Update transitions for all destroyed stones
            for (auto &stone : bombed_stones)
            {
                for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d++)
                {
                    uint16_t transition = get_transition(stone, d);
                    if (transition != 0)
                    {
                        uint16_t direction = get_direction(stone, d);
                        transitions_to_remove[c].set((stone - 1) * 8 + d);
                        transitions_to_remove[c].set((transition - 1) * 8 + (direction + 4) % 8);
                    }
                }
            }
        }
    }
}
