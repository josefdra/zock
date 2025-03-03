#ifndef ALGORITHMS_HPP
#define ALGORITHMS_HPP

#define MEMORY_SIZE_WITH_BUFFER 3000
#define ZERO_EVALUATION 0
#define ZERO_SPEC 0
#define MAX_SEARCH_DEPTH 20
#define ESTIMATED_TIME_DIVISOR 1.5
#define AVERAGE_BRANCHING_FACTOR_DIVISOR 1.5
#define _30SECONDS 30000000

#include <stdint.h>
#include <array>
#include <bitset>
#include <vector>
#include <cmath>

#include "move_generator.hpp"
#include "move_executer.hpp"

class Timer;
class Board;

typedef std::vector<std::vector<std::tuple<int, uint16_t, uint8_t>>> moves_vector;
typedef std::vector<std::tuple<int, uint16_t, uint8_t>> moves;
typedef std::tuple<int, uint16_t, uint8_t> move;
typedef std::tuple<int, uint16_t, uint8_t> move_tuple;

class Algorithms
{
public:
    Algorithms();
    Algorithms(MoveExecuter &, MoveGenerator &);
    ~Algorithms();

    uint8_t get_next_player(uint8_t, Board &, Timer &, uint8_t &);
    int set_up_best_eval_minimax(Board &, uint8_t);
    int set_up_best_eval_brs(Board &, uint8_t &, uint8_t);
    int do_move_minimax(Board &, move &, int, int, uint8_t, Timer &, Board &, uint8_t, bool, uint8_t &);
    int do_move_brs(Board &, move &, int, int, uint8_t, uint8_t, Timer &, Board &, uint8_t, bool, uint8_t &);
    void get_eval_minimax(Board &, moves &, int, int, uint8_t, Timer &, Board &, uint8_t, int &, bool, uint8_t &);
    void get_eval_brs(Board &, moves &, int, int, uint8_t, uint8_t, Timer &, Board &, uint8_t, int &, bool, uint8_t &);
    move get_first_move(moves &);
    int minimax(Board &, int, int, uint8_t, uint8_t, Timer &, bool, uint8_t &);
    int brs(Board &, int, int, uint8_t, uint8_t, uint8_t, Timer &, bool, uint8_t &);
    void set_up_killer(moves &, uint8_t);
    void sort_valid_moves(Board &, uint8_t, moves &, Timer &, uint8_t);
    void set_up_moves(Board &, uint8_t, moves &, uint8_t);
    void init_best_board(Board &);
    void sort_best_moves_and_communities_to_front(Board &, std::vector<uint16_t> &, uint8_t &, moves_vector &);
    Board get_best_coord(Board &, Timer &, bool);
    void calculate_average_branching_factor();
    double estimate_runtime_next_depth(uint8_t &, Timer &);
    void adapt_depth_to_map_progress(uint8_t &, Board &);
    void calculate_thresholds(double);

    std::vector<std::vector<uint16_t>> killer_moves;

private:
    MoveExecuter move_exec;
    MoveGenerator m_move_gen;

    double average_branching_factor = 0;
    uint32_t total_nodes = 0;
    uint32_t total_valid_moves = 0;

    // thresholds for small maps
    uint8_t small_map_threshold_60 = 60;
    uint8_t small_map_threshold_40 = 40;
    uint8_t small_map_threshold_25 = 25;
    uint8_t small_map_threshold_15 = 15;

    // thresholds for large maps
    uint8_t large_map_threshold_80 = 80;
    uint8_t large_map_threshold_60 = 60;
    uint8_t large_map_threshold_45 = 45;
    uint8_t large_map_threshold_30 = 30;

    uint8_t threshold_60_80 = 0;
    uint8_t threshold_40_60 = 0;
    uint8_t threshold_25_45 = 0;
    uint8_t threshold_15_30 = 0;
};

#endif // ALGORITHMS_HPP