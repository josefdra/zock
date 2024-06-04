#include "move_generator.hpp"
#include "map.hpp"
#include "algorithms.hpp"
#include "move_executer.hpp"
#include "timer.hpp"
#include "board.hpp"
#include "logging.hpp"

MoveGenerator::MoveGenerator() {}

MoveGenerator::MoveGenerator(Map &map)
{
    m_transitions = map.get_transitions();
    m_num_of_fields = map.get_num_of_fields();
    m_num_of_players = map.get_player_count();
    m_player_num = map.get_player_number();
}

MoveGenerator::~MoveGenerator() {}

uint16_t MoveGenerator::get_transition(uint16_t c, uint8_t d)
{
    return m_transitions[(c - 1) * 8 + d] / 10;
}

uint8_t MoveGenerator::get_direction(uint16_t c, uint8_t d)
{
    return m_transitions[(c - 1) * 8 + d] % 10;
}

uint16_t MoveGenerator::get_num_of_fields()
{
    return m_num_of_fields;
}

uint8_t MoveGenerator::get_num_of_players()
{
    return m_num_of_players;
}

uint8_t MoveGenerator::get_player_num()
{
    return m_player_num;
}

bool MoveGenerator::check_if_valid_move(Board &board, uint16_t c, uint8_t player_number, Timer &timer)
{
    try
    {
        for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d++)
        {
            uint16_t next_coord = get_transition(c, d);
            uint8_t next_direction = get_direction(c, d);
            uint16_t prev_coord = c;
            uint8_t prev_direction = d;
            if (timer.return_rest_time() < timer.exception_time)
            {
                throw TimeLimitExceededException("check_if_valid_move");
            }
            while (next_coord != 0 && next_coord != c && !board.board_sets[1].test(next_coord) && !board.player_sets[player_number].test(next_coord))
            {
                prev_coord = next_coord;
                prev_direction = next_direction;
                next_coord = get_transition(prev_coord, prev_direction);
                if (next_coord == 0)
                    break;
                if (board.player_sets[player_number].test(next_coord))
                {
                    return true;
                }
                next_direction = get_direction(prev_coord, prev_direction);
                if ((prev_direction + 4) % 8 == next_direction)
                {
                    break;
                }
            }
        }
    }
    catch (TimeLimitExceededException)
    {
        throw;
    }
    return false;
}

void MoveGenerator::calculate_valid_no_overwrite_moves_from_player(Board &board, uint8_t player_number, uint16_t c, Timer &timer)
{
    try
    {
        for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d++)
        {
            if (timer.return_rest_time() < timer.exception_time)
            {
                throw TimeLimitExceededException(("Timeout in function calculate_valid_no_overwrite_moves_from_player in for loop."));
            }
            uint16_t next_coord = get_transition(c, d);
            uint8_t next_direction = get_direction(c, d);
            uint16_t prev_coord = c;
            uint8_t prev_direction = d;
            while (next_coord != 0 && next_coord != c && !board.board_sets[1].test(next_coord) && !board.player_sets[player_number].test(next_coord))
            {
                if (timer.return_rest_time() < timer.exception_time)
                {
                    throw TimeLimitExceededException(("Timeout in function calculate_valid_no_overwrite_moves_from_player in while loop."));
                }
                prev_coord = next_coord;
                prev_direction = next_direction;
                next_coord = get_transition(prev_coord, prev_direction);
                if (next_coord == 0)
                    break;
                if (board.board_sets[1].test(next_coord))
                {
                    board.valid_moves[player_number].set(next_coord);
                }
                next_direction = get_direction(prev_coord, prev_direction);
                if ((prev_direction + 4) % 8 == next_direction)
                {
                    break;
                }
            }
        }
    }
    catch (TimeLimitExceededException)
    {
        throw;
    }
}

