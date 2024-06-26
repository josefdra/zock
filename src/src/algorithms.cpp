#include "algorithms.hpp"
#include "map.hpp"
#include "timer.hpp"
#include "board.hpp"
#include "evaluator.hpp"
#include "logging.hpp"

#define MAX_SEARCH_DEPTH 15
#define ESTIMATED_TIME_DIVISOR 1.75
#define AVERAGE_BRANCHING_FACTOR_DIVISOR 1.5

Algorithms::Algorithms(MoveExecuter &move_exec, MoveGenerator &move_gen) : killer_moves(std::vector<std::vector<uint16_t>>(MAX_SEARCH_DEPTH, std::vector<uint16_t>(move_exec.get_num_of_fields()))), m_move_exec(move_exec), m_move_gen(move_gen)
{
}

Algorithms::~Algorithms() {}

uint8_t Algorithms::get_next_player(uint8_t player_num, Board &board, Timer &timer, uint8_t &index)
{
    uint8_t next_player = player_num;
    do
    {
        next_player = (next_player + 1) % m_move_exec.get_num_of_players();
        if (!board.disqualified[next_player])
        {
            board.valid_moves[next_player].clear();
            board.valid_moves[next_player].resize(board.get_num_of_communities());
            if ((board.communities[index] & board.player_sets[next_player]).count() != 0)
            {
                m_move_gen.calculate_valid_no_ow_moves(board, next_player, index);
                if (board.valid_moves[next_player][index].count() == 0)
                    m_move_gen.calculate_valid_ow_moves(board, next_player, timer, index);
            }
        }

        if (next_player == player_num)
        {
            if (board.valid_moves[next_player][index].count() == 0)
                board.set_final_state();
            return player_num;
        }
    } while (board.disqualified[next_player] || board.valid_moves[next_player][index].count() == 0);
    return next_player;
}

int Algorithms::set_up_best_eval_minimax(uint8_t player_num)
{
    int best_eval;
    if (player_num == m_move_exec.get_player_num())
    {
        best_eval = -INT32_MAX;
    }
    else
        best_eval = INT32_MAX;
    return best_eval;
}

int Algorithms::set_up_best_eval_brs(uint8_t &brs_m, uint8_t player_num)
{
    int best_eval;
    if (player_num == m_move_exec.get_player_num())
    {
        brs_m = 0;
        best_eval = -INT32_MAX;
    }
    else
        best_eval = INT32_MAX;
    return best_eval;
}

int Algorithms::do_move_minimax(Board &board, move &m, int alpha, int beta, uint8_t depth, Timer &timer, Board &prev_board, uint8_t next_player, bool sorting, uint8_t &index)
{
    board.set_coord(std::get<1>(m));
    board.set_spec(std::get<2>(m));
    uint8_t prev_index = index;
    m_move_exec.exec_move(next_player, board, index);
    int eval = minimax(board, alpha, beta, depth - 1, next_player, timer, sorting, index);
    board = prev_board;
    index = prev_index;
    return eval;
}

int Algorithms::do_move_brs(Board &board, move &m, int alpha, int beta, uint8_t brs_m, uint8_t depth, Timer &timer, Board &prev_board, uint8_t next_player, bool sorting, uint8_t &index)
{
    board.set_coord(std::get<1>(m));
    board.set_spec(std::get<2>(m));
    uint8_t prev_index = index;
    m_move_exec.exec_move(next_player, board, index);
    int eval = brs(board, alpha, beta, brs_m, depth - 1, next_player, timer, sorting, index);
    board = prev_board;
    index = prev_index;
    return eval;
}

