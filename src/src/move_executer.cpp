#include "move_executer.hpp"
#include "map.hpp"
#include "board.hpp"
#include "move_generator.hpp"
#include "timer.hpp"
#include "logging.hpp"

MoveExecuter::MoveExecuter() {}

MoveExecuter::MoveExecuter(Map &map)
{
    m_transitions = map.get_transitions();
    m_num_of_fields = map.get_num_of_fields();
    m_num_of_players = map.get_player_count();
    m_player_num = map.get_player_number();
}

MoveExecuter::~MoveExecuter() {}

uint16_t MoveExecuter::get_transition(uint16_t c, uint8_t d)
{
    return m_transitions[(c - 1) * 8 + d] / 10;
}

uint8_t MoveExecuter::get_direction(uint16_t c, uint8_t d)
{
    return m_transitions[(c - 1) * 8 + d] % 10;
}

uint16_t MoveExecuter::get_num_of_fields()
{
    return m_num_of_fields;
}

uint8_t MoveExecuter::get_num_of_players()
{
    return m_num_of_players;
}

uint8_t MoveExecuter::get_player_num()
{
    return m_player_num;
}

void MoveExecuter::update_bits(std::bitset<2501> &to_color, uint8_t player, Board &board, Timer &timer)
{
    for (auto &bitset : board.board_sets)
    {
        bitset &= ~to_color;
    }
    for (auto &bitset : board.player_sets)
    {
        bitset &= ~to_color;
    }
    board.player_sets[player] |= to_color;
    for (uint16_t c = 1; c < get_num_of_fields(); c++)
    {
        if (to_color.test(c))
        {
            for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d++)
            {
                uint16_t next_coord = get_transition(c, d);
                if (next_coord != 0 && board.board_sets[1].test(next_coord))
                {
                    board.board_sets[6].set(next_coord);
                }
            }
        }
    }
}

std::bitset<2501> MoveExecuter::get_bits_to_update(uint8_t player, Board &board, Timer &timer)
{
    uint16_t coord = board.get_coord();
    std::bitset<2501> to_color;
    to_color.set(coord);
    for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d++)
    {
        uint16_t temp_transition = get_transition(coord, d);
        uint8_t temp_direction = get_direction(coord, d);
        std::bitset<2501> temp;
        while (temp_transition != 0 && temp_transition != coord && !board.board_sets[1].test(temp_transition))
        {
            if (board.player_sets[player].test(temp_transition))
            {
                to_color |= temp;
                break;
            }
            else
            {
                temp.set(temp_transition);
                uint16_t next_transition = get_transition(temp_transition, temp_direction);
                uint8_t next_direction = get_direction(temp_transition, temp_direction);
                temp_transition = next_transition;
                temp_direction = next_direction;
            }
        }
    }
    return to_color;
}

void MoveExecuter::update_boards(uint8_t player, uint8_t change_stones, Board &board, Timer &timer)
{
    uint16_t coord = board.get_coord();
    bool inversion = false;
    if (board.board_sets[2].test(coord))
    {
        board.board_sets[1].reset(coord);
        board.board_sets[2].reset(coord);
        inversion = true;
    }
    std::bitset<2501> to_color = get_bits_to_update(player, board, timer);
    update_bits(to_color, player, board, timer);
    if (inversion)
    {
        uint16_t player_count = board.get_player_count();
        std::bitset<2501> temp_board = board.player_sets[player_count - 1];
        for (uint8_t i = player_count - 1; i > 0; i--)
        {
            board.player_sets[i] = board.player_sets[i - 1];
        }
        board.player_sets[0] = temp_board;
    }
    else if (change_stones)
    {
        std::bitset<2501> temp_board = board.player_sets[player];
        board.player_sets[player] = board.player_sets[change_stones - 1];
        board.player_sets[change_stones - 1] = temp_board;
    }
}

