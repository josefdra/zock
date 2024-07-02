#include "evaluator.hpp"
#include "board.hpp"
#include "move_generator.hpp"
#include "timer.hpp"
#include "statistics.hpp"

int get_static_eval(Board &board, std::bitset<MAX_NUM_OF_FIELDS> &community_player_stones, uint8_t index)
{
    int return_value = 0;
    for (uint16_t c = std::get<0>(board.start_end_communities[index]); c <= std::get<1>(board.start_end_communities[index]); c++)
        if (community_player_stones.test(c))
            return_value += board.static_evaluation[c];

    return return_value;
}

int get_evaluation(Board &board, uint8_t player_num, Timer &timer, MoveGenerator &move_gen, uint8_t index)
{
    Timer evaluation_time;
    leafs_calculated++;

    uint8_t end_game_multiplier = 1;

    if (board.get_num_of_not_minus_fields() / 4 > board.board_sets[EMPTY].count())
        end_game_multiplier = 100;

    try
    {
        for (uint8_t i = 0; i < board.get_player_count(); i++)
        {
            if (timer.return_rest_time() < timer.exception_time)
                throw TimeLimitExceededException("Timeout in evaluation");

            std::bitset<MAX_NUM_OF_FIELDS> player_stones_in_community = board.communities[index] & board.player_sets[i];

            if (player_stones_in_community.count() == 0 || board.disqualified[i])
                continue;

            if (i != player_num)
            {
                if (move_gen.check_if_player_has_no_overwrite_move(board, i, index))
                    enemy_move_value += ENEMY_HAS_MOVE_VALUE;
                else 
                    enemy_move_value += ENEMY_HAS_NO_MOVE_VALUE;

                enemy_stone_value += player_stones_in_community.count() * ENEMY_STONE_MULTIPLIER * end_game_multiplier;
                enemy_protected_fields_value += board.protected_fields[i].count() * ENEMY_PROTECTED_FIELD_MULTIPLIER;
                enemy_static_eval -= get_static_eval(board, player_stones_in_community, index);                
            }
            else
            {
                our_move_value += board.valid_moves[i][index].count() * MOVE_MULTIPLIER;
                our_stone_value += player_stones_in_community.count() * STONE_MULTIPLIER * end_game_multiplier;
                our_protected_fields_value += board.protected_fields[i].count() * PROTECTED_FIELD_MULTIPLIER;
                our_static_eval += get_static_eval(board, player_stones_in_community, index);                
            }
        }

        score += enemy_move_value + enemy_stone_value + enemy_protected_fields_value + enemy_static_eval;
        score += our_move_value + our_stone_value + our_protected_fields_value + our_static_eval;

        before_bonus_value = (board.before_bonus_fields & board.player_sets[player_num]).count() * BEFORE_BONUS_VALUE;
        before_choice_value = (board.before_choice_fields & board.player_sets[player_num]).count() * BEFORE_CHOICE_VALUE;

        score += before_bonus_value + before_choice_value;

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