void Algorithms::get_eval_minimax(Board &board, moves &moves, int alpha, int beta, uint8_t depth, Timer &timer, Board &prev_board, uint8_t next_player, int &best_eval, bool sorting, uint8_t &index)
{
    for (auto &m : moves)
    {
        if (timer.return_rest_time() < timer.exception_time)
            throw TimeLimitExceededException("Timeout in get_eval_minimax");

        int eval;
        if (board.num_of_players_in_community[index] > 2)
        {
            uint8_t brs_m;
            if (next_player == m_move_exec.get_player_num())
                brs_m = 0;
            else
                brs_m = 1;
            eval = do_move_brs(board, m, alpha, beta, brs_m, depth, timer, prev_board, next_player, sorting, index);
        }
        else
            eval = do_move_minimax(board, m, alpha, beta, depth, timer, prev_board, next_player, sorting, index);
        if (next_player == m_move_exec.get_player_num())
        {
            best_eval = std::max(best_eval, eval);
            alpha = std::max(alpha, best_eval);
            if (beta <= alpha)
            {
                killer_moves[depth][std::get<1>(m)] += 1;
                break;
            }
        }
        else
        {
            best_eval = std::min(best_eval, eval);
            beta = std::min(beta, best_eval);
            if (beta <= alpha)
            {
                killer_moves[depth][std::get<1>(m)] += 1;
                break;
            }
        }
    }
}

void Algorithms::get_eval_brs(Board &board, moves &moves, int alpha, int beta, uint8_t brs_m, uint8_t depth, Timer &timer, Board &prev_board, uint8_t next_player, int &best_eval, bool sorting, uint8_t &index)
{
    for (auto &m : moves)
    {
        if (timer.return_rest_time() < timer.exception_time)
            throw TimeLimitExceededException("Timeout in get_eval_brs");

        int eval;
        if (board.num_of_players_in_community[index] > 2)
            eval = do_move_brs(board, m, alpha, beta, brs_m + 1, depth, timer, prev_board, next_player, sorting, index);
        else
            eval = do_move_minimax(board, m, alpha, beta, depth, timer, prev_board, next_player, sorting, index);
        if (next_player == m_move_exec.get_player_num())
        {
            best_eval = std::max(best_eval, eval);
            alpha = std::max(alpha, best_eval);
            if (beta <= alpha)
            {
                killer_moves[depth][std::get<1>(m)] += 1;
                break;
            }
        }
        else
        {
            best_eval = std::min(best_eval, eval);
            beta = std::min(beta, best_eval);
            if (beta <= alpha)
            {
                killer_moves[depth][std::get<1>(m)] += 1;
                break;
            }
        }
    }
}

move Algorithms::get_first_move(moves &moves)
{
    for (auto &m : moves)
        return m;

    return move();
}

int Algorithms::minimax(Board &board, int alpha, int beta, uint8_t depth, uint8_t player_num, Timer &timer, bool sorting, uint8_t &index)
{
    Board prev_board = board;
    try
    {
        static int call_count = 0;
        call_count++;
#ifdef DEBUG
        LOG_INFO("trying move: " + std::to_string(board.get_coord()) + " by player " + std::to_string(player_num + 1) + " depth: " + std::to_string(depth) + " time left: " + std::to_string(timer.return_rest_time()) + " elapsed time " + std::to_string(timer.get_elapsed_time()));
#endif
        uint8_t next_player = get_next_player(player_num, board, timer, index);
        if (depth == 0 || board.is_final_state())
            return get_evaluation(board, player_num, timer);

        moves moves;
        moves.reserve(MEMORY_SIZE_WITH_BUFFER);
        set_up_moves(board, next_player, moves, index);
        total_nodes += moves.size();
        total_valid_moves++;
        calculate_average_branching_factor(false);

        if (sorting)
            sort_valid_moves(board, next_player, moves, timer, depth);
        int best_eval = set_up_best_eval_minimax(player_num);
        get_eval_minimax(board, moves, alpha, beta, depth, timer, prev_board, next_player, best_eval, sorting, index);
        return best_eval;
    }
    catch (const TimeLimitExceededException &)
    {
        throw;
    }
}

