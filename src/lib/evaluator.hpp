#ifndef EVALUATOR_HPP
#define EVALUATOR_HPP

#include <bitset>
#include <stdint.h>

class MoveBoard;
class MoveGenerator;
class Timer;

int get_wall_value(MoveBoard &, uint8_t);
int get_eliminate_player_score(MoveBoard &, uint8_t);
int get_evaluation(MoveBoard &, uint8_t, MoveGenerator &, Timer &);

#endif // EVALUATOR_HPP