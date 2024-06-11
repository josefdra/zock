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

void MoveExecuter::update_bits(std::bitset<2501> &to_color, uint8_t player, Board &board)
{
    for (auto &bitset : board.board_sets)
        bitset &= ~to_color;

    for (auto &bitset : board.player_sets)
        bitset &= ~to_color;

    board.player_sets[player] |= to_color;
}

std::bitset<2501> MoveExecuter::get_bits_to_update(uint8_t player, Board &board)
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
    return to_color;
}

void MoveExecuter::update_communities(std::bitset<2501> &to_color, Board &board)
{
    for (auto &community : board.communities)
        if ((community & to_color).count() != 0)
            community |= to_color;

    for (uint8_t p = 0; p < board.get_player_count(); p++)
        for (auto &community : board.player_communities[p])
            if ((community & to_color).count() != 0)
                community |= to_color;
}

void MoveExecuter::update_frames(std::bitset<2501> &to_color, Board &board)
{
    std::bitset<2501> temp;
    for (uint16_t c = 1; c < m_num_of_fields; c++)
        if (to_color.test(c))
            for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d++)
            {
                uint16_t next_coord = get_transition(c, d);
                if (next_coord != 0 && board.board_sets[1].test(next_coord))
                    temp.set(next_coord);
            }

    for (uint8_t p = 0; p < board.get_player_count(); p++)
        for (auto &frame : board.player_frames[p])
            if ((frame & to_color).count() != 0)
            {
                frame &= ~to_color;
                frame |= temp;
            }
}

void MoveExecuter::adjust_communities(Board &board)
{
    bool changes = true;
    while (changes)
    {
        changes = false;
        for (auto &community : board.communities)
            for (uint8_t p = 0; p < board.get_player_count(); p++)
                for (uint8_t i = 0; i < board.player_frames[p].size(); i++)
                    if ((community & board.player_frames[p][i]).count() != 0)
                    {
                        community |= board.player_communities[p][i];
                        board.player_frames[p][i].reset();
                        changes = true;
                    }
    }
    board.remove_double_communities();
}

void MoveExecuter::adjust_frames(Board &board)
{
    for (uint8_t p = 0; p < m_num_of_players; p++)
    {
        board.player_frames[p].clear();
        board.player_frames[p].resize(board.player_communities[p].size());
    }
    for (uint16_t c = 1; c < get_num_of_fields(); c++)
        for (uint8_t p = 0; p < m_num_of_players; p++)
            for (uint8_t i = 0; i < board.player_communities[p].size(); i++)
                if (board.player_communities[p][i].test(c))
                    for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d++)
                    {
                        uint16_t next_coord = get_transition(c, d);
                        if (next_coord != 0 && board.board_sets[1].test(next_coord))
                            board.player_frames[p][i].set(next_coord);
                    }
}

void MoveExecuter::update_boards(uint8_t player, uint8_t change_stones, Board &board)
{
    uint16_t coord = board.get_coord();
    bool inversion = false;
    if (board.board_sets[2].test(coord))
    {
        board.board_sets[1].reset(coord);
        board.board_sets[2].reset(coord);
        inversion = true;
    }
    std::bitset<2501> to_color = get_bits_to_update(player, board);
    update_bits(to_color, player, board);
    update_communities(to_color, board);
    update_frames(to_color, board);
    adjust_communities(board);
    adjust_frames(board);
    if (inversion)
    {
        uint16_t player_count = board.get_player_count();
        std::bitset<2501> temp_board = board.player_sets[player_count - 1];
        for (uint8_t i = player_count - 1; i > 0; i--)
            board.player_sets[i] = board.player_sets[i - 1];

        board.player_sets[0] = temp_board;
    }
    else if (change_stones)
    {
        std::bitset<2501> temp_board = board.player_sets[player];
        board.player_sets[player] = board.player_sets[change_stones - 1];
        board.player_sets[change_stones - 1] = temp_board;
    }
}

void MoveExecuter::exec_move(uint8_t player, Board &board)
{
    uint16_t coord = board.get_coord();
    uint8_t spec = board.get_spec();
    uint8_t change_stones = 0;
    if (!board.board_sets[1].test(coord))
        board.decrement_overwrite_stones(player);

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
    update_boards(player, change_stones, board);
}

void MoveExecuter::get_bomb_coords(uint16_t start_coord, uint16_t c, uint8_t strength, std::bitset<2501> &mask, Board &board, std::bitset<2501> &fields_to_remove)
{
    if (strength == 0 || fields_to_remove == mask)
        return;

    for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d++)
    {
        uint16_t next_coord = get_transition(c, d);
        if (next_coord != 0 && !board.board_sets[0].test(next_coord) && next_coord != start_coord)
        {
            fields_to_remove.set(next_coord);
            get_bomb_coords(start_coord, next_coord, strength - 1, mask, board, fields_to_remove);
        }
    }
}

std::bitset<2501> MoveExecuter::get_fields_to_remove(Board &board, uint16_t coord, uint8_t strength, std::bitset<2501> &mask)
{
    std::bitset<2501> fields_to_remove;
    fields_to_remove.set(coord);
    if (strength > 0)
        for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d++)
        {
            uint16_t next_coord = get_transition(coord, d);
            if (next_coord != 0 && !board.board_sets[0].test(next_coord) && next_coord != coord)
            {
                fields_to_remove.set(next_coord);
                get_bomb_coords(coord, next_coord, strength - 1, mask, board, fields_to_remove);
            }
        }
    return fields_to_remove;
}

Board MoveExecuter::exec_bomb(uint8_t player, Board board, uint8_t strength)
{
    LOG_INFO("exec");
    uint16_t coord = board.get_coord();
    std::bitset<2501> mask;
    for (uint16_t c = 1; c < m_num_of_fields; c++)
        if (!board.board_sets[0].test(c))
            mask.set(c);

    std::bitset<2501> fields_to_remove(get_fields_to_remove(board, coord, strength, mask));
    board.decrement_bombs(player);
    for (uint8_t i = 0; i < board.board_sets.size(); i++)
    {
        board.board_sets[i] |= fields_to_remove;
        board.board_sets[i] &= ~fields_to_remove;
    }
    for (uint8_t i = 0; i < board.player_sets.size(); i++)
    {
        board.player_sets[i] |= fields_to_remove;
        board.player_sets[i] &= ~fields_to_remove;
    }
    board.board_sets[0] |= fields_to_remove;
    return board;
}
