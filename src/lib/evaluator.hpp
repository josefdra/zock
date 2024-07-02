#ifndef EVALUATOR_HPP
#define EVALUATOR_HPP

#define OVERWRITE_VALUE -100000
#define BONUS_VALUE 500
#define CHOICE_VALUE 100
#define BEFORE_BONUS_VALUE -100
#define BEFORE_CHOICE_VALUE -20
#define NUM_OF_DIRECTIONS 8
#define NO_MOVE_VALUE -100000
#define MAX_NUM_OF_FIELDS 2501
#define ENEMY_HAS_MOVE_VALUE -100
#define ENEMY_HAS_NO_MOVE_VALUE 500
#define ENEMY_STONE_VALUE -1
#define ENEMY_PROTECTED_FIELD_VALUE -2
#define MOVE_VALUE 25
#define STONE_VALUE 10
#define PROTECTED_FIELD_VALUE 25

#include <bitset>
#include <stdint.h>
#include <cmath>
#include <iomanip>

class Board;
class MoveGenerator;
class Timer;

int get_static_eval(Board &, std::bitset<MAX_NUM_OF_FIELDS> &, uint8_t);
int get_evaluation(Board &, uint8_t, Timer &, MoveGenerator &, uint8_t);

#endif // EVALUATOR_HPP