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

class Algorithms
{
public:
    Algorithms(MoveExecuter &, MoveGenerator &);
    ~Algorithms();

    uint8_t get_next_player(uint8_t, Board &, Timer &);
    int minimaxOrParanoidWithPruning(Board &, int, int, int8_t, uint8_t, bool, Timer &);
    void sort_valid_moves(Board &, uint8_t, std::vector<std::tuple<int, uint16_t, uint8_t>> &, Timer &);
    void set_up_moves(Board &, uint8_t, std::vector<std::tuple<int, uint16_t, uint8_t>> &);
    void init_best_board(Board &);
    Board get_best_coord(Board &, Timer &, bool);    

private:
    MoveExecuter m_move_exec;
    MoveGenerator m_move_gen;
};

#endif // ALGORITHMS_HPP