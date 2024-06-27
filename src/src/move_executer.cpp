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
    return m_transitions[(c - 1) * NUM_OF_DIRECTIONS + d] / COORD_TO_DIR_OFFSET;
}

uint8_t MoveExecuter::get_direction(uint16_t c, uint8_t d)
{
    return m_transitions[(c - 1) * NUM_OF_DIRECTIONS + d] % COORD_TO_DIR_OFFSET;
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

void MoveExecuter::update_bits(std::bitset<MAX_NUM_OF_FIELDS> &to_color, uint8_t player, Board &board)
{
    for (auto &bitset : board.board_sets)
        bitset &= ~to_color;

    for (auto &bitset : board.player_sets)
        bitset &= ~to_color;

    board.player_sets[player] |= to_color;
}

std::bitset<MAX_NUM_OF_FIELDS> MoveExecuter::get_bits_to_update(uint8_t player, Board &board)
{
    uint16_t coord = board.get_coord();
    std::bitset<MAX_NUM_OF_FIELDS> to_color;
    to_color.set(coord);
    for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d++)
    {
        uint16_t temp_transition = get_transition(coord, d);
        uint8_t temp_direction = get_direction(coord, d);
        std::bitset<MAX_NUM_OF_FIELDS> temp;
        while (temp_transition != 0 && temp_transition != coord && !board.board_sets[EMPTY].test(temp_transition))
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

void MoveExecuter::update_communities_and_frames(std::bitset<MAX_NUM_OF_FIELDS> &to_color, Board &board)
{
    std::bitset<MAX_NUM_OF_FIELDS> temp;
    uint16_t temp_lowest_community_coord = 65000;
    uint16_t temp_highest_community_coord = 0;
    uint16_t temp_lowest_frame_coord = 65000;
    uint16_t temp_highest_frame_coord = 0;
    for (uint16_t c = 1; c < m_num_of_fields; c++)
        if (to_color.test(c))
        {
            if (c < temp_lowest_community_coord)
                temp_lowest_community_coord = c;
            if (c > temp_highest_community_coord)
                temp_highest_community_coord = c;
            for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d++)
            {
                uint16_t next_coord = get_transition(c, d);
                if (next_coord != 0 && board.board_sets[EMPTY].test(next_coord))
                {
                    if (next_coord < temp_lowest_frame_coord)
                        temp_lowest_frame_coord = next_coord;
                    if (next_coord > temp_highest_frame_coord)
                        temp_highest_frame_coord = next_coord;
                    temp.set(next_coord);
                }
            }
        }

    for (uint8_t i = 0; i < board.get_num_of_communities(); i++)
        if ((board.communities[i] & to_color).count() != 0)
        {
            if (std::get<0>(board.start_end_communities[i]) > temp_lowest_community_coord)
                std::get<0>(board.start_end_communities[i]) = temp_lowest_community_coord;
            if (std::get<1>(board.start_end_communities[i]) < temp_highest_community_coord)
                std::get<1>(board.start_end_communities[i]) = temp_highest_community_coord;
            if (std::get<0>(board.start_end_frames[i]) > temp_lowest_frame_coord)
                std::get<0>(board.start_end_frames[i]) = temp_lowest_frame_coord;
            if (std::get<1>(board.start_end_frames[i]) < temp_highest_frame_coord)
                std::get<1>(board.start_end_frames[i]) = temp_highest_frame_coord;
            board.communities[i] |= to_color;
            board.frames[i] |= temp;
            board.frames[i] &= ~board.communities[i];
        }
}

void MoveExecuter::update_players_in_communities_count(Board &board)
{
    std::vector<uint8_t> temp_num_of_players_in_communities = board.num_of_players_in_community;
    board.num_of_players_in_community.clear();
    board.num_of_players_eliminated.clear();
    board.num_of_players_eliminated.resize(board.get_num_of_communities(), 0);
    for (uint8_t i = 0; i < board.get_num_of_communities(); i++)
    {
        uint8_t count = 0;
        for (uint8_t i = 0; i < m_num_of_players; i++)
            if ((board.communities[i] & board.player_sets[i]).count() != 0)
                count++;

        board.num_of_players_in_community.push_back(count);
        if (temp_num_of_players_in_communities[i] > count)
            board.num_of_players_eliminated[i] = temp_num_of_players_in_communities[i] - count;
        else
            board.num_of_players_eliminated[i] = 0;
    }
}

void MoveExecuter::merge_communities(Board &board, uint8_t &index)
{
    bool merge = true;
    while (merge)
    {
        merge = false;
        for (uint8_t i = 0; i < board.get_num_of_communities(); i++)
            for (uint8_t j = 0; j < board.get_num_of_communities(); j++)
                if (i != j && (board.communities[i] & board.frames[j]).count() != 0)
                {
                    if (index == j)
                        index = i;
                    board.communities[i] |= board.communities[j];
                    board.communities[j].reset();
                    board.frames[i] |= board.frames[j];
                    board.frames[j].reset();
                    board.frames[i] &= ~board.communities[i];
                    if (std::get<0>(board.start_end_communities[i]) > std::get<0>(board.start_end_communities[j]))
                        std::get<0>(board.start_end_communities[i]) = std::get<0>(board.start_end_communities[j]);
                    if (std::get<1>(board.start_end_communities[i]) < std::get<1>(board.start_end_communities[j]))
                        std::get<1>(board.start_end_communities[i]) = std::get<1>(board.start_end_communities[j]);
                    if (std::get<0>(board.start_end_frames[i]) > std::get<0>(board.start_end_frames[j]))
                        std::get<0>(board.start_end_frames[i]) = std::get<0>(board.start_end_frames[j]);
                    if (std::get<1>(board.start_end_frames[i]) < std::get<1>(board.start_end_frames[j]))
                        std::get<1>(board.start_end_frames[i]) = std::get<1>(board.start_end_frames[j]);
                    merge = true;
                }

        std::vector<std::bitset<MAX_NUM_OF_FIELDS>> temp_communities;
        std::vector<std::bitset<MAX_NUM_OF_FIELDS>> temp_frames;
        std::vector<std::tuple<uint16_t, uint16_t>> temp_start_end_communities;
        std::vector<std::tuple<uint16_t, uint16_t>> temp_start_end_frames;
        uint8_t temp_index = index;
        for (uint8_t i = 0; i < board.get_num_of_communities(); i++)
            if (board.communities[i].count() != 0)
            {
                temp_communities.push_back(board.communities[i]);
                temp_frames.push_back(board.frames[i]);
                temp_start_end_communities.push_back(board.start_end_communities[i]);
                temp_start_end_frames.push_back(board.start_end_frames[i]);
            }
            else if (i < temp_index)
                index--;

        board.communities = temp_communities;
        board.frames = temp_frames;
        board.start_end_communities = temp_start_end_communities;
        board.start_end_frames = temp_start_end_frames;
        for (auto &community : board.communities)
        {
            uint8_t count = 0;
            for (uint8_t i = 0; i < m_num_of_players; i++)
                if ((community & board.player_sets[i]).count() != 0)
                    count++;

            board.num_of_players_in_community.push_back(count);
        }
    }
}

void MoveExecuter::check_if_protected_field_with_extending(Board &board, uint8_t player, uint16_t coord)
{
    uint8_t counter = 0;
    uint8_t most = 0;
    for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d++)
    {
        uint16_t next_coord = get_transition(coord, d);
        uint8_t prev_dir = d;
        d = (d + 1) % NUM_OF_DIRECTIONS;
        while ((next_coord == 0 || board.protected_fields[player].test(next_coord)) && d != prev_dir)
        {
            counter++;
            if (counter > most)
                most = counter;
            next_coord = get_transition(coord, d);
            d = (d + 1) % NUM_OF_DIRECTIONS;
        }
        counter = 0;
        d = prev_dir;
    }

    if (most > 3)
    {
        board.protected_fields[player].set(coord);
        for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d++)
        {
            uint16_t next_coord = get_transition(coord, d);
            if (next_coord != 0 && board.player_sets[player].test(next_coord) && !board.protected_fields[player].test(next_coord))
                check_if_protected_field_with_extending(board, player, next_coord);
        }
    }
}

