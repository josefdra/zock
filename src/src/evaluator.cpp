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

int get_before_wall_value(Board &board, uint8_t player_num)
{
    int value = 0;
    value += (board.player_sets[player_num] & board.before_wall_sets[ONE_WALL]).count() * ONE_WALL_VALUE;
    value += (board.player_sets[player_num] & board.before_wall_sets[TWO_WALLS]).count() * TWO_WALLS_VALUE;
    value += (board.player_sets[player_num] & board.before_wall_sets[THREE_WALLS]).count() * THREE_WALLS_VALUE;
    value += (board.player_sets[player_num] & board.before_wall_sets[FOUR_WALLS]).count() * FOUR_WALLS_VALUE;
    value += (board.player_sets[player_num] & board.before_wall_sets[FIVE_WALLS]).count() * FIVE_WALLS_VALUE;
    value += (board.player_sets[player_num] & board.before_wall_sets[SIX_WALLS]).count() * SIX_WALLS_VALUE;
    value += (board.player_sets[player_num] & board.before_wall_sets[SEVEN_WALLS]).count() * SEVEN_WALLS_VALUE;
    value += (board.player_sets[player_num] & board.before_wall_sets[EIGHT_WALLS]).count() * EIGHT_WALLS_VALUE;
    return value * BEFORE_WALL_MULTIPLIER;
}

int get_before_before_wall_value(Board &board, uint8_t player_num)
{
    int value = 0;
    value += (board.player_sets[player_num] & board.before_before_wall_sets[ONE_WALL]).count() * ONE_WALL_VALUE;
    value += (board.player_sets[player_num] & board.before_before_wall_sets[TWO_WALLS]).count() * TWO_WALLS_VALUE;
    value += (board.player_sets[player_num] & board.before_before_wall_sets[THREE_WALLS]).count() * THREE_WALLS_VALUE;
    value += (board.player_sets[player_num] & board.before_before_wall_sets[FOUR_WALLS]).count() * FOUR_WALLS_VALUE;
    value += (board.player_sets[player_num] & board.before_before_wall_sets[FIVE_WALLS]).count() * FIVE_WALLS_VALUE;
    value += (board.player_sets[player_num] & board.before_before_wall_sets[SIX_WALLS]).count() * SIX_WALLS_VALUE;
    value += (board.player_sets[player_num] & board.before_before_wall_sets[SEVEN_WALLS]).count() * SEVEN_WALLS_VALUE;
    value += (board.player_sets[player_num] & board.before_before_wall_sets[EIGHT_WALLS]).count() * EIGHT_WALLS_VALUE;
    return value * BEFORE_BEFORE_WALL_MULTIPLIER;
}

void adjust_wall_values(Board &board)
{
    std::bitset<MAX_NUM_OF_FIELDS> all_stones = board.board_sets[X];
    for (uint8_t i = 0; i < board.get_player_count(); i++)
        all_stones |= board.player_sets[i];

    for (uint8_t i = 0; i < NUM_OF_WALL_SETS; i++)
    {
        board.before_wall_sets[i] &= all_stones;
        board.before_before_wall_sets[i] &= all_stones;
    }
}

