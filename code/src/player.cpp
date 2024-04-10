#include "player.hpp"

Player::Player(char n) : m_player_symbol(n) {}
Player::~Player(){};

void Player::set_bombs_and_stones(uint16_t o, uint16_t b)
{
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

void Player::decrement_overwrite_stone()
{
    m_overwrite_stones--;
}

void Player::decrement_bombs()
{
    m_bombs--;
}