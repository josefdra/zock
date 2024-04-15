#include "player.hpp"
#include "map.hpp"

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

void Player::check_protected_fields(Map &m)
{
    m_protected_fields.clear();
    for (auto &c : m.m_corners)
    {
        m.m_protected_fields.insert(c);
        if (check_empty_fields(m.m_symbol_and_transitions[c].symbol))
        {
            m_protected_fields.insert(c);
        }
        else if (m.m_symbol_and_transitions[c].symbol == m_symbol)
        {
            m_protected_fields.insert(c);
            for (int i = 0; i < NUM_OF_DIRECTIONS; i += 2)
            {
                if (m.m_symbol_and_transitions[c].transitions[i] != 0)
                {
                    m_protected_fields.insert(m.m_symbol_and_transitions[c].transitions[i] / 10);
                    m.m_protected_fields.insert(m.m_symbol_and_transitions[c].transitions[i] / 10);
                }
            }
        }
    }
}

void Player::check_before_before_protected_fields(Map &m)
{
    m_before_before_protected_fields.clear();
    for (auto &c : m.m_before_protected_fields)
    {
        for (int i = 0; i < NUM_OF_DIRECTIONS; i++)
        {
            if (m_protected_fields.find(m.m_symbol_and_transitions[c].transitions[i] / 10) != m_protected_fields.end())
            {
                m_before_before_protected_fields.insert(m.m_symbol_and_transitions[c].transitions[(i + 4) % 8] / 10);
                m_before_before_protected_fields.insert(m.m_symbol_and_transitions[c].transitions[(i + 3) % 8] / 10);
                m_before_before_protected_fields.insert(m.m_symbol_and_transitions[c].transitions[(i + 5) % 8] / 10);
            }
        }
    }
    m_before_before_protected_fields.erase(0);
}

bool Player::check_if_remove_border(Map &m, uint16_t c)
{
    bool val = false;
    if (m.m_protected_fields.find(c) != m.m_protected_fields.end())
    {
        val = true;
    }
    else if (m.m_before_protected_fields.find(c) != m.m_before_protected_fields.end())
    {
        val = true;
    }
    else if (m_before_before_protected_fields.find(c) != m_before_before_protected_fields.end())
    {
        val = true;
    }
    return val;
}

void Player::update_borders(Map &m)
{
    m_borders.clear();
    m_before_borders.clear();
    m_before_before_borders.clear();
    for (auto &c : m.m_borders)
    {
        if (!check_if_remove_border(m, c))
        {
            m_borders.insert(c);
        }
    }
    for (auto &c : m.m_before_borders)
    {
        if (!check_if_remove_border(m, c))
        {
            m_before_borders.insert(c);
        }
    }
    for (auto &c : m.m_before_before_borders)
    {
        if (!check_if_remove_border(m, c))
        {
            m_before_before_borders.insert(c);
        }
    }
}
