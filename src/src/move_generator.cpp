#include "move_generator.hpp"
#include "initializer.hpp"
#include "algorithms.hpp"
#include "move_executer.hpp"
#include "timer.hpp"
#include "board.hpp"
#include "logging.hpp"

MoveGenerator::MoveGenerator() {}

MoveGenerator::MoveGenerator(Initializer &init)
{
    next_coords = init.next_coords;
    m_num_of_fields = init.get_num_of_fields();
    m_num_of_players = init.get_player_count();
}

MoveGenerator::~MoveGenerator() {}

/**
 *
 * HERE ARE JUST INITIALIZATIONS AND SETTER AND GETTER
 *
 */

uint16_t MoveGenerator::get_num_of_fields()
{
    return m_num_of_fields;
}

uint8_t MoveGenerator::get_num_of_players()
{
    return m_num_of_players;
}

uint8_t MoveGenerator::get_reverse_direction(uint8_t d)
{
    return (d + 4) % NUM_OF_DIRECTIONS;
}

/// @brief checks if the move is valid
/// @param board current board layout
/// @param c current field
/// @param player_number current player
/// @return true if the move is valid, false otherwise
bool MoveGenerator::check_if_valid_move(Board &board, uint16_t c, uint8_t player_number)
{
    for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d++)
    {
        std::vector<uint16_t> next_coords_vector = next_coords[(c - 1) * NUM_OF_DIRECTIONS + d];
        bool first = true;
        for (uint16_t i = 0; i < next_coords_vector.size(); i++)
        {
            uint16_t next_coord = next_coords_vector[i];
            if (first)
            {
                if (board.player_sets[player_number].test(next_coord))
                    break;
                else
                    first = false;
            }

            if (board.board_sets[EMPTY].test(next_coord))
                break;
            else if (board.player_sets[player_number].test(next_coord))
                return true;
        }
    }
    return false;
}

/// @brief calculates the valid no_overwrite_moves from player
/// @param board current board layout
/// @param player_number current player
/// @param c current field
/// @param index current community
void MoveGenerator::calculate_valid_no_overwrite_moves_from_player(Board &board, uint8_t player_number, uint16_t c, uint8_t index)
{
    for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d++)
    {
        std::vector<uint16_t> next_coords_vector = next_coords[(c - 1) * NUM_OF_DIRECTIONS + d];
        bool first = true;
        for (uint16_t i = 0; i < next_coords_vector.size(); i++)
        {
            uint16_t next_coord = next_coords_vector[i];
            if (board.player_sets[player_number].test(next_coord))
                break;
            if (first)
            {
                if (board.board_sets[EMPTY].test(next_coord))
                    break;
                else
                    first = false;
            }

            if (board.board_sets[EMPTY].test(next_coord))
            {
                board.valid_moves[player_number][index].set(next_coord);
                break;
            }
        }
    }
}

/// @brief calculates the valid overwrite_moves from player
/// @param board current board layout
/// @param player_number current player
/// @param c current field
/// @param index current community
void MoveGenerator::calculate_valid_overwrite_moves_from_player(Board &board, uint8_t player_number, uint16_t c, uint8_t index)
{
    for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d++)
    {
        std::vector<uint16_t> next_coords_vector = next_coords[(c - 1) * NUM_OF_DIRECTIONS + d];
        bool first = true;
        for (uint16_t i = 0; i < next_coords_vector.size(); i++)
        {
            uint16_t next_coord = next_coords_vector[i];
            if (first)
            {
                if (board.player_sets[player_number].test(next_coord) || board.board_sets[EMPTY].test(next_coord))
                    break;
                else
                {
                    first = false;
                    continue;
                }
            }

            if (board.board_sets[EMPTY].test(next_coord))
                break;
            else
                board.valid_moves[player_number][index].set(next_coord);
            if (board.player_sets[player_number].test(next_coord))
                break;
        }
    }
}

