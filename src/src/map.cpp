#include "map.hpp"
#include "board.hpp"
#include "move_generator.hpp"
#include "move_board.hpp" 

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
    return m_initial_m_overwrite_stones;
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
    return m_transitions_to_remove;
}

std::vector<std::bitset<2501>> Map::get_fields_to_remove()
{
    return m_fields_to_remove;
}

/**
 * @brief reads the input and sets all the information required for the game map
 *
 * @param inputfile mapfile to read
 */
void Map::read_map(std::stringstream mapfile)
{
    char temp;
    mapfile >> m_player_count >> m_initial_m_overwrite_stones >> m_initial_bombs >> m_strength >> m_height >> m_width;
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
        board.get_board_set(1).set(c);
    }
    else if (get_symbol(c) == '-')
    {
        board.get_board_set(0).set(c);
    }
    else if (get_symbol(c) == 'i')
    {
        board.get_board_set(1).set(c);
        board.get_board_set(2).set(c);
    }
    else if (get_symbol(c) == 'c')
    {
        board.get_board_set(1).set(c);
        board.get_board_set(3).set(c);
    }
    else if (get_symbol(c) == 'b')
    {
        board.get_board_set(1).set(c);
        board.get_board_set(4).set(c);
    }
    else if (get_symbol(c) == 'x')
    {
        board.get_board_set(5).set(c);
    }
    else if (check_players(get_symbol(c)))
    {
        board.get_player_set(get_symbol(c) - '0' - 1).set(c);
    }
}

void Map::init_wall_values(MoveBoard &move_board)
{
    for (uint16_t c = 1; c < move_board.get_num_of_fields(); c++)
    {
        if (move_board.get_board_set(0).test(c))
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
            move_board.get_wall_set(counter - 1).set(c);
        }
    }
}

bool Map::get_walls(MoveBoard &move_board, std::bitset<2501> &checked)
{
    for (uint16_t c = 1; c < m_num_of_fields; c++)
    {
        for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d++)
        {
            if (get_transition(c, d) == 0 && !move_board.get_board_set(0).test(c))
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
        move_board.get_border_set(0) = checked;
        return true;
    }
    void init_wall_values(Board & move_board);
}

std::bitset<2501> Map::get_inside_of_walls(MoveBoard &move_board, std::bitset<2501> &checked, uint16_t counter)
{
    std::bitset<2501> set;
    for (uint16_t c = 1; c < m_num_of_fields; c++)
    {
        if (move_board.get_board_set(counter).test(c))
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

bool Map::set_player_border_sets(MoveBoard &move_board, std::bitset<2501> set)
{
    if (set.count() == 0)
        return false;
    move_board.get_border_sets().push_back(set);
    return true;
}

void Map::init_evaluation(MoveBoard &move_board)
{
    std::bitset<2501> checked;
    if (get_walls(move_board, checked))
    {
        uint16_t counter = 0;
        while (set_player_border_sets(move_board, get_inside_of_walls(move_board, checked, counter)))
            counter++;
    }
}

MoveBoard Map::init_boards_and_players()
{
    Board board(*this);
    MoveBoard ret_board = MoveBoard(board);
    for (uint16_t c = 1; c < m_num_of_fields; c++)
    {
        set_values(ret_board, c);
    }
    for (uint16_t c = 1; c < get_num_of_fields(); c++)
    {
        if (!ret_board.get_board_set(1).test(c))
        {
            for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d++)
            {
                uint16_t next_coord = get_transition(c, d);
                if (next_coord != 0 && ret_board.get_board_set(1).test(next_coord))
                {
                    ret_board.get_board_set(6).set(next_coord);
                }
            }
        }
    }
    init_evaluation(ret_board);
    return ret_board;
}
