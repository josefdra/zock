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

int evaluate_board(uint8_t game_phase, Player &p, Map &m, std::vector<char> &currMap)
{
    p.m_board_value = 0;

    uint8_t value = 10;
    uint8_t bonus_value = 30;
    uint8_t choice_value = 20 * (game_phase + 1);
    uint8_t inversion_value = 30;
    uint8_t before_special_value = 30;
    uint16_t player_points = 0;
    std::array<int, 9> wall_values{0, 1, 2, 3, 16, 12, 8, 4, 0};
    for (uint16_t c = 1; c < m.m_num_of_fields; c++)
    {
        m.m_good_fields[c] = 0;
        m.m_bad_fields[c] = 0;
        if (currMap[c] == p.m_symbol)
        {
            player_points++;
        }
    }
    for (uint16_t c = 1; c < m.m_num_of_fields; c++)
    {
        bool special = false;
        char s = currMap[c];
        switch (s)
        {
        case 'b':
            m.m_good_fields[c] += bonus_value;
            special = true;
            break;
        case 'c':
            m.m_good_fields[c] += choice_value;
            special = true;
            break;
        case 'i':
            m.m_bad_fields[c] -= inversion_value;
            special = true;
            break;
        default:
            break;
        }
        uint8_t value_to_add = 0;
        uint8_t walls = 0;
        for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d++)
        {
            if (m.get_transition(c, d) == 0)
            {
                walls++;
                uint16_t trans = m.get_transition(c, (d + 4) % 8);
                if (trans != 0)
                {
                    m.m_bad_fields[trans] -= 10;
                }
            }
            if (special == true)
            {
                uint16_t trans = m.get_transition(c, d);
                if (trans != 0)
                {
                    m.m_bad_fields[trans] -= before_special_value;
                }
            }
        }
        m.m_good_fields[c] += wall_values[walls] * value;
    }
    for (uint16_t c = 1; c < m.m_num_of_fields; c++)
    {
        if (m.get_symbol(c) == p.m_symbol)
        {
            p.m_board_value += m.m_good_fields[c] + m.m_bad_fields[c];
        }
    }
    if (game_phase == 0)
    {
        player_points *= 5;
    }
    else if (game_phase == 1)
    {
        player_points *= 10;
    }
    get_frontier_score(p, currMap, m);
    p.m_board_value += p.m_frontier_score;
    p.m_board_value += player_points;
    return p.m_board_value;
}

// this algorithm could be optimized by adding functionality to check if enemy has direct influence in our player and if he can "attack" us, else his
// possible turns could be ignored
int minimaxOrParanoidWithPruning(Game &g, uint8_t depth, int alpha, int beta, bool maximizingPlayer, std::vector<char> &currMap, uint8_t &playersTurn, uint8_t &game_phase, Player &myPlayer, uint16_t &turns)
{
    bool affectsMyPlayer = false;
    int maxEval = std::numeric_limits<int>::min();
    int minEval = std::numeric_limits<int>::max();
    turns++;

    Player currPlayer = g.m_players[playersTurn];
    check_moves(g.m_map, currPlayer, currMap, affectsMyPlayer, myPlayer.m_symbol);
    uint8_t nextPlayer = ((playersTurn + 1) % g.m_map.m_player_count);

    if (depth == 0 || currPlayer.m_valid_moves.size() < 1)
    {
        return evaluate_board(game_phase, myPlayer, g.m_map, currMap);
    }

    /*
    // check if current player has any influence on our player else skip his moves
    if (g.m_map.m_player_count > 2)
    {
        if (currPlayer.m_symbol != myPlayer.m_symbol && !affectsMyPlayer)
        {
            // warum nicht depth - 1?
            minimaxOrParanoidWithPruning(g, depth, alpha, beta, maximizingPlayer, map, game, nextPlayer, game_phase, myPlayer, turns);
        }
    }
    */

    if (maximizingPlayer)
    {
        for (auto &child : currPlayer.m_valid_moves)
        {
            int eval = minimaxOrParanoidWithPruning(g, depth - 1, alpha, beta, false, child.second, nextPlayer, game_phase, myPlayer, turns);
            maxEval = std::max(maxEval, eval);
            alpha = std::max(alpha, eval);
            if (beta <= alpha)
            {
                break;
            }
        }
        return maxEval;
    }
    else
    {
        for (auto &child : currPlayer.m_valid_moves)
        {
            int eval;
            // initializes paranoia
            if (g.m_players[playersTurn].m_symbol == myPlayer.m_symbol)
            {
                eval = minimaxOrParanoidWithPruning(g, depth - 1, alpha, beta, true, child.second, nextPlayer, game_phase, myPlayer, turns);
            }
            else
            {
                eval = minimaxOrParanoidWithPruning(g, depth - 1, alpha, beta, false, child.second, nextPlayer, game_phase, myPlayer, turns);
            }

            minEval = std::min(minEval, eval);
            beta = std::min(beta, eval);
            if (beta <= alpha)
            {
                break;
            }
        }
        return minEval;
    }
}
