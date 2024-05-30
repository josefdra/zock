#include "algorithms.hpp"
#include "map.hpp"
#include "timer.hpp"
#include "move_board.hpp"
#include "bomb_board.hpp"
#include "evaluator.hpp"

MiniMax::MiniMax(MoveExecuter &move_exec, MoveGenerator &move_gen) : m_move_exec(move_exec), m_move_gen(move_gen) {}

MiniMax::~MiniMax() {}

int MiniMax::minimaxOrParanoidWithPruning(MoveBoard &move_board, int alpha, int beta, int8_t depth, uint8_t player_num, bool sorting, Timer &timer)
{
    if (depth == 0)
    {
        return get_evaluation(move_board, player_num, m_move_gen, timer);
    }
    uint8_t next_player = (player_num + 1) % m_move_exec.get_num_of_players();
    while (move_board.is_disqualified(next_player))
    {
        next_player = (next_player + 1) % m_move_exec.get_num_of_players();
    }
    m_move_gen.calculate_valid_moves(move_board, player_num, timer);
    uint8_t prev_player = player_num;
    while (move_board.get_valid_moves(player_num).count() == 0)
    {
        player_num = (player_num + 1) % m_move_exec.get_num_of_players();
        while (move_board.is_disqualified(next_player))
        {
            next_player = (next_player + 1) % m_move_exec.get_num_of_players();
        }
        if (player_num == prev_player)
            return get_evaluation(move_board, player_num, m_move_gen, timer);
        next_player = (next_player + 1) % m_move_exec.get_num_of_players();
        m_move_gen.calculate_valid_moves(move_board, player_num, timer);
    }
    std::vector<MoveBoard> move_boards = generate_boards(move_board, player_num, timer);
    int best_eval;
    if (player_num == m_move_exec.get_player_num())
        best_eval = -INT32_MAX;
    else
        best_eval = INT32_MAX;
    for (auto &b : move_boards)
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

std::vector<MoveBoard> MiniMax::generate_boards(MoveBoard &move_board, uint8_t player_num, Timer &timer)
{
    std::vector<MoveBoard> move_boards;
    for (uint16_t i = 1; i < m_move_exec.get_num_of_fields(); i++)
    {
        if (move_board.get_valid_moves(player_num).test(i))
        {
            if (move_board.get_board_set(3).test(i))
            {
                for (uint8_t j = 0; j < m_move_exec.get_num_of_players(); j++)
                {
                    move_boards.push_back(m_move_exec.exec_move(player_num, MoveBoard(move_board, i, j), timer));
                }
            }
            else if (move_board.get_board_set(4).test(i))
            {
                move_boards.push_back(m_move_exec.exec_move(player_num, MoveBoard(move_board, i, 21), timer));
            }
            else
            {
                move_boards.push_back(m_move_exec.exec_move(player_num, MoveBoard(move_board, i, 0), timer));
            }
        }
    }
    return move_boards;
}

void MiniMax::init_best_board(MoveBoard &move_board)
{
    for (uint16_t c = 0; c < move_board.get_num_of_fields(); c++)
    {
        if (move_board.get_valid_moves(move_board.get_player_num()).test(c))
        {
            move_board.set_coord(c);
            if (move_board.get_board_set(3).test(c))
            {
                move_board.set_spec(m_move_exec.get_player_num());
            }
            else if (move_board.get_board_set(4).test(c))
            {
                move_board.set_spec(21);
            }
            else
            {
                move_board.set_spec(0);
            }
            break;
        }
    }
}

MoveBoard MiniMax::get_best_coord(MoveBoard &move_board, Timer &timer, uint8_t depth, bool sorting)
{
    int best_eval = -INT32_MAX;
    MoveBoard best_board(move_board);
    init_best_board(best_board);
    try
    {
        std::vector<MoveBoard> move_boards = generate_boards(move_board, m_move_exec.get_player_num(), timer);
        for (auto &b : move_boards)
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