void print_static_evaluation(Board &board)
{
    for (uint16_t c = 1; c < board.get_num_of_fields(); c++)
    {
        int field_score = 0;
        if (board.wall_sets[ONE_WALL].test(c))
            field_score += ONE_WALL_VALUE * WALL_MULTIPLIER;
        else if (board.wall_sets[TWO_WALLS].test(c))
            field_score += TWO_WALLS_VALUE * WALL_MULTIPLIER;
        else if (board.wall_sets[THREE_WALLS].test(c))
            field_score += THREE_WALLS_VALUE * WALL_MULTIPLIER;
        else if (board.wall_sets[FOUR_WALLS].test(c))
            field_score += FOUR_WALLS_VALUE * WALL_MULTIPLIER;
        else if (board.wall_sets[FIVE_WALLS].test(c))
            field_score += FIVE_WALLS_VALUE * WALL_MULTIPLIER;
        else if (board.wall_sets[SIX_WALLS].test(c))
            field_score += SIX_WALLS_VALUE * WALL_MULTIPLIER;
        else if (board.wall_sets[SEVEN_WALLS].test(c))
            field_score += SEVEN_WALLS_VALUE * WALL_MULTIPLIER;
        else if (board.wall_sets[EIGHT_WALLS].test(c))
            field_score += EIGHT_WALLS_VALUE * WALL_MULTIPLIER;
        if (board.before_wall_sets[ONE_WALL].test(c))
            field_score += ONE_WALL_VALUE * BEFORE_WALL_MULTIPLIER;
        if (board.before_wall_sets[TWO_WALLS].test(c))
            field_score += TWO_WALLS_VALUE * BEFORE_WALL_MULTIPLIER;
        if (board.before_wall_sets[THREE_WALLS].test(c))
            field_score += THREE_WALLS_VALUE * BEFORE_WALL_MULTIPLIER;
        if (board.before_wall_sets[FOUR_WALLS].test(c))
            field_score += FOUR_WALLS_VALUE * BEFORE_WALL_MULTIPLIER;
        if (board.before_wall_sets[FIVE_WALLS].test(c))
            field_score += FIVE_WALLS_VALUE * BEFORE_WALL_MULTIPLIER;
        if (board.before_wall_sets[SIX_WALLS].test(c))
            field_score += SIX_WALLS_VALUE * BEFORE_WALL_MULTIPLIER;
        if (board.before_wall_sets[SEVEN_WALLS].test(c))
            field_score += SEVEN_WALLS_VALUE * BEFORE_WALL_MULTIPLIER;
        if (board.before_wall_sets[EIGHT_WALLS].test(c))
            field_score += EIGHT_WALLS_VALUE * BEFORE_WALL_MULTIPLIER;
        if (board.before_before_wall_sets[ONE_WALL].test(c))
            field_score += ONE_WALL_VALUE * BEFORE_BEFORE_WALL_MULTIPLIER;
        if (board.before_before_wall_sets[TWO_WALLS].test(c))
            field_score += TWO_WALLS_VALUE * BEFORE_BEFORE_WALL_MULTIPLIER;
        if (board.before_before_wall_sets[THREE_WALLS].test(c))
            field_score += THREE_WALLS_VALUE * BEFORE_BEFORE_WALL_MULTIPLIER;
        if (board.before_before_wall_sets[FOUR_WALLS].test(c))
            field_score += FOUR_WALLS_VALUE * BEFORE_BEFORE_WALL_MULTIPLIER;
        if (board.before_before_wall_sets[FIVE_WALLS].test(c))
            field_score += FIVE_WALLS_VALUE * BEFORE_BEFORE_WALL_MULTIPLIER;
        if (board.before_before_wall_sets[SIX_WALLS].test(c))
            field_score += SIX_WALLS_VALUE * BEFORE_BEFORE_WALL_MULTIPLIER;
        if (board.before_before_wall_sets[SEVEN_WALLS].test(c))
            field_score += SEVEN_WALLS_VALUE * BEFORE_BEFORE_WALL_MULTIPLIER;
        if (board.before_before_wall_sets[EIGHT_WALLS].test(c))
            field_score += EIGHT_WALLS_VALUE * BEFORE_BEFORE_WALL_MULTIPLIER;

        std::cout << std::setw(4) << field_score << " ";
        if (c % board.get_width() == 0)
            std::cout << std::endl;
    }
}

int get_evaluation(Board &board, uint8_t player_num, Timer &timer, MoveGenerator &move_gen)
{
    // print_static_evaluation(board);
    // exit(0);
    uint8_t end_game_multiplier = 1;

    if (sqrt(board.get_num_of_fields()) > board.board_sets[EMPTY].count())
        end_game_multiplier = 10;

    try
    {
        int score = 0;

        for (uint8_t i = 0; i < board.get_player_count(); i++)
        {
            if (timer.return_rest_time() < timer.exception_time)
                throw TimeLimitExceededException("Timeout in evaluation");

            if (board.disqualified[i])
                board.reset_valid_moves(i);

            if (i != player_num)
                score -= board.get_total_moves(i).count() * ENEMY_MOVE_MULTIPLIER + board.player_sets[i].count() * ENEMY_STONE_MULTIPLIER * end_game_multiplier + board.protected_fields[i].count() * ENEMY_PROTECTED_FIELD_MULTIPLIER;
            else
                score += board.get_total_moves(player_num).count() * MOVE_MULTIPLIER + board.player_sets[player_num].count() * STONE_MULTIPLIER * end_game_multiplier + board.protected_fields[i].count() * PROTECTED_FIELD_MULTIPLIER;
        }

        score += get_wall_value(board, player_num);
        score += get_before_wall_value(board, player_num);
        score += get_before_before_wall_value(board, player_num);

        adjust_wall_values(board);

        score -= (board.before_bonus_fields & board.player_sets[player_num]).count() * BONUS_VALUE;
        score -= (board.before_choice_fields & board.player_sets[player_num]).count() * CHOICE_VALUE;

        if (board.is_overwrite_move(player_num))
            score -= OVERWRITE_VALUE;
        else if (board.check_bonus_field())
            score += BONUS_VALUE;
        else if (board.check_choice_field())
            score += CHOICE_VALUE;
        return score;
    }
    catch (const TimeLimitExceededException &)
    {
        throw;
    }
}
