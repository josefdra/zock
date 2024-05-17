#include "algorithms.hpp"
#define TIME_BUFFER 100

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
        else if (check_players(currMap[c]))
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
            calculate_valid_moves(m, pl, currMap, p.m_valid_moves);
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
    mobility *= m.m_mobility_multiplicator;
    corners_and_special_value *= m.m_corners_and_special_multiplicator;
    p.m_points *= 50 * m.m_stone_multiplicator;
    p.m_board_value += mobility + corners_and_special_value + p.m_points;
    return p.m_board_value;
}

int simple_eval(Player &p, std::vector<char> &currMap, Map &m)
{
    int eval = 0;
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
        // Stone counter
        else if (check_players(currMap[c]))
        {
            if (currMap[c] != p.m_symbol)
            {
                eval -= 10;
            }
            else
            {
                eval += 10;
            }
        }
    }
    eval *= 10 * m.m_stone_multiplicator;
    return eval;
}

int minimaxOrParanoidWithPruning(Map &m, std::vector<Player> &players, uint8_t depth, int alpha, int beta, std::vector<char> &currMap, uint8_t &playersTurn, uint8_t &game_phase, uint16_t &turns, const bool sort, std::chrono::high_resolution_clock::time_point &start_time, double &delta)
{
    turns++;
    std::unordered_set<uint16_t> valid_moves;
    auto current_time = std::chrono::high_resolution_clock::now();
    auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - start_time).count();
    if (turns % 500 == 0)
    {
        std::cout << "remaining time in minimax: " << delta << std::endl;
    }

    if (elapsed_time >= delta - TIME_BUFFER)
    {
        throw TimeoutException("Time limit exceeded in minimax!");
    }

    double time_remaining = delta - elapsed_time;

    if (depth == 0)
    {
        players[m.m_player_number].m_valid_moves = valid_moves;
        return evaluate_board(game_phase, players[m.m_player_number], currMap, m, players);
    }

    uint8_t nextPlayer = ((playersTurn + 1) % m.m_player_count);
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
        players[m.m_player_number].m_valid_moves = valid_moves;
        return evaluate_board(game_phase, players[m.m_player_number], currMap, m, players);
    }

    if (sort)
    {
        std::vector<std::pair<int, uint16_t>> move_values;

        for (const auto &move : valid_moves)
        {
            std::vector<char> this_map = currMap;
            std::vector<char> next_map = temp_color(move, players[(playersTurn + counter) % m.m_player_count].m_symbol, m, currMap);
            move_values.push_back({simple_eval(players[m.m_player_number], next_map, m), move});
            currMap = this_map;
        }
        if (players[m.m_player_number].m_symbol == players[playersTurn].m_symbol)
        {
            std::sort(move_values.begin(), move_values.end(), [](const std::pair<int, uint16_t> move1, const std::pair<int, uint16_t> move2)
                      { return move1.first > move2.first; });
        }
        else
        {
            std::sort(move_values.begin(), move_values.end(), [](const std::pair<int, uint16_t> move1, const std::pair<int, uint16_t> move2)
                      { return move1.first < move2.first; });
        }
        valid_moves.clear();
        for (const auto &move_values : move_values)
        {
            valid_moves.insert(move_values.second);
        }
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
                    currEval = minimaxOrParanoidWithPruning(m, players, depth - 1, alpha, beta, next_map, nextPlayer, game_phase, turns, sort, current_time, time_remaining);
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
            eval = minimaxOrParanoidWithPruning(m, players, depth - 1, alpha, beta, next_map, nextPlayer, game_phase, turns, sort, current_time, time_remaining);
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