int Algorithms::brs(Board &board, int alpha, int beta, uint8_t brs_m, uint8_t depth, uint8_t player_num, Timer &timer, bool sorting, uint8_t &index)
{
    Board prev_board = board;
    try
    {
        static int call_count = 0;
        call_count++;
#ifdef DEBUG
        LOG_INFO("trying move: " + std::to_string(board.get_coord()) + " by player " + std::to_string(player_num + 1) + " depth: " + std::to_string(depth) + " time left: " + std::to_string(timer.return_rest_time()) + " elapsed time " + std::to_string(timer.get_elapsed_time()));
#endif
        uint8_t next_player = get_next_player(player_num, board, timer, index);
        if (depth == 0 || board.is_final_state())
            return get_evaluation(board, player_num, timer);

        moves moves;
        moves.reserve(MEMORY_SIZE_WITH_BUFFER);
        set_up_moves(board, next_player, moves, index);
        total_nodes += moves.size();
        total_valid_moves++;
        calculate_average_branching_factor(false);

        if (sorting)
            sort_valid_moves(board, next_player, moves, timer, depth);
        int best_eval = set_up_best_eval_brs(brs_m, player_num);
        if (brs_m < 2)
            get_eval_brs(board, moves, alpha, beta, brs_m, depth, timer, prev_board, next_player, best_eval, sorting, index);

        if (brs_m > 0)
        {
            move first_move = get_first_move(moves);
            do_move_brs(board, first_move, alpha, beta, brs_m, depth, timer, prev_board, next_player, sorting, index);
        }

        return best_eval;
    }
    catch (const TimeLimitExceededException &)
    {
        throw;
    }
}

void Algorithms::set_up_killer(moves &moves, uint8_t depth)
{
    std::vector<std::tuple<uint16_t, uint8_t>> temp_killer;
    for (uint16_t c = 1; c < m_move_exec.get_num_of_fields(); c++)
        if (killer_moves[depth][c] > 0)
            temp_killer.push_back(std::make_tuple(c, killer_moves[depth][c]));

    std::sort(temp_killer.begin(), temp_killer.end(), [](const std::tuple<uint16_t, uint8_t> &a, const std::tuple<uint16_t, uint8_t> &b)
              { return std::get<0>(a) > std::get<0>(b); });
    uint16_t smallest_killer_coord;
    uint16_t i = 0;
    while (!temp_killer.empty())
    {
        smallest_killer_coord = std::get<0>(temp_killer.back());
        while (std::get<1>(moves[i]) != smallest_killer_coord && i < moves.size())
            i++;

        while (std::get<1>(moves[i]) == smallest_killer_coord && i < moves.size())
        {
            std::get<0>(moves[i]) = std::get<1>(temp_killer[temp_killer.size() - 1]) * -10000;
            i++;
        }
        temp_killer.pop_back();
    }
}

/// @brief sorts the valid moves of a player and needs already the calculated valid moves in the board
/// @param board
/// @param player_num
/// @param timer
/// @param maximizer
/// @return sorted valid moves as vector
void Algorithms::sort_valid_moves(Board &board, uint8_t player_num, moves &moves, Timer &timer, uint8_t depth)
{
    Board prev_board = board;
    try
    {
        set_up_killer(moves, depth);
        if (player_num == m_move_exec.get_player_num())
            for (auto &m : moves)
                if (std::get<0>(m) != 0)
                    std::get<0>(m) = std::get<0>(m) * -1;

        for (auto &m : moves)
        {
            if (timer.return_rest_time() < timer.exception_time)
                throw TimeLimitExceededException("Timeout in sort_valid_moves before sorting");

            board.set_coord(std::get<1>(m));
            board.set_spec(std::get<2>(m));
            std::get<0>(m) += m_move_exec.get_bits_to_update(player_num, board).count();
        }
        if (player_num == m_move_exec.get_player_num())
            std::sort(moves.begin(), moves.end(), [](const move_tuple &a, const move_tuple &b)
                      { return std::get<0>(a) > std::get<0>(b); });

        else
            std::sort(moves.begin(), moves.end(), [](const move_tuple &a, const move_tuple &b)
                      { return std::get<0>(a) < std::get<0>(b); });

        if (timer.return_rest_time() < timer.exception_time)
            throw TimeLimitExceededException(("Timeout in sort_valid_moves after sorting"));
    }
    catch (const TimeLimitExceededException &)
    {
        throw;
    }
}

