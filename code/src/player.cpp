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
    for (auto elem : m_valid_moves)
    {
        coord = elem.first;
        coord % width == 0 ? x = width - 1 : x = coord % width - 1;
        y = (coord - (x + 1)) / width;
        std::cout << "(" << x << " , " << y << ")" << std::endl;
    }
}
