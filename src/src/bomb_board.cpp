#include "bomb_board.hpp"
#include "board.hpp"
#include "map.hpp"

BombBoard::BombBoard() {}

BombBoard::~BombBoard() {}

void BombBoard::init_bomb_board(Board &board, Map &map)
{
    board_sets = board.board_sets;
    player_sets = board.player_sets;
    valid_moves = board.valid_moves;
    fields_to_remove = map.get_fields_to_remove();
    transitions_to_remove = map.get_transitions_to_remove();
    bombs = board.bombs;
    disqualified = board.disqualified;
    m_player_count = board.get_player_count();
    m_num_of_fields = board.get_num_of_fields();
    m_width = board.get_width();
    m_height = board.get_height();
    m_coord = 0;
    m_transitions = map.get_transitions();
    m_strength = map.get_strength();
}

void BombBoard::set_coord(uint16_t coord)
{
    m_coord = coord;
}

void BombBoard::set_transition(uint16_t c, uint8_t d, uint16_t t)
{
    m_transitions[(c - 1) * 8 + d] = t;
}

void BombBoard::set_zero_transition(uint16_t c)
{
    m_transitions[c] = 0;
}

uint8_t BombBoard::get_player_count()
{
    return m_player_count;
}

uint16_t BombBoard::get_num_of_fields()
{
    return m_num_of_fields;
}

uint8_t BombBoard::get_width()
{
    return m_width;
}

uint8_t BombBoard::get_height()
{
    return m_height;
}

uint16_t BombBoard::get_bombs(uint8_t player)
{
    return bombs[player];
}

uint16_t BombBoard::get_coord()
{
    return m_coord;
}

int BombBoard::get_evaluation()
{
    return evaluation;
}

uint8_t BombBoard::get_strength()
{
    return m_strength;
}

uint16_t BombBoard::get_transition(uint16_t c, uint8_t d)
{
    return m_transitions[(c - 1) * 8 + d] / 10;
}

uint8_t BombBoard::get_direction(uint16_t c, uint8_t d)
{
    return m_transitions[(c - 1) * 8 + d] % 10;
}

void BombBoard::decrement_bombs(uint8_t player)
{
    bombs[player]--;
}

void BombBoard::one_dimension_2_second_dimension(uint16_t _1D_coord, uint8_t &x, uint8_t &y)
{
    _1D_coord % m_width == 0 ? x = m_width - 1 : x = _1D_coord % m_width - 1;
    y = (_1D_coord - (x + 1)) / m_width;
}

uint16_t BombBoard::two_dimension_2_one_dimension(uint8_t x, uint8_t y)
{
    return (x + 1 + y * m_width);
}

std::string BombBoard::get_color_string(Colors_Bomb color)
{
    switch (color)
    {
    case m_orange:
        return "\e[38;5;208m";
    case m_red:
        return "\e[91m";
    case m_green:
        return "\e[92m";
    case m_yellow:
        return "\e[93m";
    case m_blue:
        return "\e[94m";
    case m_magenta:
        return "\e[95m";
    case m_cyan:
        return "\e[96m";
    case m_dark_black:
        return "\e[30m";
    default:
        return "\e[37m";
    }
}

void BombBoard::print_upper_outlines()
{
    std::cout << "     ";
    for (uint16_t i = 0; i < get_width(); i++)
    {
        std::cout << std::setw(2) << i;
    }
    std::cout << std::endl
              << "    /";
    for (uint16_t i = 0; i < get_width(); i++)
    {
        std::cout << "--";
    }
    std::cout << std::endl;
}

bool BombBoard::print_bomb_board_sets(uint16_t c)
{
    if (board_sets[0].test(c))
    {
        std::cout << "-";
        return true;
    }
    else if (board_sets[2].test(c))
    {
        std::cout << "i";
        return true;
    }
    else if (board_sets[3].test(c))
    {
        std::cout << "c";
        return true;
    }
    else if (board_sets[4].test(c))
    {
        std::cout << "b";
        return true;
    }
    // BombBoards[1] is printed after i, c, b, because these fields are also 0 fields
    // and otherwise the i, c, b values would be overwritten
    else if (board_sets[1].test(c))
    {
        std::cout << "0";
        return true;
    }
    else if (board_sets[5].test(c))
    {
        std::cout << "x";
        return true;
    }
    return false;
}

void BombBoard::print(uint8_t player, bool our_player)
{
    print_upper_outlines();
    for (uint16_t y = 0; y < get_height(); y++)
    {
        std::cout << " " << std::setw(2) << y << " | ";
        for (uint16_t x = 0; x < get_width(); x++)
        {
            uint16_t c = two_dimension_2_one_dimension(x, y);
            if (!print_bomb_board_sets(c))
            {
                for (uint16_t i = 0; i < m_player_count; i++)
                {
                    if (player_sets[i].test(c))
                    {
                        std::cout << get_color_string(Colors_Bomb(i + 1)) << i + 1
                                  << "\033[0m";
                    }
                }
            }
            if (our_player && valid_moves[player].test(c))
            {
                std::cout << "'";
            }
            else
            {
                std::cout << " ";
            }
        }
        std::cout << std::endl;
    }
    std::cout << "Calculated valid moves: " << valid_moves[player].count() << std::endl;
    std::cout << std::endl;
}

