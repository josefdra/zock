#include "player.hpp"
#include "map.hpp"

Player::Player(unsigned char s, uint16_t o, uint16_t b) : m_symbol(s),
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
    uint16_t x, y, c;
    for (auto &elem : m_valid_moves)
    {
        c = elem.first;
        c % width == 0 ? x = width - 1 : x = c % width - 1;
        y = (c - (x + 1)) / width;
        std::cout << "(" << x << " , " << y << ")" << std::endl;
    }
}

void Player::print_frontiers(Map &m)
{
    for (uint16_t c = 1; c < m.m_num_of_fields + 1; c++)
    {
        if (m.get_symbol(c) == m_symbol)
        {
            std::cout << getColorString(Colors((m.get_symbol(c) - '0'))) << std::setw(3) << check_frontier(m, c) << " "
                      << "\e[0m";
        }
        else
        {
            std::cout << std::setw(3) << m.get_symbol(c) << " ";
        }
        if (c % m.m_width == 0)
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
        if (check_empty_fields(m.get_symbol(c)))
        {
            m_protected_fields.insert(c);
        }
        else if (m.get_symbol(c) == m_symbol)
        {
            m_protected_fields.insert(c);
            for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d += 2)
            {
                if (m.get_transition(c, d) != 0)
                {
                    m_protected_fields.insert(m.get_transition(c, d));
                    m.m_protected_fields.insert(m.get_transition(c, d));
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
        for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d++)
        {
            if (m_protected_fields.find(m.get_transition(c, d)) != m_protected_fields.end())
            {
                m_before_before_protected_fields.insert(m.get_transition(c, (d + 4) % 8) / 10);
                m_before_before_protected_fields.insert(m.get_transition(c, (d + 3) % 8) / 10);
                m_before_before_protected_fields.insert(m.get_transition(c, (d + 5) % 8) / 10);
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

void Player::get_frontier_score(Map &m)
{
    for (uint16_t c = 1; c < m.m_num_of_fields + 1; c++)
    {
        if (m.get_symbol(c) == m_symbol)
        {
            m_frontier_score += check_frontier(m, c);
        }
    }
}

void Player::get_moves_score(Map &m)
{
    std::cout << getColorString(Colors(m_symbol - '0')) << "Number of moves for Player " << m_symbol << ": " << m_valid_moves.size() << "\e[0m" << std::endl;
    std::cout << getColorString(Colors(m_symbol - '0')) << std::endl
              << "-----------Scores:"
              << "\e[0m" << std::endl
              << std::endl;
    std::string msg = "";
    for (auto &move : m_valid_moves)
    {
        uint16_t special = std::get<0>(move.second);
        if (special == 0) // normal
        {
            if (check_empty_fields(m.get_symbol(move.first))) // empty field (0)
            {
                m_moves_scores[move.first] = 1;
                msg = "(0)";
            }
            else if (check_players(m.get_symbol(move.first))) // enemy player
            {
                m_moves_scores[move.first] = -20;
                msg = "enemy player";
            }
            else // expansion-stone
            {
                m_moves_scores[move.first] = -10;
                msg = "expansion-stone";
            }
        }
        else if (special == 1) // inversion
        {
            m_moves_scores[move.first] = 25;
            msg = "inversion";
        }
        else if (special == 2) // choice
        {
            m_moves_scores[move.first] = 25;
            msg = "choice";
        }
        else if (special == 3) // bonus
        {
            m_moves_scores[move.first] = 25;
            msg = "bonus";
        }
        else
        {
            std::cout << "invalid special value" << std::endl;
        }
        std::cout << getColorString(Colors(m_symbol - '0')) << move.first << "-Move (" << move.first % m.m_width - 1 << "," << move.first / m.m_width << "): "
                  << m_moves_scores[move.first] << " - " << msg << "\e[0m" << std::endl;
    }
    std::cout << std::endl;
}