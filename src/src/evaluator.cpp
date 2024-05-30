#include "evaluator.hpp"
#include "move_board.hpp"
#include "move_generator.hpp"
#include "timer.hpp"

int get_wall_value(MoveBoard &move_board, uint8_t player_num)
{
    int value = 0;
    value += (move_board.get_player_set(player_num) & move_board.get_wall_set(0)).count() * 2;
    value += (move_board.get_player_set(player_num) & move_board.get_wall_set(1)).count() * 3;
    value += (move_board.get_player_set(player_num) & move_board.get_wall_set(2)).count() * 5;
    value += (move_board.get_player_set(player_num) & move_board.get_wall_set(3)).count() * 9;
    value += (move_board.get_player_set(player_num) & move_board.get_wall_set(4)).count() * 7;
    value += (move_board.get_player_set(player_num) & move_board.get_wall_set(5)).count() * 3;
    value += (move_board.get_player_set(player_num) & move_board.get_wall_set(6)).count() * 1;
    value += (move_board.get_player_set(player_num) & move_board.get_wall_set(7)).count() * -5;
    return value * 10;
}

int get_eliminate_player_score(MoveBoard &move_board, uint8_t player_num)
{
    int value = 0;
    for (uint8_t i = 0; i < move_board.get_player_count(); i++)
    {
        if (i != player_num && move_board.get_player_set(i).count() == 0)
            value += 10000;
    }
    return value;
}

int get_evaluation(MoveBoard &move_board, uint8_t player_num, MoveGenerator &move_gen, Timer &timer)
{
    uint16_t border_set_size = move_board.get_border_set_size();
    int score = 0;

    for (uint8_t i = 0; i < move_board.get_player_count(); i++)
    {
        if (!move_board.is_disqualified(i))
            move_gen.calculate_valid_moves(move_board, i, timer);
        else
            move_board.get_valid_moves(i).reset();
        if (i != player_num)
            score -= move_board.get_player_set(i).count() * 100;
    }
    for (uint16_t j = 0; j < border_set_size; j++)
    {
        if (j == 0)
            score += get_wall_value(move_board, player_num);
        else if (j == 1)
            score -= (border_set_size) * 10 * (move_board.get_border_set(j) & move_board.get_player_set(player_num)).count();
        else
            score += (border_set_size - j) * 10 * (move_board.get_border_set(j) & move_board.get_player_set(player_num)).count();
    }
    if (move_board.is_overwrite_move(player_num))
        score -= 100000;
    return score + move_board.get_player_set(player_num).count() * 10 + move_board.get_valid_moves(player_num).count() * 100;
}