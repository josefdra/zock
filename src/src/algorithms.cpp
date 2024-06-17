#include "algorithms.hpp"
#include "map.hpp"
#include "timer.hpp"
#include "board.hpp"
#include "evaluator.hpp"
#include "logging.hpp"

#define MAX_SEARCH_DEPTH 15

Algorithms::Algorithms(MoveExecuter &move_exec, MoveGenerator &move_gen) : killer_moves(std::vector<std::vector<uint16_t>>(MAX_SEARCH_DEPTH, std::vector<uint16_t>(move_exec.get_num_of_fields()))), m_move_exec(move_exec), m_move_gen(move_gen)
{
}

Algorithms::~Algorithms() {}

uint8_t Algorithms::get_next_player(uint8_t player_num, Board &board, Timer &timer)
{
    uint8_t next_player = player_num;
    do
    {
        next_player = (next_player + 1) % m_move_exec.get_num_of_players();
        if (!board.disqualified[next_player])
            m_move_gen.calculate_valid_moves(board, next_player, timer);

        if (next_player == player_num)
            return player_num;

    } while (board.disqualified[next_player] || board.get_total_moves(next_player).count() == 0);
    return next_player;
}

int Algorithms::set_up_best_eval(uint8_t &brs_m, uint8_t player_num)
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

int Algorithms::do_move(Board &board, move &m, int alpha, int beta, uint8_t brs_m, uint8_t depth, Timer &timer, Board &prev_board, uint8_t next_player, bool sorting, uint8_t index)
{
    board.set_coord(std::get<1>(m));
    board.set_spec(std::get<2>(m));
    m_move_exec.exec_move(next_player, board);
    int eval = brs(board, alpha, beta, brs_m, depth - 1, next_player, timer, sorting, index);
    board = prev_board;
    return eval;
}

