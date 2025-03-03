#include "evaluator.hpp"
#include "board.hpp"
#include "move_generator.hpp"
#include "timer.hpp"
#include "statistics.hpp"

/// @brief calculates a static evaluation not dependant on any move by any player
/// @param board current board layout
/// @param community_player_stones stones in a community by a player
/// @param index community index
/// @return calculated static evaluation
int get_static_eval(Board &board, std::bitset<MAX_NUM_OF_FIELDS> &community_player_stones, uint8_t index)
{
    int return_value = 0;
    for (uint16_t c = std::get<0>(board.start_end_communities[index]); c <= std::get<1>(board.start_end_communities[index]); c++)
        if (community_player_stones.test(c))
            return_value += board.static_evaluation[c];

    return return_value;
}

/// @brief calculates current situation of board for us + our move
/// @param board current board layout
/// @param player_num current player that gets evaluated
/// @param timer timer object to check if time is up
/// @param move_gen movegenerator object
/// @param index community index
/// @return evaluation of current situation for the board
int get_evaluation(Board &board, uint8_t player_num, Timer &timer, MoveGenerator &move_gen, uint8_t index)
{
    Timer evaluation_time;
    leafs_calculated++;

    uint8_t end_game_multiplier = 1;

    if (sqrt(board.get_num_of_fields()) > board.board_sets[EMPTY].count())
        end_game_multiplier = 10;

    try
    {
        for (uint8_t i = 0; i < board.get_player_count(); i++)
        {
            if (timer.return_rest_time() < timer.exception_time)
                throw TimeLimitExceededException("Timeout in evaluation");

            std::bitset<MAX_NUM_OF_FIELDS> player_stones_in_community = board.communities[index] & board.player_sets[i];

            uint16_t count_of_player_stones = player_stones_in_community.count();
            if (i == player_num && count_of_player_stones == 0)
                continue;

            if (i != player_num)
            {
                if (move_gen.check_if_player_has_no_overwrite_move(board, i, index))
                    enemy_move_value += ENEMY_HAS_MOVE_VALUE;
                else
                    enemy_move_value += ENEMY_HAS_NO_MOVE_VALUE;

                enemy_stone_value += player_stones_in_community.count() * ENEMY_STONE_VALUE * end_game_multiplier;
                enemy_protected_fields_value += board.protected_fields[i].count() * ENEMY_PROTECTED_FIELD_VALUE;
                enemy_static_eval -= get_static_eval(board, player_stones_in_community, index);
            }
            else
            {
                our_move_value += board.valid_moves[i][index].count() * MOVE_VALUE;
                our_stone_value += player_stones_in_community.count() * STONE_VALUE * end_game_multiplier;
                our_protected_fields_value += board.protected_fields[i].count() * PROTECTED_FIELD_VALUE;
                our_static_eval += get_static_eval(board, player_stones_in_community, index);
            }
        }

        score += enemy_move_value + enemy_stone_value + enemy_protected_fields_value + enemy_static_eval;
        score += our_move_value + our_stone_value + our_protected_fields_value + our_static_eval;

        before_bonus_value = (board.before_bonus_fields & board.player_sets[player_num]).count() * BEFORE_BONUS_VALUE;
        before_choice_value = (board.before_choice_fields & board.player_sets[player_num]).count() * BEFORE_CHOICE_VALUE;

        score += before_bonus_value + before_choice_value;

        uint16_t coord = board.get_coord();
        for (uint8_t p = 0; p < board.get_player_count(); p++)
            if (board.before_protected_fields[p].test(coord))
            {
                if (p != player_num)
                {
                    score -= BEFORE_ENEMY_PROTECTED_FIELDS_VALUE;
                    if (board.corners_and_walls.test(coord))
                        for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d++)
                        {
                            uint16_t next_coord = move_gen.next_coords[(coord - 1) * NUM_OF_DIRECTIONS + d].size() > 0 ? move_gen.next_coords[(coord - 1) * NUM_OF_DIRECTIONS + d][0] : 0;
                            if (next_coord != 0 && board.player_sets[p].test(next_coord))
                            {
                                uint16_t other_direction_next_coord = move_gen.next_coords[(coord - 1) * NUM_OF_DIRECTIONS + d].size() > 0 ? move_gen.next_coords[(coord - 1) * NUM_OF_DIRECTIONS + (d + 4) % NUM_OF_DIRECTIONS][0] : 0;
                                if (other_direction_next_coord != 0 && board.player_sets[p].test(other_direction_next_coord))
                                    score += BLOCKED_ENEMIES_PROTECTED_FIELDS_VALUE;
                            }
                        }
                }
                else
                    score += BEFORE_OUR_PROTECTED_FIELDS_VALUE;
            }

        uint16_t max = 0;
        uint8_t winner = 0;

        for (uint8_t i = 0; i < board.get_player_count(); i++)
        {
            if (board.disqualified[i])
                continue;

            else
            {
                if (board.player_sets[i].count() >= max)
                {
                    max = board.player_sets[i].count();
                    winner = i;
                }
            }
        }

        if (winner == player_num)
            score += WINNER_VALUE * end_game_multiplier;
        else
            score += NOT_WINNER_VALUE * end_game_multiplier;

        if (board.is_overwrite_move(player_num))
            overwrite_value += OVERWRITE_VALUE;

        if (board.valid_moves[player_num][index].count() == 0)
            no_move_value += NO_MOVE_VALUE;
        else if (board.check_bonus_field())
            bonus_value += BONUS_VALUE;
        else if (board.check_choice_field())
            choice_value += CHOICE_VALUE;

        score += overwrite_value + no_move_value + bonus_value + choice_value;

        average_evaluation_time += evaluation_time.get_elapsed_time();
        int return_value = score;
        adjust_evaluation_values();
        ajdust_time_values();
        return return_value;
    }
    catch (const TimeLimitExceededException &)
    {
        throw;
    }
}
