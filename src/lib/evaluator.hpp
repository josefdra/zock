#ifndef EVALUATOR_HPP
#define EVALUATOR_HPP

#include <bitset>
#include <stdint.h>

class Board;
class MoveGenerator;
class Timer;

int get_wall_value(Board &, uint8_t);
int get_eliminate_player_score(Board &, uint8_t);
int get_evaluation(Board &, uint8_t, MoveGenerator &, Timer &);

#endif // EVALUATOR_HPP