void MoveExecuter::recalculate_protected_fields(Board &board, std::bitset<MAX_NUM_OF_FIELDS> &to_color)
{
    board.before_bonus_fields.reset();
    board.before_choice_fields.reset();
    for (uint8_t p = 0; p < m_num_of_players; p++)
        if ((board.player_sets[p] & to_color).count() != 0)
        {
            board.protected_fields[p] = board.fixed_protected_fields & board.player_sets[p];
            for (uint16_t c = 1; c < m_num_of_fields; c++)
            {
                if (board.board_sets[C].test(c))
                    calculcate_choice_and_bonus_fields(c, board.before_choice_fields);
                else if (board.board_sets[B].test(c))
                    calculcate_choice_and_bonus_fields(c, board.before_bonus_fields);

                if (board.fixed_protected_fields.test(c))
                    for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d++)
                    {
                        uint16_t next_coord = get_transition(c, d);
                        if (next_coord != 0 && board.player_sets[p].test(next_coord) && !board.protected_fields[p].test(next_coord))
                            check_if_protected_field_with_extending(board, p, next_coord);
                    }
            }
        }
}

void MoveExecuter::calculcate_choice_and_bonus_fields(uint16_t c, std::bitset<MAX_NUM_OF_FIELDS> &bitset)
{
    for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d++)
    {
        uint16_t next_coord = get_transition(c, d);
        if (next_coord != 0)
            bitset.set(next_coord);
    }
}

