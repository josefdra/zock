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

int evaluate_board(uint8_t game_phase, Player &p, std::vector<char> &currMap, Game &g)
{
    p.m_board_value = 0;
    p.m_frontier_score = 0;
    for (auto &pl : g.m_players)
    {
        pl.m_points = 0;
    }
    for (uint16_t c = 1; c < g.m_map.m_num_of_fields; c++)
    {
        if (currMap[c] == p.m_symbol)
        {
            p.m_board_value += g.m_map.m_constant_board_values[c];
            p.m_points++;
        }
        else if (check_players(currMap[c]))
        {
            g.m_players[currMap[c] - 1 - '0'].m_points += 1;
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
    for (auto &pl : g.m_players)
    {
        if(pl.m_points == 0 && pl.m_symbol != p.m_symbol){
            p.m_points += 100000;
        }
    }
    // get_frontier_score(p, currMap, g.m_map);
    p.m_board_value += p.m_frontier_score;
    p.m_board_value += p.m_points;
    p.m_board_value += p.m_valid_moves.size() * 20;
    if (p.m_valid_moves.size() < 1)
    {
        p.m_board_value -= 100000;
    }
    for (auto &pl : g.m_players)
    {
        if (pl.m_symbol != g.m_player_number + 1 + '0')
        {
            calculate_valid_moves(g.m_map, pl, currMap);
        }
        if (pl.m_valid_moves.size() < 1)
        {
            p.m_board_value += 5000;
        }
    }
    return p.m_board_value;
}

// this algorithm could be optimized by adding functionality to check if enemy has direct influence in our player and if he can "attack" us, else his
// possible turns could be ignored
int minimaxOrParanoidWithPruning(Game &g, uint8_t depth, int alpha, int beta, std::vector<char> &currMap, uint8_t &playersTurn, uint8_t &game_phase, uint16_t &turns)
{
    turns++;

    if (depth == 0)
    {
        return evaluate_board(game_phase, g.m_players[g.m_player_number], currMap, g); 
    }

    Player currPlayer = g.m_players[playersTurn];
    uint8_t nextPlayer = ((playersTurn + 1) % g.m_map.m_player_count);
    calculate_valid_moves(g.m_map, currPlayer, currMap);

    uint8_t counter = g.m_map.m_player_count;
    while (currPlayer.m_valid_moves.size() < 1 && counter != 0)
    {
        currPlayer = g.m_players[(playersTurn + 1) % g.m_map.m_player_count];
        nextPlayer = ((playersTurn + 1) % g.m_map.m_player_count);
        calculate_valid_moves(g.m_map, currPlayer, currMap);
        counter--;
    }
    if (counter == 0)
    {
        return evaluate_board(game_phase, g.m_players[g.m_player_number], currMap, g);
    }

    int maxEval;
    int minEval;
    for (auto &move : currPlayer.m_valid_moves)
    {
        std::vector<char> next_map = temp_color(move, currPlayer.m_symbol, g.m_map, currMap, g.m_players[g.m_player_number].m_symbol);
        int eval = minimaxOrParanoidWithPruning(g, depth - 1, alpha, beta, next_map, nextPlayer, game_phase, turns);
        if (currPlayer.m_symbol == g.m_players[g.m_player_number].m_symbol)
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
    if (currPlayer.m_symbol == g.m_players[g.m_player_number].m_symbol)
    {
        return maxEval;
    }
    else
    {
        return minEval;
    }
}

std::vector<uint16_t> sort_moves(std::unordered_set<uint16_t> &moves)
{
}

int iterativeDeepening(Game &g, uint8_t depth, int alpha, int beta, std::vector<char> &currMap, uint8_t &playersTurn, uint8_t &game_phase, uint16_t &turns)
{
    turns++;

    if (depth == 0)
    {
        return evaluate_board(game_phase, g.m_players[g.m_player_number], currMap, g);
    }

    Player currPlayer = g.m_players[playersTurn];
    uint8_t nextPlayer = ((playersTurn + 1) % g.m_map.m_player_count);
    calculate_valid_moves(g.m_map, currPlayer, currMap);

    uint8_t counter = g.m_map.m_player_count;
    while (currPlayer.m_valid_moves.size() < 1 && counter != 0)
    {
        currPlayer = g.m_players[(playersTurn + 1) % g.m_map.m_player_count];
        nextPlayer = ((playersTurn + 1) % g.m_map.m_player_count);
        calculate_valid_moves(g.m_map, currPlayer, currMap);
        counter--;
    }
    if (counter == 0)
    {
        return evaluate_board(game_phase, g.m_players[g.m_player_number], currMap, g);
    }

    std::vector<uint16_t> sorted_moves = sort_moves(currPlayer.m_valid_moves);

    int maxEval;
    int minEval;
    for (auto &move : currPlayer.m_valid_moves)
    {
        std::vector<char> next_map = temp_color(move, currPlayer.m_symbol, g.m_map, currMap, g.m_players[g.m_player_number].m_symbol);
        int eval = minimaxOrParanoidWithPruning(g, depth - 1, alpha, beta, next_map, nextPlayer, game_phase, turns);
        if (currPlayer.m_symbol == g.m_players[g.m_player_number].m_symbol)
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
    if (currPlayer.m_symbol == g.m_players[g.m_player_number].m_symbol)
    {
        return maxEval;
    }
    else
    {
        return minEval;
    }
}