void MoveGenerator::calculate_valid_overwrite_moves_from_player(Board &board, uint8_t player_number, uint16_t c, Timer &timer)
{
    try
    {

        for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d++)
        {

            uint16_t next_coord = get_transition(c, d);
            uint8_t next_direction = get_direction(c, d);
            uint16_t prev_coord = c;
            uint8_t prev_direction = d;
            if (next_coord == 0 || board.player_sets[player_number].test(next_coord))
                continue;
            while (!board.board_sets[1].test(next_coord) && next_coord != c && !board.player_sets[player_number].test(next_coord))
            {
                if (timer.return_rest_time() < timer.exception_time)
                {
                    throw TimeLimitExceededException(("Timeout in function calculate_valid_overwrite_moves_from_player"));
                }
                prev_coord = next_coord;
                prev_direction = next_direction;
                next_coord = get_transition(prev_coord, prev_direction);
                if (next_coord == 0 || board.board_sets[1].test(next_coord))
                    break;
                board.valid_moves[player_number].set(next_coord);
                next_direction = get_direction(prev_coord, prev_direction);
                if ((prev_direction + 4) % 8 == next_direction)
                {
                    break;
                }
            }
        }
    }
    catch (TimeLimitExceededException)
    {
        throw;
    }
}

void MoveGenerator::calculate_moves_from_player(Board &board, uint8_t player_number, Timer &timer)
{
    try
    {
        for (uint16_t c = 1; c < m_num_of_fields; c++)
        {

            if (board.player_sets[player_number].test(c))
            {
                calculate_valid_no_overwrite_moves_from_player(board, player_number, c, timer);
            }
        }
        if (board.valid_moves[player_number].count() == 0 && board.has_overwrite_stones(player_number))
        {

            board.set_overwrite_move(player_number);
            for (uint16_t c = 1; c < m_num_of_fields; c++)
            {

                if (board.player_sets[player_number].test(c))
                {
                    calculate_valid_overwrite_moves_from_player(board, player_number, c, timer);
                }
                if (board.board_sets[5].test(c))
                {
                    board.valid_moves[player_number].set(c);
                }
            }
        }
    }
    catch (TimeLimitExceededException)
    {
        throw;
    }
}

void MoveGenerator::calculate_moves_from_frame(Board &board, uint8_t player_number, Timer &timer)
{
    for (uint16_t c = 1; c < m_num_of_fields; c++)
    {
        if (board.board_sets[6].test(c))
        {
            if (check_if_valid_move(board, c, player_number, timer))
                board.valid_moves[player_number].set(c);
        }
    }
    if (board.valid_moves[player_number].count() == 0 && board.has_overwrite_stones(player_number))
    {
        board.set_overwrite_move(player_number);
        for (uint16_t c = 1; c < m_num_of_fields; c++)
        {
            if (!board.board_sets[1].test(c))
            {
                if (check_if_valid_move(board, c, player_number, timer))
                    board.valid_moves[player_number].set(c);
            }
            if (board.board_sets[5].test(c))
            {
                board.valid_moves[player_number].set(c);
            }
        }
    }
}

void MoveGenerator::calculate_valid_moves(Board &board, uint8_t player_number, Timer &timer)
{
    board.valid_moves[player_number].reset();
    if (2 * board.player_sets[player_number].count() < board.board_sets[6].count())
    {
        calculate_moves_from_player(board, player_number, timer);
    }
    else
    {
        calculate_moves_from_frame(board, player_number, timer);
    }
}

uint32_t MoveGenerator::generate_move(Board &board, Map &map, Timer &timer, bool sorting)
{

    MoveExecuter move_exec(map);
    MoveGenerator move_gen(map);
    Algorithms algorithms(move_exec, move_gen);
    uint8_t x, y, player;
    player = map.get_player_number();
    try
    {
        calculate_valid_moves(board, player, timer);
    }
    catch (TimeLimitExceededException &e)
    {
        LOG_WARNING(e.what());
    }
    Board res = algorithms.get_best_coord(board, timer, sorting);
    if (board.board_sets[3].test(res.get_coord()))
    {
        res.set_spec(res.get_spec() + 1);
    }
    board.reset_overwrite_moves();
    map.one_dimension_2_second_dimension(res.get_coord(), x, y);
    uint32_t move = (uint32_t)x << 16 | (uint32_t)y << 8 | (uint32_t)res.get_spec();
    return move;
}

