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
    return m_transitions[(c - 1) * NUM_OF_DIRECTIONS + d] / COORD_TO_DIR_OFFSET;
}

uint8_t MoveGenerator::get_direction(uint16_t c, uint8_t d)
{
    return m_transitions[(c - 1) * NUM_OF_DIRECTIONS + d] % COORD_TO_DIR_OFFSET;
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

uint8_t MoveGenerator::get_reverse_direction(uint8_t d)
{
    return (d + 4) % NUM_OF_DIRECTIONS;
}

bool MoveGenerator::check_if_valid_move(Board &board, uint16_t c, uint8_t player_number)
{
    for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d++)
    {
        uint16_t next_coord = get_transition(c, d);
        uint8_t next_direction = get_direction(c, d);
        uint16_t prev_coord = c;
        uint8_t prev_direction = d;
        while (next_coord != 0 && next_coord != c && !board.board_sets[EMPTY].test(next_coord) && !board.player_sets[player_number].test(next_coord))
        {
            prev_coord = next_coord;
            prev_direction = next_direction;
            next_coord = get_transition(prev_coord, prev_direction);
            if (next_coord == 0)
                break;
            if (board.player_sets[player_number].test(next_coord))
                return true;

            next_direction = get_direction(prev_coord, prev_direction);
            if (get_reverse_direction(prev_direction) == next_direction && prev_coord == next_coord)
                break;
        }
    }
    return false;
}

void MoveGenerator::calculate_valid_no_overwrite_moves_from_player(Board &board, uint8_t player_number, uint16_t c, uint8_t index)
{
    for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d++)
    {
        uint16_t next_coord = get_transition(c, d);
        uint8_t next_direction = get_direction(c, d);
        uint16_t prev_coord = c;
        uint8_t prev_direction = d;
        while (next_coord != 0 && next_coord != c && !board.board_sets[EMPTY].test(next_coord) && !board.player_sets[player_number].test(next_coord))
        {
            prev_coord = next_coord;
            prev_direction = next_direction;
            next_coord = get_transition(prev_coord, prev_direction);
            if (next_coord == 0)
                break;

            if (board.board_sets[EMPTY].test(next_coord))
                board.valid_moves[player_number][index].set(next_coord);

            next_direction = get_direction(prev_coord, prev_direction);
            if (get_reverse_direction(prev_direction) == next_direction && prev_coord == next_coord)
                break;
        }
    }
}

void MoveGenerator::calculate_valid_overwrite_moves_from_player(Board &board, uint8_t player_number, uint16_t c, uint8_t index)
{
    for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d++)
    {

        uint16_t next_coord = get_transition(c, d);
        uint8_t next_direction = get_direction(c, d);
        uint16_t prev_coord = c;
        uint8_t prev_direction = d;
        if (next_coord == 0 || board.player_sets[player_number].test(next_coord))
            continue;

        while (!board.board_sets[EMPTY].test(next_coord) && next_coord != c && !board.player_sets[player_number].test(next_coord))
        {
            prev_coord = next_coord;
            prev_direction = next_direction;
            next_coord = get_transition(prev_coord, prev_direction);
            if (next_coord == 0 || board.board_sets[EMPTY].test(next_coord) || next_coord == c)
                break;

            board.valid_moves[player_number][index].set(next_coord);
            next_direction = get_direction(prev_coord, prev_direction);
            if (get_reverse_direction(prev_direction) == next_direction && prev_coord == next_coord)
                break;
        }
    }
}

void MoveGenerator::calculate_moves_from_player_no_ow(Board &board, uint8_t player_number, uint8_t index)
{
    for (uint16_t c = std::get<0>(board.start_end_communities[index]); c < std::get<1>(board.start_end_communities[index]) + 1; c++)
        if ((board.player_sets[player_number] & board.communities[index]).test(c))
            calculate_valid_no_overwrite_moves_from_player(board, player_number, c, index);
}

void MoveGenerator::calculate_moves_from_player_ow(Board &board, uint8_t player_number, Timer &timer, uint8_t index)
{
    for (uint16_t c = std::get<0>(board.start_end_communities[index]); c < std::get<1>(board.start_end_communities[index]) + 1; c++)
    {
        if (timer.return_rest_time() < timer.exception_time)
            throw TimeLimitExceededException(("Timeout in move calculation from player"));

        if ((board.player_sets[player_number] & board.communities[index]).test(c))
            calculate_valid_overwrite_moves_from_player(board, player_number, c, index);

        if (board.board_sets[X].test(c))
            board.valid_moves[player_number][index].set(c);
    }
}

void MoveGenerator::calculate_moves_from_frame_no_ow(Board &board, uint8_t player_number, uint8_t index)
{
    for (uint16_t c = std::get<0>(board.start_end_frames[index]); c < std::get<1>(board.start_end_frames[index]) + 1; c++)
        if (board.frames[index].test(c))
            if (check_if_valid_move(board, c, player_number))
                board.valid_moves[player_number][index].set(c);
}