void Algorithms::set_up_moves(Board &board, uint8_t player_num, moves &moves, uint8_t index)
{
    for (uint16_t c = 1; c < m_move_exec.get_num_of_fields(); c++)
        if (board.valid_moves[player_num][index].test(c))
        {
            if (board.board_sets[C].test(c))
                for (uint8_t j = 0; j < m_move_exec.get_num_of_players(); j++)
                    moves.push_back(std::make_tuple(ZERO_EVALUATION, c, j));

            else if (board.board_sets[B].test(c))
                moves.push_back(std::make_tuple(ZERO_EVALUATION, c, OVERWRITE_SPEC));

            else
                moves.push_back(std::make_tuple(ZERO_EVALUATION, c, 0));
        }
}

void Algorithms::init_best_board(Board &board)
{
    std::bitset<MAX_NUM_OF_FIELDS> total_moves = board.get_total_moves(m_move_exec.get_player_num());
    LOG_INFO("total moves: " + std::to_string(total_moves.count()));
    for (uint16_t c = 1; c < board.get_num_of_fields(); c++)
        if (total_moves.test(c))
        {
            board.set_coord(c);
            if (board.board_sets[C].test(c))
                board.set_spec(m_move_exec.get_player_num());

            else if (board.board_sets[B].test(c))
                board.set_spec(OVERWRITE_SPEC);

            else
                board.set_spec(0);
            break;
        }
}

void Algorithms::sort_best_moves_and_communities_to_front(Board &board, std::vector<uint16_t> &best_move_in_community_index, uint8_t &best_community_index, moves_vector &moves)
{
    if (board.get_num_of_communities() > 1)
    {
        std::bitset<MAX_NUM_OF_FIELDS> temp_community = board.communities[best_community_index];
        std::bitset<MAX_NUM_OF_FIELDS> temp_frame = board.frames[best_community_index];
        uint16_t temp_move_index = best_move_in_community_index[best_community_index];
        for (uint8_t i = best_community_index; i > 0; i--)
        {
            board.communities[i] = board.communities[i - 1];
            board.frames[i] = board.frames[i - 1];
            best_move_in_community_index[i] = best_move_in_community_index[i - 1];
        }
        board.communities[0] = temp_community;
        board.frames[0] = temp_frame;
        best_move_in_community_index[0] = temp_move_index;
    }

    for (uint8_t i = 0; i < board.get_num_of_communities(); i++)
    {
        if (moves[i].size() == 0)
            continue;
        std::tuple<int, uint16_t, uint8_t> temp_move = moves[i][best_move_in_community_index[i]];
        for (uint8_t j = best_move_in_community_index[i]; j > 0; j--)
            moves[i][j] = moves[i][j - 1];

        moves[i][0] = temp_move;
    }
}

