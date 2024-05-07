#include "algorithms.hpp"

void get_frontier_score(Player &p, std::vector<char> &currMap, Map &m)
{
    for (uint16_t c = 1; c < m.m_num_of_fields; c++)
    {
        if (currMap[c] == p.m_symbol)
        {
            p.m_frontier_score = p.m_frontier_score + check_frontier(m, c, currMap);
        }
    }
}

int evaluate_board(uint8_t game_phase, Player &p, std::vector<char> &currMap, Map &m, std::vector<Player> &players)
{
    int corners_and_special_value = 0;
    int mobility = 0;
    p.m_board_value = 0;
    p.m_frontier_score = 0;
    for (auto &pl : players)
    {
        pl.m_points = 0;
    }
    for (uint16_t c = 1; c < m.m_num_of_fields; c++)
    {
        // Bonus Fields
        if (currMap[c] == 'c' || currMap[c] == 'b')
        {
            m.m_variable_board_values[c] = 200;
            for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d++)
            {
                if (uint16_t trans = m.get_transition(c, d) != 0)
                {
                    m.m_variable_board_values[trans] = -200;
                }
            }
        }
        // Corners and Borders
        if (currMap[c] == p.m_symbol)
        {
            corners_and_special_value += m.m_constant_board_values[c];
            corners_and_special_value += m.m_variable_board_values[c];
        }
        // Stone counter
        if (check_players(currMap[c]))
        {
            players[currMap[c] - 1 - '0'].m_points += 1;
        }
    }
    for (auto &pl : players)
    {
        if (pl.m_points == 0 && pl.m_symbol != p.m_symbol)
        {
            p.m_board_value += 100000;
        }
        else if (pl.m_points == 0 && pl.m_symbol == p.m_symbol)
        {
            p.m_board_value -= 1000000;
        }
        if (pl.m_points < p.m_points)
        {
            if (game_phase == 0)
            {
                p.m_points = p.m_points / 2;
            }
            else if (game_phase == 1)
            {
                p.m_points = p.m_points * 2;
            }
        }
        if (pl.m_symbol != p.m_symbol)
        {
            calculate_valid_moves(m, pl, currMap);
        }
    }
    if (p.m_valid_moves.size() < 1)
    {
        p.m_board_value = p.m_board_value - 100000;
    }
    mobility = p.m_valid_moves.size() * 50;
    for (auto &pl : players)
    {
        if (p.m_valid_moves.size() < pl.m_valid_moves.size())
        {
            mobility = mobility - p.m_points * 25;
        }
        else
        {
            mobility = mobility + p.m_points * 25;
        }
    }
    mobility = mobility * p.m_mobility_multiplicator;
    corners_and_special_value = corners_and_special_value * p.m_corners_and_special_multiplicator;
    p.m_points = p.m_points * 50 * p.m_stone_multiplicator;
    p.m_board_value = p.m_board_value + mobility + corners_and_special_value + p.m_points;
    return p.m_board_value;
}
