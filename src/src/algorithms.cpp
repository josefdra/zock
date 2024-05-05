#include "algorithms.hpp"

void get_frontier_score(Player &p, std::vector<char> &currMap, Map &m)
{
    for (uint16_t c = 1; c < m.m_num_of_fields; c++)
    {
        if (currMap[c] == p.m_symbol)
        {
            p.m_frontier_score += check_frontier(m, c, currMap);
        }
    }
}

int evaluate_board(uint8_t game_phase, Player &p, std::vector<char> &currMap, Map &m, std::vector<Player> &players)
{
    std::unordered_set<uint16_t> valid_moves;
    p.m_board_value = 0;
    p.m_frontier_score = 0;
    for (auto &pl : players)
    {
        pl.m_points = 0;
    }
    for (uint16_t c = 1; c < m.m_num_of_fields; c++)
    {
        if (currMap[c] == p.m_symbol)
        {
            p.m_board_value += m.m_constant_board_values[c];
            p.m_points++;
        }
        else if (check_players(currMap[c]))
        {
            players[currMap[c] - 1 - '0'].m_points += 1;
        }
    }
    if (game_phase == 0)
    {
        p.m_points *= 30;
    }
    else if (game_phase == 1)
    {
        p.m_points *= 60;
    }
    for (auto &pl : players)
    {
        if (pl.m_points == 0 && pl.m_symbol != p.m_symbol)
        {
            p.m_points += 100000;
        }
        if (pl.m_points < p.m_points)
        {
            p.m_points += p.m_points;
        }
    }
    // get_frontier_score(p, currMap, g.m_map);
    p.m_board_value += p.m_frontier_score;
    p.m_board_value += p.m_points * 10;
    p.m_board_value += valid_moves.size() * 50;
    if (valid_moves.size() < 1)
    {
        p.m_board_value -= 100000;
    }
    for (auto &pl : players)
    {
        if (pl.m_symbol != m.m_player_number + 1 + '0')
        {
            calculate_valid_moves(m, pl, currMap, valid_moves);
        }
        if (valid_moves.size() < 1)
        {
            p.m_board_value += 5000;
        }
    }
    return p.m_board_value;
}

// this algorithm could be optimized by adding functionality to check if enemy has direct influence in our player and if he can "attack" us, else his
// possible turns could be ignored
int minimaxOrParanoidWithPruning(Map &m, std::vector<Player> &players, uint8_t depth, int alpha, int beta, std::vector<char> &currMap, uint8_t &playersTurn, uint8_t &game_phase, uint16_t &turns)
{
    turns++;

    if (depth == 0)
    {
        return evaluate_board(game_phase, players[m.m_player_number], currMap, m, players);
    }

    uint8_t nextPlayer = ((playersTurn + 1) % m.m_player_count);
    std::unordered_set<uint16_t> valid_moves;
    calculate_valid_moves(m, players[playersTurn], currMap, valid_moves);

    uint8_t counter = 0;
    while (valid_moves.size() < 1 && counter < m.m_player_count)
    {
        counter++;
        nextPlayer = ((playersTurn + 1) % m.m_player_count);
        calculate_valid_moves(m, players[(playersTurn + counter) % m.m_player_count], currMap, valid_moves);
    }
    if (counter == m.m_player_count)
    {
        return evaluate_board(game_phase, players[m.m_player_number], currMap, m, players);
    }

    int maxEval;
    int minEval;
    for (auto &move : valid_moves)
    {
        int eval = -INT32_MAX, currEval;
        if (m.m_symbols[move] == 'c')
        {
            for (auto &p : players)
            {
                if (p.m_symbol != players[m.m_player_number].m_symbol)
                {
                    std::vector<char> next_map = temp_color(move, players[m.m_player_number].m_symbol, m, currMap);
                    change_players(next_map, players[m.m_player_number].m_symbol, p.m_symbol);
                    currEval = minimaxOrParanoidWithPruning(m, players, depth - 1, alpha, beta, next_map, nextPlayer, game_phase, turns);
                    if (currEval > eval)
                    {
                        eval = currEval;
                    }
                }
            }
        }
        else
        {
            std::vector<char> next_map = temp_color(move, players[(playersTurn + counter) % m.m_player_count].m_symbol, m, currMap);
            eval = minimaxOrParanoidWithPruning(m, players, depth - 1, alpha, beta, next_map, nextPlayer, game_phase, turns);
        }

        if (players[(playersTurn + counter) % m.m_player_count].m_symbol == players[m.m_player_number].m_symbol)
        {
            maxEval = std::numeric_limits<int>::min();
            maxEval = std::max(maxEval, eval);
            alpha = std::max(alpha, eval);
            if (beta <= alpha)
            {
                break;
            }
        }
        else
        {
            minEval = std::numeric_limits<int>::max();
            minEval = std::min(minEval, eval);
            beta = std::min(beta, eval);
            if (beta <= alpha)
            {
                break;
            }
        }
    }
    if (players[(playersTurn + counter) % m.m_player_count].m_symbol == players[m.m_player_number].m_symbol)
    {
        return maxEval;
    }
    else
    {
        return minEval;
    }
}
