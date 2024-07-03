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
#define ENEMY_HAS_MOVE_VALUE -200
#define ENEMY_HAS_NO_MOVE_VALUE 1000
#define ENEMY_STONE_VALUE -3
#define ENEMY_PROTECTED_FIELD_VALUE -10
#define MOVE_VALUE 20
#define STONE_VALUE 10
#define PROTECTED_FIELD_VALUE 30
#define WINNER_VALUE 1000
#define NOT_WINNER_VALUE -200
#define BEFORE_OUR_PROTECTED_FIELDS_VALUE 50
#define BEFORE_ENEMY_PROTECTED_FIELDS_VALUE -25
#define BLOCKED_ENEMIES_PROTECTED_FIELDS_VALUE 50

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