/// @brief calculates the valid no_overwrite_moves from player
/// @param board current board layout
/// @param player_number current player
/// @param index current community
void MoveGenerator::calculate_moves_from_player_no_ow(Board &board, uint8_t player_number, uint8_t index)
{
    for (uint16_t c = std::get<0>(board.start_end_communities[index]); c < std::get<1>(board.start_end_communities[index]) + 1; c++)
        if ((board.player_sets[player_number] & board.communities[index]).test(c))
            calculate_valid_no_overwrite_moves_from_player(board, player_number, c, index);
}

/// @brief calculates the valid overwrite_moves from player
/// @param board current board layout
/// @param player_number current player
/// @param timer timer object to check if time is up
/// @param index current community
void MoveGenerator::calculate_moves_from_player_ow(Board &board, uint8_t player_number, Timer &timer, uint8_t index)
{
    for (uint16_t c = std::get<0>(board.start_end_communities[index]); c < std::get<1>(board.start_end_communities[index]) + 1; c++)
    {
        if (timer.return_rest_time() < timer.exception_time)
            throw TimeLimitExceededException(("Timeout in move calculation from player"));

        if ((board.player_sets[player_number] & board.communities[index]).test(c))
            calculate_valid_overwrite_moves_from_player(board, player_number, c, index);
    }
}

/// @brief calculates the valid no_overwrite_moves from frame
/// @param board current board layout
/// @param player_number current player
/// @param index current community
void MoveGenerator::calculate_moves_from_frame_no_ow(Board &board, uint8_t player_number, uint8_t index)
{
    for (uint16_t c = std::get<0>(board.start_end_frames[index]); c < std::get<1>(board.start_end_frames[index]) + 1; c++)
        if (board.frames[index].test(c))
            if (check_if_valid_move(board, c, player_number))
                board.valid_moves[player_number][index].set(c);
}

/// @brief checks if player has no_overwrite moves from player
/// @param board current board layout
/// @param player_number current player
/// @param index current community
/// @return true if the player has no_overwrite moves, false otherwise
bool MoveGenerator::check_no_ow_moves_from_player(Board &board, uint8_t player_number, uint8_t index)
{
    for (uint16_t c = std::get<0>(board.start_end_communities[index]); c < std::get<1>(board.start_end_communities[index]) + 1; c++)
        if ((board.player_sets[player_number] & board.communities[index]).test(c))
            for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d++)
            {
                std::vector<uint16_t> next_coords_vector = next_coords[(c - 1) * NUM_OF_DIRECTIONS + d];
                bool first = true;
                for (uint16_t i = 0; i < next_coords_vector.size(); i++)
                {
                    uint16_t next_coord = next_coords_vector[i];
                    if (board.player_sets[player_number].test(next_coord))
                        break;
                    if (first)
                    {
                        if (board.board_sets[EMPTY].test(next_coord))
                            break;
                        else
                            first = false;
                    }

                    if (board.board_sets[EMPTY].test(next_coord))
                        return true;
                }
            }

    return false;
}

/// @brief checks if player has no_overwrite moves from frame
/// @param board current board layout
/// @param player_number current player
/// @param index current community
/// @return true if the player has no_overwrite moves, false otherwise
bool MoveGenerator::check_no_ow_moves_from_frame(Board &board, uint8_t player_number, uint8_t index)
{
    for (uint16_t c = std::get<0>(board.start_end_frames[index]); c < std::get<1>(board.start_end_frames[index]) + 1; c++)
        if (board.frames[index].test(c))
            if (check_if_valid_move(board, c, player_number))
                return true;

    return false;
}

/// @brief checks if player has no_overwrite_moves
/// @param board current board layout
/// @param player_number current player
/// @param index current community
/// @return true if the player has no_overwrite moves, false otherwise
bool MoveGenerator::check_if_player_has_no_overwrite_move(Board &board, uint8_t player_number, uint8_t index)
{
    if (2 * (board.communities[index] & board.player_sets[player_number]).count() < board.frames[index].count())
    {
        if (check_no_ow_moves_from_player(board, player_number, index))
            return true;
    }
    else if (check_no_ow_moves_from_frame(board, player_number, index))
        return true;

    return false;
}

