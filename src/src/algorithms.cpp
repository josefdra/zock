#include "algorithms.hpp"
#include "map.hpp"
#include "timer.hpp"
#include "board.hpp"
#include "evaluator.hpp"
#include "math.h"
#include "logging.hpp"

#define MAX_SEARCH_DEPTH 15

MiniMax::MiniMax(MoveExecuter &move_exec, MoveGenerator &move_gen) : m_move_exec(move_exec), m_move_gen(move_gen) {}

MiniMax::~MiniMax() {}

int MiniMax::minimaxOrParanoidWithPruning(Board &board, int alpha, int beta, int8_t depth, uint8_t player_num, bool sorting, Timer &timer)
{

    try
    {

#ifdef DEBUG
        if (call_count % 1 == 0)
        {
            LOG_INFO("trying move: " + std::to_string(board.get_coord()) + " by player " + std::to_string(player_num + 1) + " depth: " + std::to_string(depth) + " time left: " + std::to_string(timer.return_rest_time()) + " elapsed time " + std::to_string(timer.get_elapsed_time()));
        }
#endif
        if (timer.return_rest_time() < timer.exception_time)
        {
            throw TimeLimitExceededException("Timeout at beginning of minimax recursion.");
        }

        if (depth == 0)
        {

            return get_evaluation(board, player_num, m_move_gen, timer);
        }
        uint32_t call_count = track_number_of_nodes(false);
        uint8_t next_player = (player_num + 1) % m_move_exec.get_num_of_players();
        while (board.disqualified[next_player])
        {
            next_player = (next_player + 1) % m_move_exec.get_num_of_players();
        }
        m_move_gen.calculate_valid_moves(board, player_num, timer);
        uint16_t num_val_moves = board.valid_moves[player_num].count();
        average_branching_factor = calculate_average_branching_factor(num_val_moves, call_count, false);

        uint8_t prev_player = player_num;
        while (board.valid_moves[player_num].count() == 0)
        {
            player_num = (player_num + 1) % m_move_exec.get_num_of_players();
            while (board.disqualified[next_player])
            {
                next_player = (next_player + 1) % m_move_exec.get_num_of_players();
            }
            if (player_num == prev_player)
                return get_evaluation(board, player_num, m_move_gen, timer);
            next_player = (next_player + 1) % m_move_exec.get_num_of_players();
            if (timer.return_rest_time() < timer.exception_time)
            {
                throw TimeLimitExceededException("Timeout in minimax calculating next player with valid moves.");
            }
            m_move_gen.calculate_valid_moves(board, player_num, timer);
        }

        if (timer.return_rest_time() < timer.exception_time)
        {
            throw TimeLimitExceededException(("Timeout in minimax after calculating valid moves."));
        }

        std::vector<Board> boards = generate_boards(board, player_num, timer);
        if (sorting)
        {
            board.valid_moves = sort_valid_moves(board, player_num, timer, boards, player_num == m_move_exec.get_player_num());
        }

        int best_eval;
        if (player_num == m_move_exec.get_player_num())
            best_eval = -INT32_MAX;
        else
            best_eval = INT32_MAX;

        for (auto &b : boards)
        {
            if (timer.return_rest_time() < timer.exception_time)
            {
                throw TimeLimitExceededException(("Timeout in minimax after generating boards and iterating over them."));
            }
            int eval = minimaxOrParanoidWithPruning(b, alpha, beta, depth - 1, next_player, sorting, timer);
            if (player_num == m_move_exec.get_player_num())
            {
                best_eval = std::max(best_eval, eval);
                alpha = std::max(alpha, best_eval);
                if (beta <= alpha)
                    break;
            }
            else
            {
                best_eval = std::min(best_eval, eval);
                beta = std::min(beta, best_eval);
                if (beta <= alpha)
                    break;
            }
        }

        return best_eval;
    }
    catch (TimeLimitExceededException &e)
    {
        throw;
    }
}

