#ifndef ALGORITHMS_HPP
#define ALGORITHMS_HPP

#include <stdint.h>
#include <array>
#include <bitset>
#include <vector>

#include "move_generator.hpp"
#include "move_executer.hpp"

class Timer;
class Board;

typedef std::vector<std::tuple<int, uint16_t, uint8_t>> moves;
typedef std::tuple<int, uint16_t, uint8_t> move;

class Algorithms
{
public:
    Algorithms(MoveExecuter &, MoveGenerator &);
    ~Algorithms();

    uint8_t get_next_player(uint8_t, Board &);
    int set_up_best_eval(uint8_t &, uint8_t);
    int do_move(Board &, move &, int, int, uint8_t, uint8_t, Timer &, Board &, uint8_t, bool);
    void get_eval(Board &, moves &, int, int, uint8_t, uint8_t, Timer &, Board &, uint8_t, int &, bool);
    int brs(Board &, int, int, uint8_t, uint8_t, uint8_t, Timer &, bool);
    void sort_valid_moves(Board &, uint8_t, moves &, Timer &);
    void set_up_moves(Board &, uint8_t, moves &);
    void init_best_board(Board &);
    Board get_best_coord(Board &, Timer &, bool);    

private:
    MoveExecuter m_move_exec;
    MoveGenerator m_move_gen;
};

#endif // ALGORITHMS_HPP