void MoveExecuter::extend_protected_fields(Board &board, uint8_t player, std::bitset<MAX_NUM_OF_FIELDS> &to_color)
{
    board.before_bonus_fields.reset();
    board.before_choice_fields.reset();
    board.protected_fields[player] |= board.fixed_protected_fields & board.player_sets[player];
    if ((to_color & board.protected_fields[player]).count() != 0)
        for (uint16_t c = 1; c < m_num_of_fields; c++)
        {
            if (board.board_sets[C].test(c))
                calculcate_choice_and_bonus_fields(c, board.before_choice_fields);
            else if (board.board_sets[B].test(c))
                calculcate_choice_and_bonus_fields(c, board.before_bonus_fields);

            if (to_color.test(c))
                check_if_protected_field_with_extending(board, player, c);
        }
}

void MoveExecuter::update_boards(uint8_t player, uint8_t change_stones, Board &board, uint8_t &index, bool overwrite_move)
{
    uint16_t coord = board.get_coord();
    bool inversion = false;
    if (board.board_sets[I].test(coord))
    {
        board.board_sets[EMPTY].reset(coord);
        board.board_sets[I].reset(coord);
        inversion = true;
    }
    std::bitset<MAX_NUM_OF_FIELDS> to_color = get_bits_to_update(player, board);
    update_communities_and_frames(to_color, board);
    update_bits(to_color, player, board);
    if (board.get_num_of_communities() > 1)
        merge_communities(board, index);

    update_players_in_communities_count(board);

    if (overwrite_move)
        recalculate_protected_fields(board, to_color);
    else
        extend_protected_fields(board, player, to_color);

    if (inversion)
    {
        uint16_t player_count = board.get_player_count();
        std::bitset<MAX_NUM_OF_FIELDS> temp_board = board.player_sets[player_count - 1];
        std::bitset<MAX_NUM_OF_FIELDS> temp_protected_fields = board.protected_fields[player_count - 1];
        for (uint8_t i = player_count - 1; i > 0; i--)
        {
            board.player_sets[i] = board.player_sets[i - 1];
            board.protected_fields[i] = board.protected_fields[i - 1];
        }
        board.protected_fields[0] = temp_protected_fields;
        board.player_sets[0] = temp_board;
    }
    else if (change_stones)
    {
        std::bitset<MAX_NUM_OF_FIELDS> temp_board = board.player_sets[player];
        std::bitset<MAX_NUM_OF_FIELDS> temp_protected_fields = board.protected_fields[player];
        board.player_sets[player] = board.player_sets[change_stones - 1];
        board.protected_fields[player] = board.protected_fields[change_stones - 1];
        board.player_sets[change_stones - 1] = temp_board;
        board.protected_fields[change_stones - 1] = temp_protected_fields;
    }
}

