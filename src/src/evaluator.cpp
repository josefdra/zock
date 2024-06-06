#include "evaluator.hpp"
#include "board.hpp"
#include "move_generator.hpp"
#include "timer.hpp"

int get_wall_value(Board &board, uint8_t player_num)
{
    int value = 0;
    value += (board.player_sets[player_num] & board.wall_sets[0]).count() * 2;
    value += (board.player_sets[player_num] & board.wall_sets[1]).count() * 3;
    value += (board.player_sets[player_num] & board.wall_sets[2]).count() * 5;
    value += (board.player_sets[player_num] & board.wall_sets[3]).count() * 9;
    value += (board.player_sets[player_num] & board.wall_sets[4]).count() * 7;
    value += (board.player_sets[player_num] & board.wall_sets[5]).count() * 3;
    value += (board.player_sets[player_num] & board.wall_sets[6]).count() * 1;
    value += (board.player_sets[player_num] & board.wall_sets[7]).count() * -5;
    return value * 50;
}

int get_eliminate_player_score(Board &board, uint8_t player_num)
{
    int value = 0;
    for (uint8_t i = 0; i < board.get_player_count(); i++)
    {
        if (i != player_num && board.player_sets[i].count() == 0)
            value += 10000;
    }
    return value;
}

int get_evaluation(Board &board, uint8_t player_num, MoveGenerator &move_gen, Timer &timer)
{
    try
    {
        uint16_t border_set_size = board.border_sets.size();
        int score = 0;

        for (uint8_t i = 0; i < board.get_player_count(); i++)
        {
            if (timer.return_rest_time() < timer.exception_time)
            {
                throw TimeLimitExceededException("Timeout in evaluation");
            }
            if (!board.disqualified[i])
                move_gen.calculate_valid_moves(board, i, timer);
            else
                board.valid_moves[i].reset();
            if (i != player_num)
                score -= board.valid_moves[i].count() * 50;
        }
        for (uint16_t j = 0; j < border_set_size; j++)
        {
            if (j == 0)
                score += get_wall_value(board, player_num);
            else if (j == 1)
                score -= (border_set_size) * 50 * (board.border_sets[j] & board.player_sets[player_num]).count();
            else
                score += (border_set_size - j) * 10 * (board.border_sets[j] & board.player_sets[player_num]).count();
        }
        if (board.is_overwrite_move(player_num))
            score -= 100000;
        return score + board.valid_moves[player_num].count() * 100;
    }
    catch (TimeLimitExceededException)
    {
        throw;
    }
}