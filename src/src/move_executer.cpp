#include "move_executer.hpp"
#include "map.hpp"
#include "board.hpp"
#include "move_generator.hpp"
#include "timer.hpp"
#include "bomb_board.hpp"
#include "move_board.hpp"

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

void MoveExecuter::update_bits(std::bitset<2501> &to_color, uint8_t player, MoveBoard &move_board, Timer &timer)
{
    for (auto &bitset : move_board.get_board_sets())
    {
        bitset &= ~to_color;
    }
    for (auto &bitset : move_board.get_player_sets())
    {
        bitset &= ~to_color;
    }
    move_board.get_player_set(player) |= to_color;
    for (uint16_t c = 1; c < get_num_of_fields(); c++)
    {
        if (timer.return_rest_time() < timer.get_exception_time())
        {
            throw TimeLimitExceededException();
        }
        if (to_color.test(c))
        {
            for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d++)
            {
                uint16_t next_coord = get_transition(c, d);
                if (next_coord != 0 && move_board.get_board_set(1).test(next_coord))
                {
                    move_board.get_board_set(6).set(next_coord);
                }
            }
        }
    }
}

std::bitset<2501> MoveExecuter::get_bits_to_update(uint8_t player, MoveBoard &move_board, Timer &timer)
{
    uint16_t coord = move_board.get_coord();
    std::bitset<2501> to_color;
    to_color.set(coord);
    for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d++)
    {
        uint16_t temp_transition = get_transition(coord, d);
        uint8_t temp_direction = get_direction(coord, d);
        std::bitset<2501> temp;
        if (timer.return_rest_time() < timer.get_exception_time())
        {
            throw TimeLimitExceededException();
        }
        while (temp_transition != 0 && temp_transition != coord && !move_board.get_board_set(1).test(temp_transition))
        {
            if (move_board.get_player_set(player).test(temp_transition))
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

void MoveExecuter::update_boards(uint8_t player, uint8_t change_stones, MoveBoard &move_board, Timer &timer)
{
    uint16_t coord = move_board.get_coord();
    bool inversion = false;
    if (move_board.get_board_set(2).test(coord))
    {
        move_board.get_board_set(1).reset(coord);
        move_board.get_board_set(2).reset(coord);
        inversion = true;
    }
    if (timer.return_rest_time() < timer.get_exception_time())
    {
        throw TimeLimitExceededException();
    }
    std::bitset<2501> to_color = get_bits_to_update(player, move_board, timer);
    update_bits(to_color, player, move_board, timer);
    if (inversion)
    {
        uint16_t player_count = move_board.get_player_count();
        std::bitset<2501> temp_board = move_board.get_player_set(player_count - 1);
        for (uint8_t i = player_count - 1; i > 0; i--)
        {
            move_board.get_player_set(i) = move_board.get_player_set(i - 1);
        }
        move_board.get_player_set(0) = temp_board;
    }
    else if (change_stones)
    {
        std::bitset<2501> temp_board = move_board.get_player_set(player);
        move_board.get_player_set(player) = move_board.get_player_set(change_stones - 1);
        move_board.get_player_set(change_stones - 1) = temp_board;
    }
}

MoveBoard MoveExecuter::exec_move(uint8_t player, MoveBoard move_board, Timer &timer)
{
    uint16_t coord = move_board.get_coord();
    uint8_t spec = move_board.get_spec();
    uint8_t change_stones = 0;
    bool overwrite_move = false;
    if (!move_board.get_player_set(1).test(coord))
    {
        move_board.decrement_overwrite_stones(player);
        overwrite_move = true;
    }
    if (spec == 20)
    {
        move_board.get_board_set(1).reset(coord);
        move_board.get_board_set(4).reset(coord);
        move_board.increment_bombs(player);
    }
    else if (spec == 21)
    {
        move_board.get_board_set(1).reset(coord);
        move_board.get_board_set(4).reset(coord);
        move_board.increment_overwrite_stones(player);
    }
    else if (spec != 0)
    {
        move_board.get_board_set(1).reset(coord);
        move_board.get_board_set(3).reset(coord);
        change_stones = spec;
    }
    update_boards(player, change_stones, move_board, timer);
    return move_board;
}

void MoveExecuter::get_bomb_coords(uint16_t coord, BombBoard &bomb_board, uint8_t strength)
{
    if (strength >= 0)
    {
        for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d++)
        {
            uint16_t t = get_transition(coord, d);
            if (t != 0)
            {
                bomb_board.get_transitions_to_remove(coord).set((coord - 1) * 8 + d);
                bomb_board.get_transitions_to_remove(coord).set((coord - 1) * 8 + (d + 4) % 8);
            }
            if (uint8_t strength = bomb_board.get_strength() > 0)
            {
                get_bomb_coords(t, bomb_board, strength - 1);
            }
        }
    }
    else
    {
        return;
    }
}

BombBoard MoveExecuter::exec_bomb(uint8_t player, BombBoard bomb_board, Timer &timer)
{
    uint16_t coord = bomb_board.get_coord();
    bomb_board.get_fields_to_remove(coord).set(coord);
    for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d++)
    {
        uint16_t t = get_transition(coord, d);
        if (t != 0)
        {
            bomb_board.get_transitions_to_remove(coord).set((coord - 1) * 8 + d);
            bomb_board.get_transitions_to_remove(coord).set((coord - 1) * 8 + (d + 4) % 8);
        }
        if (uint8_t strength = bomb_board.get_strength() > 0)
        {
            get_bomb_coords(t, bomb_board, strength - 1);
        }
    }
    bomb_board.decrement_bombs(player);
    for (uint8_t i = 0; i < bomb_board.get_board_sets().size(); i++)
    {
        bomb_board.get_board_set(i) |= bomb_board.get_fields_to_remove(coord);
        bomb_board.get_board_set(i) &= ~bomb_board.get_fields_to_remove(coord);
    }
    for (uint8_t i = 0; i < bomb_board.get_player_sets().size(); i++)
    {
        bomb_board.get_player_set(i) |= bomb_board.get_fields_to_remove(coord);
        bomb_board.get_player_set(i) &= ~bomb_board.get_fields_to_remove(coord);
    }
    bomb_board.get_board_set(0) |= bomb_board.get_fields_to_remove(coord);
    return bomb_board;
}
