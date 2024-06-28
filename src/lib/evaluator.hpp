#ifndef EVALUATOR_HPP
#define EVALUATOR_HPP

#define MOVE_MULTIPLIER 300
#define STONE_MULTIPLIER 60
#define ENEMY_MOVE_MULTIPLIER 100
#define ENEMY_STONE_MULTIPLIER 20
#define ELIMINATE_PLAYER_VALUE 100000
#define OVERWRITE_VALUE 10000000
#define ENEMY_PROTECTED_FIELD_MULTIPLIER 100
#define PROTECTED_FIELD_MULTIPLIER 300
#define BONUS_VALUE 10000
#define CHOICE_VALUE 2000
#define MAX_NUM_OF_DIRECTIONS 8
#define NO_MOVE_VALUE 100000
#define MAX_NUM_OF_FIELDS 2501

#include <bitset>
#include <stdint.h>
#include <cmath>
#include <iomanip>

class Board;
class MoveGenerator;
class Timer;

int get_static_eval(Board &, std::bitset<MAX_NUM_OF_FIELDS> &, uint8_t);
void print_static_evaluation(Board &);
int get_evaluation(Board &, uint8_t, Timer &, MoveGenerator &, uint8_t);

#endif // EVALUATOR_HPP