void Algorithms::get_eval(Board &board, moves &moves, int alpha, int beta, uint8_t brs_m, uint8_t depth, Timer &timer, Board &prev_board, uint8_t next_player, int &best_eval, bool sorting, uint8_t index)
{
    for (auto &m : moves)
    {
        if (timer.return_rest_time() < timer.exception_time)
            throw TimeLimitExceededException(("Timeout in minimax after generating boards and iterating over them."));

        int eval = do_move(board, m, alpha, beta, brs_m + 1, depth, timer, prev_board, next_player, sorting, index);
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

move Algorithms::get_first_move(moves_vector &moves)
{
    for (auto &move : moves)
        for (auto &m : move)
            return m;

    return move();
}

int Algorithms::brs(Board &board, int alpha, int beta, uint8_t brs_m, uint8_t depth, uint8_t player_num, Timer &timer, bool sorting, uint8_t index)
{
    Board prev_board = board;
    try
    {
        static int call_count = 0;
        call_count++;
#ifdef DEBUG
        LOG_INFO("trying move: " + std::to_string(board.get_coord()) + " by player " + std::to_string(player_num + 1) + " depth: " + std::to_string(depth) + " time left: " + std::to_string(timer.return_rest_time()) + " elapsed time " + std::to_string(timer.get_elapsed_time()));
#endif
        uint8_t next_player = get_next_player(player_num, board, timer);
        if (depth == 0 || next_player == player_num)
            return get_evaluation(board, player_num, m_move_gen, timer);

        moves_vector moves;
        moves.reserve(3000);
        set_up_moves(board, next_player, moves);
        if (sorting)
            sort_valid_moves(board, next_player, moves, timer, depth);
        int best_eval = set_up_best_eval(brs_m, player_num);
        for (uint8_t i = 0; i < board.get_num_of_communities(); i++)
        {
            if ((board.communities[i] & board.player_sets[player_num]).count() != 0)
                if ((board.communities[i] & board.player_sets[next_player]).count() != 0)
                {
                    if (brs_m < 2)
                        get_eval(board, moves[i], alpha, beta, brs_m, depth, timer, prev_board, next_player, best_eval, sorting, i);

                    if (brs_m > 0)
                    {
                        move first_move = get_first_move(moves);
                        do_move(board, first_move, alpha, beta, brs_m, depth, timer, prev_board, next_player, sorting, i);
                    }
                }
                else
                {
                    move first_move = get_first_move(moves);
                    do_move(board, first_move, alpha, beta, brs_m, depth, timer, prev_board, next_player, sorting, i);
                }
        }

        return best_eval;
    }
    catch (const TimeLimitExceededException &)
    {
        throw;
    }
}

void Algorithms::set_up_killer(moves_vector &moves, uint8_t depth, uint8_t index)
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
        while (std::get<1>(moves[index][i]) != smallest_killer_coord && i < moves.size())
            i++;

        while (std::get<1>(moves[index][i]) == smallest_killer_coord && i < moves.size())
        {
            std::get<0>(moves[index][i]) = std::get<1>(temp_killer[temp_killer.size() - 1]) * -10000;
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
void Algorithms::sort_valid_moves(Board &board, uint8_t player_num, moves_vector &moves, Timer &timer, uint8_t depth)
{
    Board prev_board = board;
    try
    {
        for (uint8_t index = 0; index < moves.size(); index++)
        {
            set_up_killer(moves, depth, index);
            if (player_num == m_move_exec.get_player_num())
                for (auto &m : moves[index])
                    if (std::get<0>(m) != 0)
                        std::get<0>(m) = std::get<0>(m) * -1;

            for (auto &m : moves[index])
            {
                if (timer.return_rest_time() < timer.exception_time)
                    throw TimeLimitExceededException("Timeout in sort_valid_moves before sorting");

                board.set_coord(std::get<1>(m));
                board.set_spec(std::get<2>(m));
                std::get<0>(m) += m_move_exec.get_bits_to_update(player_num, board).count();
            }
            if (player_num == m_move_exec.get_player_num())
                std::sort(moves[index].begin(), moves[index].end(), [](const std::tuple<int, uint16_t, uint8_t> &a, const std::tuple<int, uint16_t, uint8_t> &b)
                          { return std::get<0>(a) > std::get<0>(b); });

            else
                std::sort(moves[index].begin(), moves[index].end(), [](const std::tuple<int, uint16_t, uint8_t> &a, const std::tuple<int, uint16_t, uint8_t> &b)
                          { return std::get<0>(a) < std::get<0>(b); });

            if (timer.return_rest_time() < timer.exception_time)
                throw TimeLimitExceededException(("Timeout in sort_valid_moves after sorting"));
        }
    }
    catch (const TimeLimitExceededException &)
    {
        throw;
    }
}

void Algorithms::set_up_moves(Board &board, uint8_t player_num, moves_vector &moves)
{
    for (uint8_t index = 0; index < moves[player_num].size(); index++)
        for (uint16_t i = 1; i < m_move_exec.get_num_of_fields(); i++)
            if (board.valid_moves[player_num][index].test(i))
            {
                if (board.board_sets[C].test(i))
                    for (uint8_t j = 0; j < m_move_exec.get_num_of_players(); j++)
                        moves[index].push_back(std::make_tuple(0, i, j));

                else if (board.board_sets[B].test(i))
                    moves[index].push_back(std::make_tuple(0, i, OVERWRITE_SPEC));

                else
                    moves[index].push_back(std::make_tuple(0, i, 0));
            }
}

void Algorithms::init_best_board(Board &board)
{
    std::bitset<MAX_NUM_OF_FIELDS> total_moves = board.get_total_moves(m_move_exec.get_player_num());

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

Board Algorithms::get_best_coord(Board &board, Timer &timer, bool sorting)
{
    int best_eval = -INT32_MAX;
    Board best_board(board);
    init_best_board(best_board);
    uint8_t player_num = m_move_exec.get_player_num();
    moves_vector moves(board.get_num_of_communities());
    moves.reserve(3000);
    Board prev_board = board;
    int alpha = -INT32_MAX;
    int beta = INT32_MAX;
    try
    {
        set_up_moves(board, player_num, moves);
        if (sorting)
            sort_valid_moves(board, player_num, moves, timer, 0);
        for (uint8_t index = 0; index < moves.size(); index++)
            for (uint8_t search_depth = 0; search_depth < MAX_SEARCH_DEPTH; search_depth++)
                for (auto &m : moves[index])
                {
                    if (timer.return_rest_time() < timer.exception_time)
                        throw TimeLimitExceededException(("Timeout in get_best_coord"));

                    board.set_coord(std::get<1>(m));
                    board.set_spec(std::get<2>(m));
                    m_move_exec.exec_move(player_num, board);
                    int eval = brs(board, alpha, beta, 0, search_depth, m_move_exec.get_player_num(), timer, sorting, index);
                    if (eval > best_eval)
                    {
                        best_eval = eval;
                        best_board = board;
                    }
                    board = prev_board;
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
