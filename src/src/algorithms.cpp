#include "algorithms.hpp"
#include "map.hpp"
#include "timer.hpp"
#include "board.hpp"
#include "evaluator.hpp"
#include "logging.hpp"

#define MAX_SEARCH_DEPTH 15

MiniMax::MiniMax(MoveExecuter &move_exec, MoveGenerator &move_gen) : m_move_exec(move_exec), m_move_gen(move_gen) {}

MiniMax::~MiniMax() {}

int MiniMax::minimaxOrParanoidWithPruning(Board &board, int alpha, int beta, int8_t depth, uint8_t player_num, bool sorting, Timer &timer)
{

    try
    {
        static int call_count = 0;
        call_count++;
        if (call_count % 1 == 0)
        {
            LOG_INFO("trying move: " + std::to_string(board.get_coord()) + " by player " + std::to_string(player_num + 1) + " depth: " + std::to_string(depth) + " time left: " + std::to_string(timer.return_rest_time()) + " elapsed time " + std::to_string(timer.get_elapsed_time()));
        }
        if (timer.return_rest_time() < timer.exception_time)
        {
            throw TimeLimitExceededException("minimax 0");
        }
        if (depth == 0)
        {
            return get_evaluation(board, player_num, m_move_gen, timer);
        }
        uint8_t next_player = (player_num + 1) % m_move_exec.get_num_of_players();
        while (board.disqualified[next_player])
        {
            next_player = (next_player + 1) % m_move_exec.get_num_of_players();
        }
        if (timer.return_rest_time() < timer.exception_time)
        {
            throw TimeLimitExceededException("minimax 1");
        }
        m_move_gen.calculate_valid_moves(board, player_num, timer);

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
                throw TimeLimitExceededException("minimax 2");
            }
            m_move_gen.calculate_valid_moves(board, player_num, timer);
        }
        if (timer.return_rest_time() < timer.exception_time)
        {
            throw TimeLimitExceededException(("minimax 3"));
        }

        if (sorting)
        {
            std::vector<std::bitset<2501>> new_valid_moves = sort_valid_moves(board, player_num, timer, player_num == m_move_exec.get_player_num());
        }

        std::vector<Board> boards = generate_boards(board, player_num, timer);

        int best_eval;
        if (player_num == m_move_exec.get_player_num())
            best_eval = -INT32_MAX;
        else
            best_eval = INT32_MAX;

        for (auto &b : boards)
        {
            if (timer.return_rest_time() < timer.exception_time)
            {
                throw TimeLimitExceededException(("minimax 4"));
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
std::vector<std::bitset<2501>> MiniMax::sort_valid_moves(Board &board, uint8_t player_num, Timer &timer, bool maximizer)
{
    try
    {
        std::vector<std::bitset<2501>> sorted_valid_moves;
        std::vector<std::pair<int, uint16_t>> evals;
        std::vector<std::bitset<2501>> valid_moves = board.valid_moves;

        std::vector<Board> boards = generate_boards(board, player_num, timer);

        for (Board &b : boards)
        {
            if (timer.return_rest_time() < timer.exception_time)
            {
                throw TimeLimitExceededException(("sorting"));
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
            throw TimeLimitExceededException(("sorting 2"));
        }
        sorted_valid_moves.clear();
        for (const auto &eval : evals)
        {
            if (timer.return_rest_time() < timer.exception_time)
            {
                throw TimeLimitExceededException(("sorting 3"));
            }
            sorted_valid_moves.push_back(valid_moves[eval.second]);
        }
        return sorted_valid_moves;
    }
    catch (TimeLimitExceededException &e)
    {
        throw;
    }
}

std::vector<Board> MiniMax::generate_boards(Board &board, uint8_t player_num, Timer &timer)
{
    std::vector<Board> boards;
    LOG_INFO("valid moves: " + std::to_string(board.valid_moves[player_num].count()));
    for (uint16_t i = 1; i < m_move_exec.get_num_of_fields(); i++)
    {
        if (i > 1000)
            LOG_INFO("time left: " + std::to_string(timer.return_rest_time()) + " i = " + std::to_string(i) + " coord: " + std::to_string(board.get_coord()));

        if (timer.return_rest_time() < timer.exception_time)
        {
            throw TimeLimitExceededException("generate Boards 1");
        }
        if (board.valid_moves[player_num].test(i))
        {
            if (board.board_sets[3].test(i))
            {
                for (uint8_t j = 0; j < m_move_exec.get_num_of_players(); j++)
                {
                    LOG_INFO("time left: " + std::to_string(timer.return_rest_time()) + " j = " + std::to_string(j));
                    if (timer.return_rest_time() < timer.exception_time)
                    {
                        throw TimeLimitExceededException("generate Boards 2");
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
            for (auto &b : boards)
            {
                int eval = minimaxOrParanoidWithPruning(b, -INT32_MAX, INT32_MAX, search_depth, m_move_exec.get_player_num(), sorting, timer);
                if (eval > best_eval)
                {
                    best_eval = eval;
                    best_board = b;
                }
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
