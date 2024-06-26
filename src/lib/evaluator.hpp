#ifndef EVALUATOR_HPP
#define EVALUATOR_HPP

#define ONE_WALL_VALUE 2
#define TWO_WALLS_VALUE 3
#define THREE_WALLS_VALUE 5
#define FOUR_WALLS_VALUE 9
#define FIVE_WALLS_VALUE 7
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
#define WALL_MULTIPLIER 500
#define MOVE_MULTIPLIER 100
#define STONE_MULTIPLIER 50
#define ENEMY_MOVE_MULTIPLIER 50
#define ENEMY_STONE_MULTIPLIER 25
#define ELIMINATE_PLAYER_VALUE 10000
#define BEFORE_WALL_MULTIPLIER 100
#define BEFORE_BEFORE_WALL_MULTIPLIER 100
#define OVERWRITE_VALUE 1000000
#define ENEMY_PROTECTED_FIELD_MULTIPLIER 100
#define PROTECTED_FIELD_MULTIPLIER 500
#define BONUS_VALUE 5000
#define CHOICE_VALUE 1000

#include <bitset>
#include <stdint.h>
#include <cmath>

class Board;
class MoveGenerator;
class Timer;

int get_wall_value(Board &, uint8_t);
int get_eliminate_player_score(Board &, uint8_t);
int get_evaluation(Board &, uint8_t, Timer &);

#endif // EVALUATOR_HPP