// void MoveExecuter::adjust_protected_fields(Board &board, uint8_t player)
// {
//     for (uint16_t c = 1; c < board.get_num_of_fields(); c++)
//     {
//         if (board.protected_fields[player].test(c))
//         {
//             for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d++)
//             {
//                 uint16_t next_coord = get_transition(c, d);
//                 if (next_coord == 0 || board.protected_fields[player].test(next_coord))
//                 {
//                     uint16_t trans1 = get_transition(c, (d + 3) % NUM_OF_DIRECTIONS);
//                     uint16_t trans2 = get_transition(c, (d + 4) % NUM_OF_DIRECTIONS);
//                     uint16_t trans3 = get_transition(c, (d + 5) % NUM_OF_DIRECTIONS);
//                     bool found_proteceted = false;
//                     if (test_if_protected(board, player, trans1))
//                     {
//                         found_proteceted = true;
//                         board.protected_fields[player].set(trans1);
//                     }
//                 }
//             }
//         }
//     }
// }

void MoveExecuter::exec_move(uint8_t player, Board &board, Timer &timer)
{
    uint16_t coord = board.get_coord();
    uint8_t spec = board.get_spec();
    uint8_t change_stones = 0;
    bool overwrite_move = false;
    if (!board.board_sets[1].test(coord))
    {
        board.decrement_overwrite_stones(player);
        overwrite_move = true;
    }
    if (spec == 20)
    {
        board.board_sets[1].reset(coord);
        board.board_sets[4].reset(coord);
        board.increment_bombs(player);
    }
    else if (spec == 21)
    {
        board.board_sets[1].reset(coord);
        board.board_sets[4].reset(coord);
        board.increment_overwrite_stones(player);
    }
    else if (spec != 0)
    {
        board.board_sets[1].reset(coord);
        board.board_sets[3].reset(coord);
        change_stones = spec;
    }
    update_boards(player, change_stones, board, timer);
}

void MoveExecuter::get_bomb_coords(uint16_t start_coord, uint16_t c, uint8_t strength, std::bitset<2501> &mask, Board &board)
{
    if (strength == 0 || board.fields_to_remove[start_coord] == mask)
    {
        return;
    }
    for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d++)
    {
        uint16_t next_coord = get_transition(c, d);
        if (next_coord != 0 && !board.board_sets[0].test(next_coord) && next_coord != start_coord)
        {
            board.fields_to_remove[start_coord].set(next_coord);
            get_bomb_coords(start_coord, next_coord, strength - 1, mask, board);
        }
    }
}

void MoveExecuter::init_bomb_phase_boards(Board &board, uint16_t coord, uint8_t strength)
{
    std::bitset<2501> mask;
    for (uint16_t c = 1; c < m_num_of_fields; c++)
    {
        if (!board.board_sets[0].test(c))
            mask.set(c);
    }
    board.fields_to_remove[coord].set(coord);
    if (strength > 0)
    {
        for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d++)
        {
            uint16_t next_coord = get_transition(coord, d);
            if (next_coord != 0 && !board.board_sets[0].test(next_coord) && next_coord != coord)
            {
                board.fields_to_remove[coord].set(next_coord);
                get_bomb_coords(coord, next_coord, strength - 1, mask, board);
            }
        }
    }
}

Board MoveExecuter::exec_bomb(uint8_t player, Board board, Timer &timer, uint8_t strength)
{
    LOG_INFO("exec");
    uint16_t coord = board.get_coord();
    init_bomb_phase_boards(board, coord, strength);
    board.decrement_bombs(player);
    for (uint8_t i = 0; i < board.board_sets.size(); i++)
    {
        board.board_sets[i] |= board.fields_to_remove[coord];
        board.board_sets[i] &= ~board.fields_to_remove[coord];
    }
    for (uint8_t i = 0; i < board.player_sets.size(); i++)
    {
        board.player_sets[i] |= board.fields_to_remove[coord];
        board.player_sets[i] &= ~board.fields_to_remove[coord];
    }
    board.board_sets[0] |= board.fields_to_remove[coord];
    return board;
}