void MoveGenerator::calculate_valid_no_ow_moves(Board &board, uint8_t player_number, uint8_t &index)
{
    if (2 * (board.communities[index] & board.player_sets[player_number]).count() < board.frames[index].count())
        calculate_moves_from_player_no_ow(board, player_number, index);
    else
        calculate_moves_from_frame_no_ow(board, player_number, index);
}

void MoveGenerator::calculate_valid_ow_moves(Board &board, uint8_t player_number, Timer &timer, uint8_t &index)
{
    board.set_overwrite_move(player_number);
    calculate_moves_from_player_ow(board, player_number, timer, index);
}

uint32_t MoveGenerator::generate_move(Board &board, Map &map, Timer &timer, bool sorting)
{
    MoveExecuter move_exec(map);
    MoveGenerator move_gen(map);
    Algorithms algorithms(move_exec, move_gen);
    uint8_t x, y, player;
    player = map.get_player_number();
    board.valid_moves[player].clear();
    board.valid_moves[player].resize(board.get_num_of_communities());

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
    map.one_dimension_2_second_dimension(res.get_coord(), x, y);
    uint32_t move = (uint32_t)x << TWO_BYTES | (uint32_t)y << BYTE | (uint32_t)res.get_spec();
    return move;
}

void MoveGenerator::get_affected_by_bomb(uint8_t strength, uint16_t &affected_by_bomb)
{
    if (strength > 0)
        while (strength > 0)
        {
            affected_by_bomb += strength * 8;
            strength--;
        }
}

void MoveGenerator::sort_players_by_stones(std::vector<std::pair<uint8_t, uint16_t>> &player_stones, Board &board)
{
    for (uint8_t i = 0; i < m_num_of_players; i++)
        player_stones.push_back(std::make_pair(i, board.player_sets[i].count()));

    std::sort(player_stones.begin(), player_stones.end(), [](const std::pair<uint8_t, uint16_t> &a, const std::pair<uint8_t, uint16_t> &b)
              { return a.second > b.second; });
}

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

uint32_t MoveGenerator::generate_bomb(Board &board, Map &map, Timer &timer)
{
    uint8_t x, y;
    uint16_t coord = 0;
    try
    {
        uint16_t affected_by_bomb = 1;
        get_affected_by_bomb(map.get_strength(), affected_by_bomb);
        // Sort players by number of stones in ascending order
        std::vector<std::pair<uint8_t, uint16_t>> player_stones;
        sort_players_by_stones(player_stones, board);
        uint8_t target_player = map.get_player_number(), player_index = 0;
        select_target_player(target_player, player_index, board, player_stones);
        if ((player_stones[target_player].second - player_stones[map.get_player_number()].second) > affected_by_bomb * board.get_bombs(map.get_player_number()) && (player_index + 1) < static_cast<int>(player_stones.size()))
            target_player = player_stones[player_index + 1].first;

        uint8_t counter = board.get_player_count();
        while (board.player_sets[target_player].count() == 0)
        {
            target_player = (target_player + 1) % map.get_player_count();
            counter--;
            if (counter == 0)
                break;
        }

        uint16_t most_deleted_stones = 0;
        std::bitset<MAX_NUM_OF_FIELDS> mask;
        for (uint16_t c = 1; c < m_num_of_fields; c++)
            if (!board.board_sets[MINUS].test(c))
                mask.set(c);

        MoveExecuter move_exec(map);
        for (uint16_t c = 1; c < m_num_of_fields; c++)
        {
            if (timer.return_rest_time() < timer.exception_time)
                throw TimeLimitExceededException(("Timeout in generate_bomb"));

            if (board.board_sets[MINUS].test(c))
                continue;

            std::bitset<MAX_NUM_OF_FIELDS> fields_to_remove(move_exec.get_fields_to_remove(board, c, map.get_strength(), mask));
            uint16_t target_deleted_stones = (board.player_sets[target_player] & fields_to_remove).count();
            uint16_t current_player_deleted_stones = (board.player_sets[map.get_player_number()] & fields_to_remove).count();
            if (target_deleted_stones > most_deleted_stones || (target_deleted_stones == most_deleted_stones && current_player_deleted_stones < (board.player_sets[map.get_player_number()] & fields_to_remove).count()))
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
                if (board.player_sets[map.get_player_number()].test(c))
                {
                    coord = c;
                    break;
                }
    }
    catch (const TimeLimitExceededException &)
    {
    }
    LOG_INFO("Bomb at: " + std::to_string(coord));
    map.one_dimension_2_second_dimension(coord, x, y);
    uint32_t bomb = (uint32_t)x << TWO_BYTES | (uint32_t)y << BYTE;
    return bomb;
}