// For visualization of a bitset. Can probably be removed
void BombBoard::print_bitset(std::bitset<2501> &bitset)
{
    for (uint16_t c = 1; c < m_num_of_fields; c++)
    {
        if (bitset.test(c))
        {
            std::cout << get_color_string(m_yellow) << "1 "
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

void BombBoard::print_transitions(std::bitset<20000> &set)
{
    std::cout << std::endl;
    for (int y = 0; y < m_height; y++)
    {
        for (int n = 1; n < m_width + 1; n++)
        {
            int x = m_width * y + n;
            if (set.test((x - 1) * 8 + 7))
                std::cout << "\e[93m";
            std::cout << std::setw(4) << m_transitions[(x - 1) * 8 + 7] / 10 << "\033[0m"
                      << " ";
            if (set.test((x - 1) * 8 + 0))
                std::cout << "\e[93m";
            std::cout << std::setw(4) << m_transitions[(x - 1) * 8 + 0] / 10 << "\033[0m"
                      << " ";
            if (set.test((x - 1) * 8 + 1))
                std::cout << "\e[93m";
            std::cout << std::setw(4) << m_transitions[(x - 1) * 8 + 1] / 10 << "\033[0m"
                      << " ";
            std::cout << "  ";
        }
        std::cout << std::endl;
        for (int n = 1; n < m_width + 1; n++)
        {
            int x = m_width * y + n;
            if (set.test((x - 1) * 8 + 6))
                std::cout << "\e[93m";
            std::cout << std::setw(4) << m_transitions[(x - 1) * 8 + 6] / 10 << "\033[0m"
                      << " ";
            std::cout << std::setw(4) << "x"
                      << " ";
            if (set.test((x - 1) * 8 + 2))
                std::cout << "\e[93m";
            std::cout << std::setw(4) << m_transitions[(x - 1) * 8 + 2] / 10 << "\033[0m"
                      << " ";
            std::cout << "  ";
        }
        std::cout << std::endl;
        for (int n = 1; n < m_width + 1; n++)
        {
            int x = m_width * y + n;
            if (set.test((x - 1) * 8 + 5))
                std::cout << "\e[93m";
            std::cout << std::setw(4) << m_transitions[(x - 1) * 8 + 5] / 10 << "\033[0m"
                      << " ";
            if (set.test((x - 1) * 8 + 4))
                std::cout << "\e[93m";
            std::cout << std::setw(4) << m_transitions[(x - 1) * 8 + 4] / 10 << "\033[0m"
                      << " ";
            if (set.test((x - 1) * 8 + 3))
                std::cout << "\e[93m";
            std::cout << std::setw(4) << m_transitions[(x - 1) * 8 + 3] / 10 << "\033[0m"
                      << " ";
            std::cout << "  ";
        }
        std::cout << std::endl;
        std::cout << std::endl;
    }
}

void BombBoard::get_bomb_coords(uint16_t start_coord, uint16_t c, uint8_t strength)
{
    if (strength == 0)
    {
        for (uint8_t dir = 0; dir < NUM_OF_DIRECTIONS; dir++)
        {
            uint16_t next_coord2 = get_transition(c, dir);
            if (next_coord2 != 0)
            {
                transitions_to_remove[start_coord].set((next_coord2 - 1) * 8 + (dir + 4) % 8);
            }
        }
        return;
    }
    for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d++)
    {
        uint16_t next_coord = get_transition(c, d);
        if (next_coord != 0)
        {
            for (uint8_t dir = 0; dir < NUM_OF_DIRECTIONS; dir++)
                transitions_to_remove[start_coord].set((next_coord - 1) * 8 + dir);
            fields_to_remove[start_coord].set(next_coord);
            get_bomb_coords(start_coord, next_coord, strength - 1);
        }
    }
}

void BombBoard::init_bomb_phase_boards()
{
    for (uint16_t c = 1; c < m_num_of_fields; c++)
    {
        fields_to_remove[c].reset();
        transitions_to_remove[c].reset();
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
            fields_to_remove[c].set(c);
            for (uint8_t s = 1; s <= m_strength; ++s)
            {
                std::vector<uint16_t> next_bombed_stones;
                for (uint16_t a = 1; a < m_num_of_fields; a++)
                {
                    if (fields_to_remove[c].test(a))
                    {
                        for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d++)
                        {
                            uint16_t transition = get_transition(a, d);
                            if (transition != 0 && !board_sets[0].test(transition))
                            {
                                fields_to_remove[c].set(transition);
                            }
                        }
                    }
                }
            }

            // Update transitions for all destroyed stones
            for (uint16_t a = 1; a < m_num_of_fields; a++)
            {
                if (fields_to_remove[c].test(a))
                {
                    for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d++)
                    {
                        uint16_t transition = get_transition(a, d);
                        if (transition != 0)
                        {
                            uint16_t direction = get_direction(a, d);
                            transitions_to_remove[c].set((a - 1) * 8 + d);
                            transitions_to_remove[c].set((transition - 1) * 8 + (direction + 4) % 8);
                        }
                    }
                }
            }
        }
    }
}