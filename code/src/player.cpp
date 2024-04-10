#include "player.hpp"

Player::Player(char s, uint16_t o, uint16_t b) : m_symbol(s),
                                                 m_overwrite_stones(o),
                                                 m_bombs(b)
{
}
Player::~Player() {}

bool Player::has_valid_moves()
{
    return m_has_valid_moves;
}

void Player::set_valid_moves(bool b)
{
    m_has_valid_moves = b;
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

void Player::decrement_overwrite_stone()
{
    m_overwrite_stones--;
}

void Player::decrement_bombs()
{
    m_bombs--;
}

char Player::get_symbol()
{
    return m_symbol;
}
