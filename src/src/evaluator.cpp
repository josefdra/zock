#include "evaluator.hpp"
#include "board.hpp"
#include "move_generator.hpp"
#include "timer.hpp"

int get_wall_value(Board &board, uint8_t player_num)
{
    int value = 0;
    value += (board.player_sets[player_num] & board.wall_sets[ONE_WALL]).count() * ONE_WALL_VALUE;
    value += (board.player_sets[player_num] & board.wall_sets[TWO_WALLS]).count() * TWO_WALLS_VALUE;
    value += (board.player_sets[player_num] & board.wall_sets[THREE_WALLS]).count() * THREE_WALLS_VALUE;
    value += (board.player_sets[player_num] & board.wall_sets[FOUR_WALLS]).count() * FOUR_WALLS_VALUE;
    value += (board.player_sets[player_num] & board.wall_sets[FIVE_WALLS]).count() * FIVE_WALLS_VALUE;
    value += (board.player_sets[player_num] & board.wall_sets[SIX_WALLS]).count() * SIX_WALLS_VALUE;
    value += (board.player_sets[player_num] & board.wall_sets[SEVEN_WALLS]).count() * SEVEN_WALLS_VALUE;
    value += (board.player_sets[player_num] & board.wall_sets[EIGHT_WALLS]).count() * EIGHT_WALLS_VALUE;
    return value * WALL_MULTIPLIER;
}

int get_eliminate_player_score(Board &board, uint8_t player_num)
{
    int value = 0;
    for (uint8_t i = 0; i < board.get_player_count(); i++)
        if (i != player_num && board.player_sets[i].count() == 0)
            value += ELIMINATE_PLAYER_VALUE;
    return value;
}

int get_evaluation(Board &board, uint8_t player_num, Timer &timer)
{
    uint8_t end_game_multiplier = 1;

    if (sqrt(board.get_num_of_fields()) > board.board_sets[EMPTY].count())
        end_game_multiplier = 10;

    try
    {
        uint16_t border_set_size = board.border_sets.size();
        int score = 0;

        for (uint8_t i = 0; i < board.get_player_count(); i++)
        {
            if (timer.return_rest_time() < timer.exception_time)
                throw TimeLimitExceededException("Timeout in evaluation");

            if (board.disqualified[i])
                board.reset_valid_moves(i);

            if (i != player_num)
                score -= board.get_total_moves(i).count() * ENEMY_MOVE_MULTIPLIER + board.player_sets[i].count() * ENEMY_STONE_MULTIPLIER * end_game_multiplier;
            else
                score += board.get_total_moves(player_num).count() * MOVE_MULTIPLIER + board.player_sets[player_num].count() * STONE_MULTIPLIER * end_game_multiplier;
        }
        for (uint16_t j = 0; j < border_set_size; j++)
            if (j == 0)
                score += get_wall_value(board, player_num);

            else if (j == 1)
                score -= border_set_size * BEFORE_WALL_MULTIPLIER * (board.border_sets[j] & board.player_sets[player_num]).count();

            else
                score += (border_set_size - j) * NORMAL_FIELD_MULTIPLIER * (board.border_sets[j] & board.player_sets[player_num]).count();

        if (board.is_overwrite_move(player_num))
            score -= OVERWRITE_VALUE;
        return score;
    }
    catch (const TimeLimitExceededException &)
    {
        throw;
    }
}