void MoveGenerator::get_bomb_coords(uint16_t start_coord, uint16_t c, uint8_t strength, std::bitset<2501> &mask, Board &board)
{
    if (strength == 0 || board.fields_to_remove[start_coord] == mask)
    {
        return;
    }
    for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d++)
    {
        uint16_t next_coord = get_transition(c, d);
        if (next_coord != 0 && next_coord != start_coord)
        {
            board.fields_to_remove[start_coord].set(next_coord);
            get_bomb_coords(start_coord, next_coord, strength - 1, mask, board);
        }
    }
}

void MoveGenerator::init_bomb_phase_boards(Board &board, uint8_t strength, uint8_t player)
{
    std::bitset<2501> mask;
    for (uint16_t c = 1; c < m_num_of_fields; c++)
    {
        if (!board.board_sets[0].test(c))
            mask.set(c);
    }
    for (uint16_t c = 1; c < m_num_of_fields; c++)
    {
        board.fields_to_remove[c].set(c);
        if (strength > 0)
        {
            for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d++)
            {
                uint16_t next_coord = get_transition(c, d);
                if (next_coord != 0 && next_coord != c)
                {
                    board.fields_to_remove[c].set(next_coord);
                    get_bomb_coords(c, next_coord, strength - 1, mask, board);
                }
            }
        }
    }
}

uint32_t MoveGenerator::generate_bomb(Board &board, Map &map, Timer &timer, bool sorting)
{
    uint8_t x, y;
    uint16_t coord = 0;
    // uint16_t affected_by_bomb = 1;
    // if (map.get_strength() > 0)
    //     affected_by_bomb = map.get_strength() * 8;
    // // Sort players by number of stones in ascending order
    // std::vector<std::pair<uint8_t, uint16_t>> player_stones;
    // for (uint8_t i = 0; i < m_num_of_players; i++)
    // {
    //     player_stones.push_back(std::make_pair(i, board.player_sets[i].count()));
    // }
    // std::sort(player_stones.begin(), player_stones.end(), [](const std::pair<uint8_t, uint16_t> &a, const std::pair<uint8_t, uint16_t> &b)
    //           { return a.second > b.second; });
    // uint8_t target_player = (map.get_player_number() + 1) % map.get_player_count();
    // uint8_t player_index;
    // for (uint8_t i = 0; i < player_stones.size(); ++i)
    // {
    //     if (player_stones[i].first == map.get_player_number())
    //     {
    //         player_index = i;
    //         if (i > 0)
    //         {
    //             target_player = player_stones[i - 1].first;
    //         }
    //         else
    //         {
    //             target_player = player_stones[i + 1].first;
    //         }
    //         break;
    //     }
    // }
    // if ((player_stones[target_player].second - player_stones[map.get_player_number()].second) > affected_by_bomb * board.get_bombs(map.get_player_number()) && (player_index + 1) < player_stones.size())
    //     target_player = player_stones[player_index + 1].first;
    // if ((player_index + 1) < player_stones.size())
    // {
    //     target_player = map.get_player_number();
    // }
    // uint16_t most_deleted_stones = 0;
    // for (uint16_t c = 1; c < m_num_of_fields; c++)
    // {
    //     if (board.board_sets[0].test(c))
    //         continue;
    //     uint16_t target_deleted_stones = (board.player_sets[target_player] & board.fields_to_remove[c]).count();
    //     uint16_t current_player_deleted_stones = (board.player_sets[map.get_player_number()] & board.fields_to_remove[c]).count();
    //     if (target_deleted_stones > most_deleted_stones || (target_deleted_stones == most_deleted_stones && current_player_deleted_stones < (board.player_sets[map.get_player_number()] & board.fields_to_remove[c]).count()))
    //     {
    //         most_deleted_stones = target_deleted_stones;
    //         coord = c;
    //     }
    // }
    // if (coord = 0)
    {
        for (uint16_t c = 1; c < m_num_of_fields; c++)
        {
            if (!board.board_sets[0].test(c) && !board.player_sets[map.get_player_number()].test(c))
            {
                coord = c;
                break;
            }
        }
    }
    if(coord = 0){
        for (uint16_t c = 1; c < m_num_of_fields; c++)
        {
            if (!board.board_sets[0].test(c))
            {
                coord = c;
                break;
            }
        }
    }
    map.one_dimension_2_second_dimension(coord, x, y);
    uint32_t bomb = (uint32_t)x << 16 | (uint32_t)y << 8;
    return bomb;
}