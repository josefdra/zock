#include "player.hpp"

Player::Player(char s, uint16_t o, uint16_t b) : m_symbol(s),
                                                 m_overwrite_stones(o),
                                                 m_bombs(b)
{
}
Player::~Player() {}

bool Player::has_bombs()
{
    if (m_bombs > 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool Player::has_overwrite_stones()
{
    if (m_overwrite_stones > 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void Player::print_valid_moves(uint16_t width)
{
    uint16_t x, y, coord;
    for (auto &elem : m_valid_moves)
    {
        coord = elem.first;
        coord % width == 0 ? x = width - 1 : x = coord % width - 1;
        y = (coord - (x + 1)) / width;
        std::cout << "(" << x << " , " << y << ")" << std::endl;
    }
}

void Player::print_frontiers(Map &m)
{
    for (uint16_t i = 1; i < (m.m_width * m.m_height + 1); i++)
    {
        if (m.m_symbol_and_transitions[i].symbol == m_symbol)
        {
            std::cout << getColorString(Colors((m.m_symbol_and_transitions[i].symbol - '0'))) << std::setw(3) << check_frontier(m, i) << " "
                      << "\e[0m";
        }
        else
        {
            std::cout << std::setw(3) << m.m_symbol_and_transitions[i].symbol << " ";
        }
        if (i % m.m_width == 0)
        {
            std::cout << std::endl;
        }
    }
    std::cout << std::endl;
}