/// @brief calculates the valid no_overwrite_moves for player
/// @param board current board layout
/// @param player_number current player
/// @param index current community
void MoveGenerator::calculate_valid_no_ow_moves(Board &board, uint8_t player_number, uint8_t &index)
{
    if (2 * (board.communities[index] & board.player_sets[player_number]).count() < board.frames[index].count())
    {
        calculate_moves_from_player_no_ow(board, player_number, index);
    }
    else
        calculate_moves_from_frame_no_ow(board, player_number, index);
}

/// @brief calculates the valid overwrite_moves for player
/// @param board current board layout
/// @param player_number current player
/// @param timer timer object to check if time is up
/// @param index current community
void MoveGenerator::calculate_valid_ow_moves(Board &board, uint8_t player_number, Timer &timer, uint8_t &index)
{
    board.set_overwrite_move(player_number);
    calculate_moves_from_player_ow(board, player_number, timer, index);
}

/// @brief generates the move
/// @param board current board layout
/// @param algorithms algorithms object
/// @param timer timer object to check if time is up
/// @param sorting true if the moves should be sorted, false otherwise
/// @return generated move
uint32_t MoveGenerator::generate_move(Board &board, Algorithms &algorithms, Timer &timer, bool sorting)
{
    uint8_t x, y, player;
    player = board.get_our_player();
    board.valid_moves[player].clear();
    board.valid_moves[player].resize(board.get_num_of_communities(), std::bitset<MAX_NUM_OF_FIELDS>(0));

    for (uint8_t index = 0; index < board.get_num_of_communities(); index++)
        if ((board.communities[index] & board.player_sets[player]).count() != 0)
            calculate_valid_no_ow_moves(board, player, index);

    if (board.get_total_moves(player).count() == 0 && board.has_overwrite_stones(player))
        for (uint8_t index = 0; index < board.get_num_of_communities(); index++)
        {
            if ((board.communities[index] & board.player_sets[player]).count() != 0)
                calculate_valid_ow_moves(board, player, timer, index);
            board.valid_moves[player][index] |= (board.board_sets[X] & board.communities[index]);
        }

    Board res = algorithms.get_best_coord(board, timer, sorting);
    if (board.board_sets[C].test(res.get_coord()))
        res.set_spec(res.get_spec() + 1);

    board.reset_overwrite_moves();
    board.one_dimension_2_second_dimension(res.get_coord(), x, y);
    uint32_t move = (uint32_t)x << TWO_BYTES | (uint32_t)y << BYTE | (uint32_t)res.get_spec();
    return move;
}

/// @brief sets value of affected_by_bomb
/// @param strength strength of the bomb
/// @param affected_by_bomb affected fields by the bomb
void MoveGenerator::get_affected_by_bomb(uint8_t strength, uint16_t &affected_by_bomb)
{
    if (strength > 0)
        while (strength > 0)
        {
            affected_by_bomb += strength * 8;
            strength--;
        }
}

/// @brief sorts players by number of stones
/// @param player_stones vector of pairs of player and number of stones
/// @param board current board layout
void MoveGenerator::sort_players_by_stones(std::vector<std::pair<uint8_t, uint16_t>> &player_stones, Board &board)
{
    for (uint8_t i = 0; i < m_num_of_players; i++)
        player_stones.push_back(std::make_pair(i, board.player_sets[i].count()));

    std::sort(player_stones.begin(), player_stones.end(), [](const std::pair<uint8_t, uint16_t> &a, const std::pair<uint8_t, uint16_t> &b)
              { return a.second > b.second; });
}

