#include "player.hpp"
#include "map.hpp"

Player::Player() {}

Player::~Player()
{
}

void Player::init(uint16_t o, uint16_t b, char s)
{
    m_symbol = s;
    m_overwrite_stones = o;
    m_bombs = b;
}

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