void MoveExecuter::exec_move(uint8_t player, Board &board, uint8_t &index)
{
    uint16_t coord = board.get_coord();
    uint8_t spec = board.get_spec();
    uint8_t change_stones = 0;
    bool overwrite_move = false;
    board.reset_bonus_field();
    board.reset_choice_field();
    if (!board.board_sets[EMPTY].test(coord))
    {
        board.decrement_overwrite_stones(player);
        overwrite_move = true;
    }

    if (spec == BOMB_SPEC)
    {
        board.board_sets[EMPTY].reset(coord);
        board.board_sets[B].reset(coord);
        board.increment_bombs(player);
        board.set_bonus_field();
    }
    else if (spec == OVERWRITE_SPEC)
    {
        board.board_sets[EMPTY].reset(coord);
        board.board_sets[B].reset(coord);
        board.increment_overwrite_stones(player);
        board.set_bonus_field();
    }
    else if (spec != 0)
    {
        board.board_sets[EMPTY].reset(coord);
        board.board_sets[C].reset(coord);
        change_stones = spec;
        board.set_choice_field();
    }
    update_boards(player, change_stones, board, index, overwrite_move);
}

void MoveExecuter::get_bomb_coords(uint16_t start_coord, uint16_t c, uint8_t strength, std::bitset<MAX_NUM_OF_FIELDS> &mask, Board &board, std::bitset<MAX_NUM_OF_FIELDS> &fields_to_remove)
{
    if (strength == 0 || fields_to_remove == mask)
        return;

    for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d++)
    {
        uint16_t next_coord = get_transition(c, d);
        if (next_coord != 0 && !board.board_sets[MINUS].test(next_coord) && next_coord != start_coord)
        {
            fields_to_remove.set(next_coord);
            get_bomb_coords(start_coord, next_coord, strength - 1, mask, board, fields_to_remove);
        }
    }
}

std::bitset<MAX_NUM_OF_FIELDS> MoveExecuter::get_fields_to_remove(Board &board, uint16_t coord, uint8_t strength, std::bitset<MAX_NUM_OF_FIELDS> &mask)
{
    std::bitset<MAX_NUM_OF_FIELDS> fields_to_remove;
    fields_to_remove.set(coord);
    if (strength > 0)
        for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d++)
        {
            uint16_t next_coord = get_transition(coord, d);
            if (next_coord != 0 && !board.board_sets[MINUS].test(next_coord) && next_coord != coord)
            {
                fields_to_remove.set(next_coord);
                get_bomb_coords(coord, next_coord, strength - 1, mask, board, fields_to_remove);
            }
        }
    return fields_to_remove;
}

Board MoveExecuter::exec_bomb(uint8_t player, Board board, uint8_t strength)
{
    uint16_t coord = board.get_coord();
    std::bitset<MAX_NUM_OF_FIELDS> mask;
    for (uint16_t c = 1; c < m_num_of_fields; c++)
        if (!board.board_sets[MINUS].test(c))
            mask.set(c);

    std::bitset<MAX_NUM_OF_FIELDS> fields_to_remove(get_fields_to_remove(board, coord, strength, mask));
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
    board.board_sets[MINUS] |= fields_to_remove;
    return board;
}