/// @brief sorts the valid moves of a player and needs already the calculated valid moves in the board
/// @param board
/// @param player_num
/// @param timer
/// @param maximizer
/// @return sorted valid moves as vector
std::vector<std::bitset<2501>> MiniMax::sort_valid_moves(Board &board, uint8_t player_num, Timer &timer, std::vector<Board> &boards, bool maximizer)
{
    try
    {
        std::vector<std::bitset<2501>> sorted_valid_moves;
        std::vector<std::pair<int, uint16_t>> evals;
        std::vector<std::bitset<2501>> valid_moves = board.valid_moves;

        for (Board &b : boards)
        {
            if (timer.return_rest_time() < timer.exception_time)
            {
                throw TimeLimitExceededException(("Timeout in sort_valid_moves after generating boards and iterating over them."));
            }
            evals.push_back(std::make_pair(get_evaluation(b, player_num, m_move_gen, timer), b.get_coord()));
        }
        if (maximizer)
        {
            std::sort(evals.begin(), evals.end(), [&timer](const std::pair<int, uint16_t> &a, const std::pair<int, uint16_t> &b)
                      { return a.first > b.first; });
        }
        else
        {
            std::sort(evals.begin(), evals.end(), [&timer](const std::pair<int, uint16_t> &a, const std::pair<int, uint16_t> &b)
                      { return a.first < b.first; });
        }
        if (timer.return_rest_time() < timer.exception_time)
        {
            throw TimeLimitExceededException(("Timeout in sort_valid_moves after sorting"));
        }
        sorted_valid_moves.clear();
        for (const auto &eval : evals)
        {
            if (timer.return_rest_time() < timer.exception_time)
            {
                throw TimeLimitExceededException(("Timeout in sort_valid_moves while appending sorted valid moves to vector."));
            }
            sorted_valid_moves.push_back(valid_moves[eval.second]);
        }
        return sorted_valid_moves;
    }
    catch (TimeLimitExceededException)
    {
        throw;
    }
}

double MiniMax::calculate_average_branching_factor(uint16_t num_of_valid_moves, uint32_t count, bool reset)
{

    static uint32_t sum = 0;

    if (!reset)
    {
        sum += num_of_valid_moves;
    }
    else
    {
        sum = 0;
    }
    // LOG_INFO("call_count = " + std::to_string(count) + " sum = " + std::to_string(sum));
    return static_cast<double>(sum) / static_cast<double>(count);
}

uint32_t MiniMax::track_number_of_nodes(bool reset)
{

    static uint32_t count = 0;
    if (!reset)
    {
        count++;
    }
    else
    {
        count = 0;
    }
    return count;
}

std::vector<Board> MiniMax::generate_boards(Board &board, uint8_t player_num, Timer &timer)
{
    std::vector<Board> boards;
    for (uint16_t i = 1; i < m_move_exec.get_num_of_fields(); i++)
    {
#ifdef DEBUG
        if (i > 1000)
            LOG_INFO("time left: " + std::to_string(timer.return_rest_time()) + " i = " + std::to_string(i) + " coord: " + std::to_string(board.get_coord()));
#endif // DEBUG
        if (timer.return_rest_time() < timer.exception_time)
        {
            throw TimeLimitExceededException("Timeout in generate_boards in first for loop.");
        }
        if (board.valid_moves[player_num].test(i))
        {
            if (board.board_sets[3].test(i))
            {
                for (uint8_t j = 0; j < m_move_exec.get_num_of_players(); j++)
                {
#ifdef DEBUG
                    LOG_INFO("time left: " + std::to_string(timer.return_rest_time()) + " j = " + std::to_string(j));
#endif // DEBUG
                    if (timer.return_rest_time() < timer.exception_time)
                    {
                        throw TimeLimitExceededException("Timeout in generate_boards in second for loop.");
                    }
                    boards.push_back(m_move_exec.exec_move(player_num, Board(board, i, j), timer));
                }
            }
            else if (board.board_sets[4].test(i))
            {
                boards.push_back(m_move_exec.exec_move(player_num, Board(board, i, 20), timer));
            }
            else
            {
                boards.push_back(m_move_exec.exec_move(player_num, Board(board, i, 0), timer));
            }
        }
    }

    return boards;
}

