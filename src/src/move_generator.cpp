#include "move_generator.hpp"
#include "map.hpp"
#include "algorithms.hpp"
#include "move_executer.hpp"
#include "timer.hpp"
#include "board.hpp"

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
    for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d++)
    {
        uint16_t next_coord = get_transition(c, d);
        uint8_t next_direction = get_direction(c, d);
        uint16_t prev_coord = c;
        uint8_t prev_direction = d;
        while (next_coord != 0 && next_coord != c && !board.board_sets[1].test(next_coord) && !board.player_sets[player_number].test(next_coord))
        {
            if (timer.return_rest_time() < timer.exception_time)
            {
                throw TimeLimitExceededException();
            }
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
    return false;
}

void MoveGenerator::calculate_valid_no_overwrite_moves_from_player(Board &board, uint8_t player_number, uint16_t c)
{
    for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d++)
    {
        uint16_t next_coord = get_transition(c, d);
        uint8_t next_direction = get_direction(c, d);
        uint16_t prev_coord = c;
        uint8_t prev_direction = d;
        while (next_coord != 0 && next_coord != c && !board.board_sets[1].test(next_coord) && !board.player_sets[player_number].test(next_coord))
        {
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

void MoveGenerator::calculate_valid_overwrite_moves_from_player(Board &board, uint8_t player_number, uint16_t c)
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

void MoveGenerator::calculate_moves_from_player(Board &board, uint8_t player_number, Timer &timer)
{
    for (uint16_t c = 1; c < m_num_of_fields; c++)
    {
        if (board.player_sets[player_number].test(c))
        {
            calculate_valid_no_overwrite_moves_from_player(board, player_number, c);
            if (timer.return_rest_time() < timer.exception_time)
            {
                throw TimeLimitExceededException();
            }
        }
    }
    if (board.valid_moves[player_number].count() == 0 && board.has_overwrite_stones(player_number))
    {
        board.set_overwrite_move(player_number);
        for (uint16_t c = 1; c < m_num_of_fields; c++)
        {
            if (board.player_sets[player_number].test(c))
            {
                calculate_valid_overwrite_moves_from_player(board, player_number, c);
                if (timer.return_rest_time() < timer.exception_time)
                {
                    throw TimeLimitExceededException();
                }
            }
            if (board.board_sets[5].test(c))
            {
                board.valid_moves[player_number].set(c);
            }
        }
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
            if (timer.return_rest_time() < timer.exception_time)
            {
                throw TimeLimitExceededException();
            }
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
                if (timer.return_rest_time() < timer.exception_time)
                {
                    throw TimeLimitExceededException();
                }
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

uint32_t MoveGenerator::generate_move(Board &board, Map &map, Timer &timer, uint8_t search_depth, bool sorting)
{
    if (search_depth == 1)
    {
        search_depth = 5;
        timer.exception_time = (search_depth + 1) * 5000;
    }
    MoveExecuter move_exec(map);
    MoveGenerator move_gen(map);
    MiniMax minimax(move_exec, move_gen);
    uint8_t x, y, player;
    player = map.get_player_number();
    calculate_valid_moves(board, player, timer);
    Board res = minimax.get_best_coord(board, timer, search_depth, sorting);
    board.reset_overwrite_moves();
    std::cout << "Debug print, to see valid moves, before move executed" << std::endl;
    board.print(player, true);
    map.one_dimension_2_second_dimension(res.get_coord(), x, y);
    uint32_t move = (uint32_t)x << 16 | (uint32_t)y << 8 | (uint32_t)res.get_spec();
    return move;
}

uint32_t MoveGenerator::generate_bomb(Board &board, Map &map, Timer &timer, uint8_t search_depth, bool sorting)
{
    uint32_t bomb = 0;
    return bomb;
}