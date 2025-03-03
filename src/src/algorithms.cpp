#include "algorithms.hpp"
#include "initializer.hpp"
#include "timer.hpp"
#include "board.hpp"
#include "evaluator.hpp"
#include "logging.hpp"
#include "statistics.hpp"

/**
 * @brief This part contains all relevant information about used algorithms through this project and it's helper functions
 *
 *
 *
 *
 */

Algorithms::Algorithms() {}

Algorithms::Algorithms(MoveExecuter &move_exec, MoveGenerator &move_gen) : killer_moves(std::vector<std::vector<uint16_t>>(MAX_SEARCH_DEPTH, std::vector<uint16_t>(move_exec.get_num_of_fields()))), move_exec(move_exec), m_move_gen(move_gen)
{
}

Algorithms::~Algorithms() {}

/// @brief calculates next player which can do moves
/// @param player_num current player
/// @param board current board layout
/// @param timer current timer object
/// @param index index of community
/// @return player after current player
uint8_t Algorithms::get_next_player(uint8_t player_num, Board &board, Timer &timer, uint8_t &index)
{
    uint8_t next_player = player_num;
    do
    {
        next_player = (next_player + 1) % move_exec.get_num_of_players();
        board.valid_moves[next_player].clear();
        board.valid_moves[next_player].resize(board.get_num_of_communities(), std::bitset<MAX_NUM_OF_FIELDS>(0));

        if (!board.disqualified[next_player])
        {
            if ((board.communities[index] & board.player_sets[next_player]).count() != 0)
            {
                m_move_gen.calculate_valid_no_ow_moves(board, next_player, index);
                if (board.has_overwrite_stones(player_num) && board.valid_moves[next_player][index].count() == 0)
                {
                    m_move_gen.calculate_valid_ow_moves(board, next_player, timer, index);
                    board.valid_moves[next_player][index] |= (board.board_sets[X] & board.communities[index]);
                }
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

/// @brief initializes evaluation value for minimax
/// @param board current board layout
/// @param player_num current player
/// @return init value according to player
int Algorithms::set_up_best_eval_minimax(Board &board, uint8_t player_num)
{
    int best_eval;
    if (player_num == board.get_our_player())
    {
        best_eval = -INT32_MAX;
    }
    else
        best_eval = INT32_MAX;
    return best_eval;
}

/// @brief initializes evaluation value for brs
/// @param board current board layout
/// @param brs_m init value of m
/// @param player_num current player
/// @return init value according to player
int Algorithms::set_up_best_eval_brs(Board &board, uint8_t &brs_m, uint8_t player_num)
{
    int best_eval;
    if (player_num == board.get_our_player())
    {
        brs_m = 0;
        best_eval = -INT32_MAX;
    }
    else
        best_eval = INT32_MAX;
    return best_eval;
}

/// @brief calls minimax function to determine value of current move and executes it to actually calculate the value of the board if move was done
/// @param board current board layout
/// @param m current move to evaluate
/// @param alpha alpha value for alpha beta pruning
/// @param beta beta value for alpha beta pruning
/// @param depth current depth
/// @param timer timer object to prevent a timeout
/// @param prev_board previous board layout to save the actual board layout to prevent interference with actual move execution sent by server
/// @param next_player player after current player
/// @param sorting tells if move sorting is enabled | true - enabled; false - disabled
/// @param index index of community
/// @return evaluation of move through minimax
int Algorithms::do_move_minimax(Board &board, move &m, int alpha, int beta, uint8_t depth, Timer &timer, Board &prev_board, uint8_t next_player, bool sorting, uint8_t &index)
{
    board.set_coord(std::get<1>(m));
    board.set_spec(std::get<2>(m));
    uint8_t prev_index = index;
    Timer move_execution_timer;
    move_exec.exec_move(next_player, board, index);
    move_execution_time += move_execution_timer.get_elapsed_time();
    int eval = minimax(board, alpha, beta, depth - 1, next_player, timer, sorting, index);
    board = prev_board;
    index = prev_index;
    return eval;
}

/// @brief calls brs function to determine value of current move and executes it to actually calculate the value of the board if move was done
/// @param board current board layout
/// @param m current move to evaluate
/// @param alpha alpha value for alpha beta pruning
/// @param beta beta value for alpha beta pruning
/// @param brs_m current m value for brs
/// @param depth current depth
/// @param timer current timer object
/// @param prev_board previous board layout to save the actual board layout to prevent interference with actual move execution sent by server
/// @param next_player player after current player
/// @param sorting tells if move sorting is enabled | true - enabled; false - disabled
/// @param index index of community
/// @return evaluation of move through brs
int Algorithms::do_move_brs(Board &board, move &m, int alpha, int beta, uint8_t brs_m, uint8_t depth, Timer &timer, Board &prev_board, uint8_t next_player, bool sorting, uint8_t &index)
{
    board.set_coord(std::get<1>(m));
    board.set_spec(std::get<2>(m));
    uint8_t prev_index = index;
    Timer move_execution_timer;
    move_exec.exec_move(next_player, board, index);
    move_execution_time += move_execution_timer.get_elapsed_time();
    int eval = brs(board, alpha, beta, brs_m, depth - 1, next_player, timer, sorting, index);
    board = prev_board;
    index = prev_index;
    return eval;
}

/// @brief evaluates moves and switches to brs evaluation if there are more than 2 players in a community and sets values for alpha beta pruning
/// @param board current board
/// @param moves contains all valid moves
/// @param alpha alpha value for alpha beta pruning
/// @param beta beta value for alpha beta pruning
/// @param depth current depth
/// @param timer timer object to check if time is up
/// @param prev_board previous board state
/// @param next_player player after current player
/// @param best_eval best eval to set
/// @param sorting sorting bool to give to other functions which have to check because of move sorting
/// @param index index of community
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
            if (next_player == board.get_our_player())
                brs_m = 0;
            else
                brs_m = 1;
            eval = do_move_brs(board, m, alpha, beta, brs_m, depth, timer, prev_board, next_player, sorting, index);
        }
        else
            eval = do_move_minimax(board, m, alpha, beta, depth, timer, prev_board, next_player, sorting, index);
        if (next_player == board.get_our_player())
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

/// @brief evaluates moves and switches to brs evaluation if there are more than 2 players in a community and sets values for alpha beta pruning
/// @param board current board
/// @param moves contains all valid moves
/// @param alpha alpha value for alpha beta pruning
/// @param beta beta value for alpha beta pruning
/// @param brs_m current brs_m state
/// @param depth current depth
/// @param timer timer object to check if time is up
/// @param prev_board previous board state
/// @param next_player player after current player
/// @param best_eval best eval to set
/// @param sorting bool to give to function which uses it to determine if moves must be sorted
/// @param index index of community
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
        if (next_player == board.get_our_player())
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

/// @brief gets first valid move of a player
/// @param moves contains all valid moves
/// @return first valid move
move Algorithms::get_first_move(moves &moves)
{
    for (auto &m : moves)
        return m;

    return move();
}

/// @brief sets everything up to be able to do a evaluation of minimax by checking next player, getting valid moves and sorting them if necessary
/// @param board current board layout
/// @param alpha alpha value for alpha beta pruning
/// @param beta beta value for alpha beta pruning
/// @param depth current depth
/// @param player_num current player
/// @param timer current timer object
/// @param sorting bool if sorting is enabled or not | true - enabled; false - disabled
/// @param index index of community
/// @return returns best value of board layout
int Algorithms::minimax(Board &board, int alpha, int beta, uint8_t depth, uint8_t player_num, Timer &timer, bool sorting, uint8_t &index)
{
    Board prev_board = board;
    try
    {
        static int call_count = 0;
        call_count++;
        nodes_calculated++;

        Timer get_next_player_timer;
        uint8_t next_player = get_next_player(player_num, board, timer, index);
        average_next_player_calculation_time += get_next_player_timer.get_elapsed_time();
        if (depth == 0 || board.is_final_state())
            return get_evaluation(board, player_num, timer, m_move_gen, index);

        if (!board.corners_and_walls.test(board.get_coord()))
            board.static_evaluation[board.get_coord()] = 0;

        Timer set_up_moves_timer;
        moves moves;
        moves.reserve(MEMORY_SIZE_WITH_BUFFER);
        set_up_moves(board, next_player, moves, index);
        set_up_moves_time += set_up_moves_timer.get_elapsed_time();
        total_nodes += moves.size();
        total_valid_moves++;
        calculate_average_branching_factor();

        Timer sorting_timer;
        if (sorting)
            sort_valid_moves(board, next_player, moves, timer, depth);
        sorting_time += sorting_timer.get_elapsed_time();
        int best_eval = set_up_best_eval_minimax(board, player_num);
        get_eval_minimax(board, moves, alpha, beta, depth, timer, prev_board, next_player, best_eval, sorting, index);
        return best_eval;
    }
    catch (const TimeLimitExceededException &)
    {
        throw;
    }
}
/// @brief setup function for brs by getting next player, valid moves and sorting if necessary. Also calls needed function in dependency of brs_m
/// @param board current board layout
/// @param alpha alpha value for alpha beta pruning
/// @param beta beta value for alpha beta pruning
/// @param brs_m current brs_m value
/// @param depth current depth
/// @param player_num current player
/// @param timer timer object
/// @param sorting bool if sorting is enabled or not | true - enabled; false - disabled
/// @param index index of community
/// @return returns best value of board layout
int Algorithms::brs(Board &board, int alpha, int beta, uint8_t brs_m, uint8_t depth, uint8_t player_num, Timer &timer, bool sorting, uint8_t &index)
{
    Board prev_board = board;
    try
    {
        static int call_count = 0;
        call_count++;

        Timer get_next_player_timer;
        uint8_t next_player = get_next_player(player_num, board, timer, index);
        average_next_player_calculation_time += get_next_player_timer.get_elapsed_time();
        if (depth == 0 || board.is_final_state())
            return get_evaluation(board, player_num, timer, m_move_gen, index);

        if (!board.corners_and_walls.test(board.get_coord()))
            board.static_evaluation[board.get_coord()] = 0;

        move_exec.calculate_before_protected_fields(board, player_num);

        nodes_calculated++;
        Timer set_up_moves_timer;
        moves moves;
        moves.reserve(MEMORY_SIZE_WITH_BUFFER);
        set_up_moves(board, next_player, moves, index);
        set_up_moves_time += set_up_moves_timer.get_elapsed_time();
        total_nodes += moves.size();
        total_valid_moves++;
        calculate_average_branching_factor();

        Timer sorting_timer;
        if (sorting)
            sort_valid_moves(board, next_player, moves, timer, depth);
        sorting_time += sorting_timer.get_elapsed_time();
        int best_eval = set_up_best_eval_brs(board, brs_m, player_num);
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
/// @brief checks for possible bad moves and puts them first into order to be able to prune it's paths early on
/// @param moves field of valid moves
/// @param depth current depth
void Algorithms::set_up_killer(moves &moves, uint8_t depth)
{
    std::vector<std::tuple<uint16_t, uint8_t>> temp_killer;
    for (uint16_t c = 1; c < move_exec.get_num_of_fields(); c++)
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

/// @brief sorts the valid moves of a player and needs the calculated valid moves in the board
/// @param board current board with calculated valid moves
/// @param player_num current player
/// @param timer timer object
/// @param maximizer determines whether it gets sorted from small to high values or vice versa | if it's our player - high2small; else small2high
/// @return sorted valid moves as vector
void Algorithms::sort_valid_moves(Board &board, uint8_t player_num, moves &moves, Timer &timer, uint8_t depth)
{
    Board prev_board = board;
    try
    {
        set_up_killer(moves, depth);
        if (player_num == board.get_our_player())
            for (auto &m : moves)
                if (std::get<0>(m) != 0)
                    std::get<0>(m) = std::get<0>(m) * -1;

        for (auto &m : moves)
        {
            if (timer.return_rest_time() < timer.exception_time)
                throw TimeLimitExceededException("Timeout in sort_valid_moves before sorting");

            board.set_coord(std::get<1>(m));
            board.set_spec(std::get<2>(m));
            std::get<0>(m) += move_exec.get_bits_to_update(player_num, board).count();
        }
        if (player_num == board.get_our_player())
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
/// @brief checks if a valid move is a special stone and sets our specifications for it, else it just gets added as normal move to moves
/// @param board current board state
/// @param player_num current player number
/// @param moves contains all valid moves
/// @param index index of community
void Algorithms::set_up_moves(Board &board, uint8_t player_num, moves &moves, uint8_t index)
{
    for (uint16_t c = 1; c < move_exec.get_num_of_fields(); c++)
        if (board.valid_moves[player_num][index].test(c))
        {
            if (board.board_sets[C].test(c))
                for (uint8_t j = 0; j < move_exec.get_num_of_players(); j++)
                    moves.push_back(std::make_tuple(ZERO_EVALUATION, c, j));

            else if (board.board_sets[B].test(c))
                moves.push_back(std::make_tuple(ZERO_EVALUATION, c, OVERWRITE_SPEC));

            else
                moves.push_back(std::make_tuple(ZERO_EVALUATION, c, 0));
        }
}
/// @brief prints all valid moves we've got and sets current coords
/// @param board
void Algorithms::init_best_board(Board &board)
{
    std::bitset<MAX_NUM_OF_FIELDS> total_moves = board.get_total_moves(board.get_our_player());
    LOG_INFO("Total moves: " + std::to_string(total_moves.count()));
    for (uint16_t c = 1; c < board.get_num_of_fields(); c++)
        if (total_moves.test(c))
        {
            board.set_coord(c);
            if (board.board_sets[C].test(c))
                board.set_spec(board.get_our_player());

            else if (board.board_sets[B].test(c))
                board.set_spec(OVERWRITE_SPEC);

            else
                board.set_spec(0);
            break;
        }
}
/// @brief sorts good moves and it's communities to front that it's paths get calculated early on to check if they remain good in other situations
/// @param board current board layout
/// @param best_move_in_community_index
/// @param best_community_index
/// @param valid_moves contains all valid moves
void Algorithms::sort_best_moves_and_communities_to_front(Board &board, std::vector<uint16_t> &best_move_in_community_index, uint8_t &best_community_index, moves_vector &valid_moves)
{
    if (board.get_num_of_communities() > 1)
    {
        std::bitset<MAX_NUM_OF_FIELDS> temp_community = board.communities[best_community_index];
        std::bitset<MAX_NUM_OF_FIELDS> temp_frame = board.frames[best_community_index];
        uint16_t temp_move_index = best_move_in_community_index[best_community_index];
        moves temp_moves = valid_moves[best_community_index];
        std::tuple<uint16_t, uint16_t> temp_start_end_community = board.start_end_communities[best_community_index];
        std::tuple<uint16_t, uint16_t> temp_start_end_frame = board.start_end_frames[best_community_index];
        for (uint8_t i = best_community_index; i > 0; i--)
        {
            board.communities[i] = board.communities[i - 1];
            board.frames[i] = board.frames[i - 1];
            best_move_in_community_index[i] = best_move_in_community_index[i - 1];
            valid_moves[i] = valid_moves[i - 1];
            board.start_end_communities[i] = board.start_end_communities[i - 1];
            board.start_end_frames[i] = board.start_end_frames[i - 1];
        }
        board.communities[0] = temp_community;
        board.frames[0] = temp_frame;
        best_move_in_community_index[0] = temp_move_index;
        valid_moves[0] = temp_moves;
        board.start_end_communities[0] = temp_start_end_community;
        board.start_end_frames[0] = temp_start_end_frame;
    }

    for (uint8_t i = 0; i < board.get_num_of_communities(); i++)
    {
        if (valid_moves[i].size() == 0)
            continue;
        std::tuple<int, uint16_t, uint8_t> temp_move = valid_moves[i][best_move_in_community_index[i]];
        for (uint8_t j = best_move_in_community_index[i]; j > 0; j--)
            valid_moves[i][j] = valid_moves[i][j - 1];

        valid_moves[i][0] = temp_move;
    }
}
/// @brief main function of algorithms.cpp which controls all of the communication between all algorithms
/// @param board current board layout
/// @param timer timer object to check if time is up
/// @param sorting bool which tells to sort moves or not
/// @return best board that got calculated
Board Algorithms::get_best_coord(Board &board, Timer &timer, bool sorting)
{
    int best_eval = INT32_MIN;
    Board best_board(board);
    init_best_board(best_board);
    uint8_t player_num = board.get_our_player();
    moves_vector moves(board.get_num_of_communities());
    moves.reserve(MEMORY_SIZE_WITH_BUFFER);
    Board prev_board = board;
    int alpha = INT32_MIN;
    int beta = INT32_MAX;
    uint8_t search_depth;
    std::vector<int> best_community_eval(board.get_num_of_communities(), INT32_MIN);
    std::vector<uint16_t> best_move_in_community_index(board.get_num_of_communities(), 0);
    uint8_t best_community_index = 0;
    static uint8_t max_search_depth = 1;
    calculate_thresholds(board.scaling_factor);

    if (max_search_depth != MAX_SEARCH_DEPTH)
        adapt_depth_to_map_progress(max_search_depth, board);

    try
    {
        for (uint8_t community_index = 0; community_index < board.get_num_of_communities(); community_index++)
        {
            if (board.valid_moves[player_num][community_index].count() == 0)
                continue;
            Timer set_up_moves_timer;
            set_up_moves(board, player_num, moves[community_index], community_index);
            set_up_moves_time += set_up_moves_timer.get_elapsed_time();
            Timer sorting_timer;
            if (sorting)
                sort_valid_moves(board, player_num, moves[community_index], timer, 0);
            sorting_time += sorting_timer.get_elapsed_time();
        }
        for (search_depth = 0; search_depth < max_search_depth; search_depth++)
        {
            Timer measure_depth_search(timer.return_rest_time());
            for (uint8_t community_index = 0; community_index < board.get_num_of_communities(); community_index++)
            {
                if (moves[community_index].size() == 0)
                    continue;

                if (search_depth == 0)
                    total_nodes = total_valid_moves = moves[community_index].size();

                for (uint16_t move_index = 0; move_index < moves[community_index].size(); move_index++)
                {
                    if (timer.return_rest_time() < timer.exception_time)
                        throw TimeLimitExceededException(("Timeout in get_best_coord"));

                    nodes_calculated++;
                    board.set_coord(std::get<1>(moves[community_index][move_index]));
                    board.set_spec(std::get<2>(moves[community_index][move_index]));
                    uint8_t prev_index = community_index;
                    Timer move_execution_timer;
                    move_exec.exec_move(player_num, board, community_index);
                    move_execution_time += move_execution_timer.get_elapsed_time();
                    int eval;
                    if (board.num_of_players_in_community[community_index] > 2)
                        eval = brs(board, alpha, beta, 0, search_depth, player_num, timer, sorting, community_index);
                    else
                        eval = minimax(board, alpha, beta, search_depth, player_num, timer, sorting, community_index);

                    if (eval > best_eval)
                    {
                        best_eval = eval;
                        best_board = board;
                        best_community_index = prev_index;
                    }
                    if (eval > best_community_eval[prev_index])
                    {
                        best_community_eval[prev_index] = eval;
                        best_move_in_community_index[prev_index] = move_index;
                    }
                    board = prev_board;
                    community_index = prev_index;
                }
            }
            sort_best_moves_and_communities_to_front(board, best_move_in_community_index, best_community_index, moves);
            double estimated_runtime = estimate_runtime_next_depth(search_depth, measure_depth_search);
            if (estimated_runtime > timer.return_rest_time())
            {
                LOG_INFO("Skipping next depth " + std::to_string(search_depth + 1) + " because estimated time exceeds time left.");
                break;
            }
            else if (estimated_runtime > _30SECONDS)
            {
                LOG_INFO("Estimated time exceeds 30 seconds, stopping search");
                break;
            }
        }
    }
    catch (TimeLimitExceededException &e)
    {
        board = prev_board;
        LOG_INFO("Time left: " + std::to_string(timer.return_rest_time()));
        LOG_WARNING(e.what());
    }
    // print_evaluation_statistics();
    // print_time_statistics();
    LOG_INFO("Best eval: " + std::to_string(best_eval));
    return best_board;
}

/// @brief calculates average branching factor by divide total_nodes by all valid moves to be able to estimate the number of nodes for next depth
void Algorithms::calculate_average_branching_factor()
{
    average_branching_factor = static_cast<double>(total_nodes) / static_cast<double>(total_valid_moves);
}

/// @brief estimates needed time by calculating the time per node and the estimated nodes for the next depth with the branching factor
/// @param current_depth
/// @param timer timer object to determine the time it took to get to this point, which results in a elapsed time that can be devided by total_nodes, because it gets called right after the last node was calculated
/// @return estimated time next depth search will take
double Algorithms::estimate_runtime_next_depth(uint8_t &current_depth, Timer &timer)
{
    if (average_branching_factor != -1.0)
    {
        double elapsed_time = timer.get_elapsed_time();
        double time_per_node = elapsed_time / (static_cast<double>(total_nodes));
        double estimated_nodes_next_depth = pow(static_cast<double>(average_branching_factor), static_cast<double>((current_depth + 1)));
#ifdef DEBUG

        LOG_INFO("Elapsed time: " + std::to_string(elapsed_time) + " no_nodes: " + std::to_string(total_nodes) + " time per node: " + std::to_string(time_per_node));
        LOG_INFO("Branching average: " + std::to_string(average_branching_factor / 1.5));
        LOG_INFO("Estimated nodes next depth: " + std::to_string(estimated_nodes_next_depth));
        LOG_INFO("Caculating: " + std::to_string(estimated_nodes_next_depth) + " * " + std::to_string(time_per_node));
#endif // DEBUG
        calculate_average_branching_factor();
        average_branching_factor = 1;
        return ((estimated_nodes_next_depth * time_per_node) /*/ ESTIMATED_TIME_DIVISOR*/); // commented for testing
    }
    else
    {
        return -1.0;
    }
}

/// @brief checks how many percent of the current map is occupied and sets a fitting max depth
/// @param max_search_depth current depth to set
/// @param b current board layout
void Algorithms::adapt_depth_to_map_progress(uint8_t &max_search_depth, Board &b)
{
    uint8_t old = max_search_depth;

    if (b.occupied_percentage > threshold_60_80)
    {
        max_search_depth = MAX_SEARCH_DEPTH;
    }
    else if (b.occupied_percentage > threshold_40_60 && max_search_depth < 7)
    {
        max_search_depth = 7;
    }
    else if (b.occupied_percentage > threshold_25_45 && max_search_depth < 4)
    {
        max_search_depth = 4;
    }
    else if (b.occupied_percentage > threshold_15_30 && max_search_depth < 2)
    {
        max_search_depth = 2;
    }
    if (old != max_search_depth)
        LOG_INFO("switched to max search depth: " + std::to_string(max_search_depth));
}

/// @brief calculates the threshold according to the current map size to be able to set the next depth sooner or later
/// @param scaling_factor scaling factor according to map size and actual playable fields
void Algorithms::calculate_thresholds(double scaling_factor)
{
    threshold_60_80 = small_map_threshold_60 + (large_map_threshold_80 - small_map_threshold_60) * scaling_factor;
    threshold_40_60 = small_map_threshold_40 + (large_map_threshold_60 - small_map_threshold_40) * scaling_factor;
    threshold_25_45 = small_map_threshold_25 + (large_map_threshold_45 - small_map_threshold_25) * scaling_factor;
    threshold_15_30 = small_map_threshold_15 + (large_map_threshold_30 - small_map_threshold_15) * scaling_factor;
}