void MiniMax::init_best_board(Board &board)
{
    for (uint16_t c = 1; c < board.get_num_of_fields(); c++)
    {
        if (board.valid_moves[m_move_exec.get_player_num()].test(c))
        {
            board.set_coord(c);
            if (board.board_sets[3].test(c))
            {
                board.set_spec(m_move_exec.get_player_num());
            }
            else if (board.board_sets[4].test(c))
            {
                board.set_spec(20);
            }
            else
            {
                board.set_spec(0);
            }
            break;
        }
    }
}

Board MiniMax::get_best_coord(Board &board, Timer &timer, bool sorting)
{
    int best_eval = -INT32_MAX;
    Board best_board(board);
    init_best_board(best_board);
    try
    {
        std::vector<Board> boards = generate_boards(board, m_move_exec.get_player_num(), timer);
        for (uint8_t search_depth = 1; search_depth < MAX_SEARCH_DEPTH; search_depth++)
        {
            Timer measure_depth_search(timer.return_rest_time());
            for (auto &b : boards)
            {
                int eval = minimaxOrParanoidWithPruning(b, -INT32_MAX, INT32_MAX, search_depth, m_move_exec.get_player_num(), sorting, timer);
                if (eval > best_eval)
                {
                    best_eval = eval;
                    best_board = b;
                }
            }

            double estimated_runtime = estimate_runtime_next_depth(search_depth, measure_depth_search);
#ifdef DEBUG
            LOG_INFO("search depth: " + std::to_string(search_depth) + " took " + std::to_string(measure_depth_search.get_elapsed_time()) + " [ms]");
            LOG_INFO("estimated runtime next depth: " + std::to_string(estimated_runtime) + " [ms]");
            LOG_INFO("time left: " + std::to_string(timer.return_rest_time()) + " [ms]");
#endif // DEBUG
            if (estimated_runtime > timer.return_rest_time())
            {
                LOG_INFO("skipping next depth " + std::to_string(search_depth + 1) + " because estimated time exceeds time left.");
                break;
            }
        }
    }
    catch (TimeLimitExceededException &e)
    {
        LOG_INFO("time left: " + std::to_string(timer.return_rest_time()));
        LOG_WARNING(e.what());
    }
    LOG_INFO("best eval: " + std::to_string(best_eval));
    return best_board;
}

double MiniMax::estimate_runtime_next_depth(uint8_t &current_depth, Timer &timer)
{
    if (average_branching_factor != -1.0)
    {
        uint32_t no_nodes = track_number_of_nodes(false);
        double elapsed_time = timer.get_elapsed_time();
        double time_per_node = elapsed_time / (static_cast<double>(no_nodes)); // times 2 because nodes at depth 0 aren't counted
        // LOG_INFO("elapsed time: " + std::to_string(elapsed_time) + " no_nodes: " + std::to_string(no_nodes) + " time per node: " + std::to_string(time_per_node));
        // LOG_INFO("branching average: " + std::to_string(average_branching_factor / 1.5));
        double estimated_nodes_next_depth = pow(static_cast<double>(average_branching_factor / 1.5), static_cast<double>((current_depth + 1)));
        // LOG_INFO("estimated nodes next depth: " + std::to_string(estimated_nodes_next_depth));
        // LOG_INFO("caculating: " + std::to_string(estimated_nodes_next_depth) + " * " + std::to_string(time_per_node));
        track_number_of_nodes(true);
        calculate_average_branching_factor(0, 0, true);
        average_branching_factor = 1;
        return ((estimated_nodes_next_depth * time_per_node) / 1.75);
    }
    else
    {
        return -1.0;
    }
}
