#include "process_moves.hpp"

/**
 * @brief check_valid_moves.cpp is responsible for all calculations to verify valid moves
 *
 */

void change_players(Map &m, unsigned char p1, unsigned char p2)
{
    for (uint16_t c = 1; c < m.m_num_of_fields; c++)
    {
        unsigned char s = m.get_symbol(c);
        if (s == p1)
        {
            m.set_symbol(c, p2);
        }
        else if (s == p2)
        {
            m.set_symbol(c, p1);
        }
    }
}

void execute_inversion(Map &m, char p)
{
    for (uint16_t c = 1; c < m.m_num_of_fields; c++)
    {
        if (check_players(m.get_symbol(c)))
        {
            m.set_symbol(c, ((m.get_symbol(c) - '0') % m.m_player_count) + '0' + 1);
        }
    }
}

void color(uint16_t c, char s, Map &m)
{
    std::unordered_set<uint16_t> to_color;
    to_color.insert(c);
    for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d++)
    {
        uint16_t temp_transition = m.get_transition(c, d);
        uint8_t temp_direction = m.get_direction(c, d);
        std::unordered_set<uint16_t> temp;
        char symbol;
        while (temp_transition != 0 && !check_empty_fields(symbol = m.get_symbol(temp_transition)))
        {
            if (symbol == s)
            {
                for (auto &c : temp)
                {
                    to_color.insert(c);
                }
                break;
            }
            else
            {
                temp.insert(temp_transition);
                uint16_t next_transition = m.get_transition(temp_transition, temp_direction);
                uint8_t next_direction = m.get_direction(temp_transition, temp_direction);
                temp_transition = next_transition;
                temp_direction = next_direction;
            }
        }
    }
    for (auto &c : to_color)
    {
        m.set_symbol(c, s);
    }
}

void execute_move(uint16_t c, uint8_t special, Player &p, Map &m)
{
    char curr_symbol = m.get_symbol(c);
    if (!check_empty_fields(curr_symbol))
    {
        p.m_overwrite_stones--;
        color(c, p.m_symbol, m);
    }
    // normal
    else if (curr_symbol == '0')
    {
        color(c, p.m_symbol, m);
    }
    // inversion
    else if (curr_symbol == 'i')
    {
        color(c, p.m_symbol, m);
        execute_inversion(m, p.m_symbol);
    }
    // choice
    else if (curr_symbol == 'c')
    {
        color(c, p.m_symbol, m);
        change_players(m, p.m_symbol, ('0' + special));
    }
    // bonus
    else if (curr_symbol == 'b')
    {
        color(c, p.m_symbol, m);
        if (special == 20)
        {
            p.m_bombs += 1;
        }
        else if (special == 21)
        {
            p.m_overwrite_stones += 1;
        }
    }
    else
    {
        std::cout << "Invalid special value" << std::endl;
        return;
    }
}

void execute_bomb(uint16_t c, Map &m, Player &p)
{
    if (m.m_strength == 0)
    {
        m.set_symbol(c, '-');
        for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d++)
        {
            uint16_t temp_transition = m.get_transition(c, d);
            uint8_t temp_direction = m.get_direction(c, d);
            m.set_transition(c, d, 0);
            m.set_transition(temp_transition, (temp_direction + 4) % 8, 0);
        }
        p.m_bombs -= 1;
    }
    else
    {
        // bomb strength greater 0: destroy neighbour stones
        std::unordered_set<uint16_t> bombed_stones;
        bombed_stones.insert(c); // add stone in the center

        for (uint8_t s = 1; s <= m.m_strength; ++s)
        {
            std::vector<uint16_t> next_bombed_stones;
            for (auto stone : bombed_stones)
            {
                // Destroy neigbour stones for each level of bomb strength
                for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d++)
                {
                    uint16_t transition = m.get_transition(stone, d);
                    if (transition != 0 && m.get_symbol(transition) != '-')
                    {
                        m.set_symbol(transition, '-');
                        next_bombed_stones.push_back(transition);
                    }
                }
            }
            bombed_stones.insert(next_bombed_stones.begin(), next_bombed_stones.end());
        }

        // Update transitions for all destroyed stones
        for (auto stone : bombed_stones)
        {
            for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d++)
            {
                uint16_t transition = m.get_transition(stone, d);
                if (transition != 0)
                {
                    uint16_t direction = m.get_direction(stone, d);
                    m.set_transition(stone, d, 0);
                    m.set_transition(transition, (direction + 4) % 8, 0);
                }
            }
        }
        m.set_symbol(c, '-');
        p.m_bombs -= 1;
    }
}

std::vector<char> temp_color(uint16_t c, char s, Map &m, std::vector<char> &currMap, char my_symbol)
{
    std::unordered_set<uint16_t> to_color;
    std::vector<char> ret_map = currMap;
    to_color.insert(c);
    for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d++)
    {
        uint16_t temp_transition = m.get_transition(c, d);
        uint8_t temp_direction = m.get_direction(c, d);
        std::unordered_set<uint16_t> temp;
        while (temp_transition != 0 && !check_empty_fields(currMap[temp_transition]))
        {
            if (currMap[temp_transition] == s)
            {
                for (auto &c : temp)
                {
                    to_color.insert(c);
                }
                break;
            }
            else
            {
                temp.insert(temp_transition);
                uint16_t next_transition = m.get_transition(temp_transition, temp_direction);
                uint8_t next_direction = m.get_direction(temp_transition, temp_direction);
                temp_transition = next_transition;
                temp_direction = next_direction;
            }
        }
    }
    for (auto &c : to_color)
    {
        ret_map[c] = s;
    }
    return ret_map;
}

/// @brief This function asks for a coordinate and checks if it's a valid move
/// @param map current map layout
/// @param player_number current player at turn
void calculate_valid_moves(Map &m, Player &p, std::vector<char> &currMap, char my_symbol)
{
    bool overrides = false;
    if (p.has_overwrite_stones())
    {
        overrides = true;
    }
    p.m_valid_moves.clear();
    for (uint16_t c = 1; c < m.m_num_of_fields; c++)
    {
        if (currMap[c] == 'x' && overrides)
        {
            p.m_valid_moves.insert(c);
        }
        if (currMap[c] == p.m_symbol)
        {
            for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d++)
            {
                uint16_t next_field = m.get_transition(c, d);
                uint8_t next_direction = m.get_direction(c, d);
                uint16_t temp_trans;
                uint8_t temp_dir;
                bool valid = false;
                while (next_field != 0)
                {
                    if (currMap[next_field] == p.m_symbol && overrides && valid)
                    {
                        p.m_valid_moves.insert(next_field);
                        break;
                    }
                    else if ((check_empty_fields(currMap[next_field]) && valid))
                    {
                        p.m_valid_moves.insert(next_field);
                        break;
                    }
                    else if (valid && overrides)
                    {
                        p.m_valid_moves.insert(next_field);
                    }
                    else if (currMap[next_field] == p.m_symbol || next_field == c || check_empty_fields(currMap[next_field]))
                    {
                        break;
                    }
                    valid = true;
                    temp_trans = next_field;
                    temp_dir = next_direction;
                    next_field = m.get_transition(next_field, next_direction);
                    next_direction = m.get_direction(temp_trans, next_direction);
                    if (temp_trans == next_field && temp_dir == (next_direction + 4) % NUM_OF_DIRECTIONS)
                    {
                        break;
                    }
                }
            }
        }
    }
}