Board Algorithms::get_best_coord(Board &board, Timer &timer, bool sorting)
{
    int best_eval = INT32_MIN;
    Board best_board(board);
    init_best_board(best_board);
    uint8_t player_num = m_move_exec.get_player_num();
    moves_vector moves(board.get_num_of_communities());
    moves.reserve(MEMORY_SIZE_WITH_BUFFER);
    Board prev_board = board;
    int alpha = INT32_MIN;
    int beta = INT32_MAX;
    uint8_t search_depth;
    std::vector<int> best_community_eval(board.get_num_of_communities(), INT32_MIN);
    std::vector<uint16_t> best_move_in_community_index(board.get_num_of_communities(), 0);
    uint8_t best_community_index = 0;

    try
    {
        for (uint8_t community_index = 0; community_index < board.get_num_of_communities(); community_index++)
        {
            if (board.valid_moves[player_num][community_index].count() == 0)
                continue;
            set_up_moves(board, player_num, moves[community_index], community_index);
            if (sorting)
                sort_valid_moves(board, player_num, moves[community_index], timer, 0);
        }
        for (search_depth = 0; search_depth < MAX_SEARCH_DEPTH; search_depth++)
        {
            Timer measure_depth_search(timer.return_rest_time());
            for (uint8_t community_index = 0; community_index < board.get_num_of_communities(); community_index++)
            {
                if (moves[community_index].size() == 0)
                    continue;
                
                if (search_depth == 0)
                    total_valid_moves += board.valid_moves[player_num][community_index].count();
                for (uint16_t move_index = 0; move_index < moves[community_index].size(); move_index++)
                {
                    if (timer.return_rest_time() < timer.exception_time)
                        throw TimeLimitExceededException(("Timeout in get_best_coord"));

                    board.set_coord(std::get<1>(moves[community_index][move_index]));
                    board.set_spec(std::get<2>(moves[community_index][move_index]));
                    uint8_t prev_index = community_index;
                    m_move_exec.exec_move(player_num, board, community_index);
                    int eval;
                    if (board.num_of_players_in_community[community_index] > 2)
                        eval = brs(board, alpha, beta, 0, search_depth, player_num, timer, sorting, community_index);
                    else
                        eval = minimax(board, alpha, beta, search_depth, player_num, timer, sorting, community_index);

                    if (eval > best_eval)
                    {
                        best_eval = eval;
                        best_board = board;
                        best_community_index = community_index;
                    }
                    if (eval > best_community_eval[community_index])
                    {
                        best_community_eval[community_index] = eval;
                        best_move_in_community_index[community_index] = move_index;
                    }
                    board = prev_board;
                    community_index = prev_index;
                }
            }
            sort_best_moves_and_communities_to_front(board, best_move_in_community_index, best_community_index, moves);
            double estimated_runtime = estimate_runtime_next_depth(search_depth, measure_depth_search);
            if (estimated_runtime > timer.return_rest_time())
            {
                LOG_INFO("skipping next depth " + std::to_string(search_depth + 1) + " because estimated time exceeds time left.");
                break;
            }
        }
    }
    catch (TimeLimitExceededException &e)
    {
        board = prev_board;
        LOG_INFO("time left: " + std::to_string(timer.return_rest_time()));
        LOG_WARNING(e.what());
    }
    LOG_INFO("best eval: " + std::to_string(best_eval));
    return best_board;
}

void Algorithms::calculate_average_branching_factor(bool reset)
{
    if (reset)
    {
        total_valid_moves = 0;
    }
    // LOG_INFO("sum: " + std::to_string(sum));
    average_branching_factor = static_cast<double>(total_nodes) / static_cast<double>(total_valid_moves);
}

double Algorithms::estimate_runtime_next_depth(uint8_t &current_depth, Timer &timer)
{
    if (average_branching_factor != -1.0)
    {
        double elapsed_time = timer.get_elapsed_time();
        double time_per_node = elapsed_time / (static_cast<double>(total_nodes));
        double estimated_nodes_next_depth = pow(static_cast<double>(average_branching_factor), static_cast<double>((current_depth + 1)));
#ifdef DEBUG

        LOG_INFO("elapsed time: " + std::to_string(elapsed_time) + " no_nodes: " + std::to_string(total_nodes) + " time per node: " + std::to_string(time_per_node));
        LOG_INFO("branching average: " + std::to_string(average_branching_factor / 1.5));
        LOG_INFO("estimated nodes next depth: " + std::to_string(estimated_nodes_next_depth));
        LOG_INFO("caculating: " + std::to_string(estimated_nodes_next_depth) + " * " + std::to_string(time_per_node));
#endif // DEBUG
        calculate_average_branching_factor(true);
        average_branching_factor = 1;
        total_nodes = 0;
        return ((estimated_nodes_next_depth * time_per_node) / ESTIMATED_TIME_DIVISOR);
    }
    else
    {
        return -1.0;
    }
}