/// @brief selects target player
/// @param target_player target player
/// @param player_index index of the player
/// @param board current board layout
/// @param player_stones vector of pairs of player and number of stones
void MoveGenerator::select_target_player(uint8_t &target_player, uint8_t &player_index, Board &board, std::vector<std::pair<uint8_t, uint16_t>> &player_stones)
{
    for (uint8_t i = 0; i < board.get_player_count(); i++)
        if (player_stones[i].first == target_player)
        {
            player_index = i;
            if (i > 0)
                do
                    target_player = player_stones[i - 1].first;
                while (board.player_sets[target_player].count() == 0 && --i > 0);

            else
                do
                    target_player = player_stones[i + 1].first;
                while (board.player_sets[target_player].count() == 0 && ++i < board.get_player_count());

            break;
        }
}

/// @brief generates the bomb
/// @param board current board layout
/// @param init initializer object
/// @param timer timer object to check if time is up
uint32_t MoveGenerator::generate_bomb(Board &board, Initializer &init, Timer &timer)
{
    uint8_t x, y;
    uint16_t coord = 0;
    try
    {
        uint16_t affected_by_bomb = 1;
        get_affected_by_bomb(init.get_strength(), affected_by_bomb);
        // Sort players by number of stones in ascending order
        std::vector<std::pair<uint8_t, uint16_t>> player_stones;
        sort_players_by_stones(player_stones, board);
        uint8_t target_player = board.get_our_player(), player_index = 0;
        select_target_player(target_player, player_index, board, player_stones);
        if ((player_stones[target_player].second - player_stones[board.get_our_player()].second) > affected_by_bomb * board.get_bombs(board.get_our_player()) && (player_index + 1) < static_cast<int>(player_stones.size()))
            target_player = player_stones[player_index + 1].first;

        uint8_t counter = board.get_player_count();
        while (board.player_sets[target_player].count() == 0)
        {
            target_player = (target_player + 1) % init.get_player_count();
            counter--;
            if (counter == 0)
                break;
        }

        uint16_t most_deleted_stones = 0;
        std::bitset<MAX_NUM_OF_FIELDS> mask;
        for (uint16_t c = 1; c < m_num_of_fields; c++)
            if (!board.board_sets[MINUS].test(c))
                mask.set(c);

        MoveExecuter move_exec(init);
        for (uint16_t c = 1; c < m_num_of_fields; c++)
        {
            if (timer.return_rest_time() < timer.exception_time)
                throw TimeLimitExceededException(("Timeout in generate_bomb"));

            if (board.board_sets[MINUS].test(c))
                continue;

            std::bitset<MAX_NUM_OF_FIELDS> fields_to_remove(move_exec.get_fields_to_remove(board, c, init.get_strength(), mask));
            uint16_t target_deleted_stones = (board.player_sets[target_player] & fields_to_remove).count();
            uint16_t current_player_deleted_stones = (board.player_sets[board.get_our_player()] & fields_to_remove).count();
            if (target_deleted_stones > most_deleted_stones || (target_deleted_stones == most_deleted_stones && current_player_deleted_stones < (board.player_sets[board.get_our_player()] & fields_to_remove).count()))
            {
                most_deleted_stones = target_deleted_stones;
                coord = c;
            }
        }
        if (coord == 0)
            for (uint16_t c = 1; c < m_num_of_fields; c++)
                if (board.board_sets[EMPTY].test(c))
                {
                    coord = c;
                    break;
                }

        if (coord == 0)
            for (uint16_t c = 1; c < m_num_of_fields; c++)
                if (board.player_sets[board.get_our_player()].test(c))
                {
                    coord = c;
                    break;
                }
    }
    catch (const TimeLimitExceededException &)
    {
    }
    LOG_INFO("Bomb at: " + std::to_string(coord));
    init.one_dimension_2_second_dimension(coord, x, y);
    uint32_t bomb = (uint32_t)x << TWO_BYTES | (uint32_t)y << BYTE;
    return bomb;
}
