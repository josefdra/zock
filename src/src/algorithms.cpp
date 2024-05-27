#include "algorithms.hpp"
#include "map.hpp"
#include "timer.hpp"
#include "board.hpp"
#include "evaluator.hpp"

MiniMax::MiniMax(MoveExecuter &move_exec, MoveGenerator &move_gen) : m_move_exec(move_exec), m_move_gen(move_gen) {}

MiniMax::~MiniMax() {}

int MiniMax::minimaxOrParanoidWithPruning(Board &board, int alpha, int beta, int8_t depth, uint8_t player_num, bool sorting, Timer &timer)
{
    if (depth == 0)
    {
        return get_evaluation(board, player_num, m_move_gen, timer);
    }
    if (timer.return_rest_time() < timer.exception_time)
    {
        throw TimeLimitExceededException();
    }
    uint8_t next_player = (player_num + 1) % m_move_exec.get_num_of_players();
    m_move_gen.calculate_valid_moves(board, player_num, timer);
    uint8_t prev_player = player_num;
    while (board.valid_moves[player_num].count() == 0)
    {
        player_num = (player_num + 1) % m_move_exec.get_num_of_players();
        if (player_num == prev_player)
            return get_evaluation(board, player_num, m_move_gen, timer);
        next_player = (next_player + 1) % m_move_exec.get_num_of_players();
        m_move_gen.calculate_valid_moves(board, player_num, timer);
    }
    std::vector<Board> boards = generate_boards(board, player_num, timer);
    int best_eval;
    if (player_num == m_move_exec.get_player_num())
        best_eval = -INT32_MAX;
    else
        best_eval = INT32_MAX;
    for (auto &b : boards)
    {
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

std::vector<Board> MiniMax::generate_boards(Board &board, uint8_t player_num, Timer &timer)
{
    std::vector<Board> boards;
    for (uint16_t i = 1; i < m_move_exec.get_num_of_fields(); i++)
    {
        if (board.valid_moves[player_num].test(i))
        {
            if (timer.return_rest_time() < timer.exception_time)
            {
                throw TimeLimitExceededException();
            }
            if (board.board_sets[3].test(i))
            {
                for (uint8_t j = 0; j < m_move_exec.get_num_of_players(); j++)
                {
                    boards.push_back(m_move_exec.exec_move(player_num, Board(board, i, j), timer));
                }
            }
            else if (board.board_sets[4].test(i))
            {
                boards.push_back(m_move_exec.exec_move(player_num, Board(board, i, 20), timer));
                boards.push_back(m_move_exec.exec_move(player_num, Board(board, i, 21), timer));
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
    for (uint16_t c = 0; c < board.get_num_of_fields(); c++)
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

Board MiniMax::get_best_coord(Board &board, Timer &timer, uint8_t depth, bool sorting)
{
    int best_eval = -INT32_MAX;
    Board best_board(board);
    init_best_board(best_board);
    try
    {
        std::vector<Board> boards = generate_boards(board, m_move_exec.get_player_num(), timer);
        for (auto &b : boards)
        {
            int eval = minimaxOrParanoidWithPruning(b, -INT32_MAX, INT32_MAX, depth, m_move_exec.get_player_num(), sorting, timer);
            if (eval > best_eval)
            {
                best_eval = eval;
                best_board = b;
            }
        }
    }
    catch (TimeLimitExceededException &e)
    {
        std::cout << timer.return_rest_time() << std::endl;
        std::cout << "time limit exceeded" << std::endl;
    }
    std::cout << "best eval" << std::endl;
    std::cout << best_eval << std::endl;
    return best_board;
}
