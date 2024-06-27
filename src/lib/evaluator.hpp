#ifndef EVALUATOR_HPP
#define EVALUATOR_HPP

#define ONE_WALL_VALUE 2
#define TWO_WALLS_VALUE 2
#define THREE_WALLS_VALUE 4
#define FOUR_WALLS_VALUE 17
#define FIVE_WALLS_VALUE 14
#define SIX_WALLS_VALUE 3
#define SEVEN_WALLS_VALUE 1
#define EIGHT_WALLS_VALUE -5
#define ONE_WALL 0
#define TWO_WALLS 1
#define THREE_WALLS 2
#define FOUR_WALLS 3
#define FIVE_WALLS 4
#define SIX_WALLS 5
#define SEVEN_WALLS 6
#define EIGHT_WALLS 7
#define WALL_MULTIPLIER 50
#define BEFORE_WALL_MULTIPLIER -30
#define BEFORE_BEFORE_WALL_MULTIPLIER 25
#define MOVE_MULTIPLIER 10
#define STONE_MULTIPLIER 5
#define ENEMY_MOVE_MULTIPLIER 5
#define ENEMY_STONE_MULTIPLIER 2
#define ELIMINATE_PLAYER_VALUE 10000
#define ENEMY_NO_MOVE_VALUE 10000
#define NO_MOVE_VALUE 100000
#define OVERWRITE_VALUE 100000
#define ENEMY_PROTECTED_FIELD_MULTIPLIER 10
#define PROTECTED_FIELD_MULTIPLIER 50
#define BONUS_VALUE 2000
#define CHOICE_VALUE 200
#define MAX_NUM_OF_DIRECTIONS 8
#define BETWEEN_PLAYERS_VALUE 50

#include <bitset>
#include <stdint.h>
#include <cmath>
#include <iomanip>

class Board;
class MoveGenerator;
class Timer;

int get_wall_value(Board &, uint8_t);
int get_before_wall_value(Board &, uint8_t);
int get_before_before_wall_value(Board &, uint8_t);
void adjust_wall_values(Board &);
void print_static_evaluation(Board &);
int get_evaluation(Board &, uint8_t, Timer &, uint8_t, MoveGenerator &);

#endif // EVALUATOR_HPP