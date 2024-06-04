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

    int minimaxOrParanoidWithPruning(Board &, int, int, int8_t, uint8_t, bool, Timer &);
    int BRS(Board &, int, int, int8_t, uint8_t, Timer &, uint8_t);
    void generate_boards(Board &, uint8_t, Timer &, std::vector<Board> &);
    void init_best_board(Board &);
    Board get_best_coord(Board &, Timer &, bool);
    void sort_valid_moves(Board &, uint8_t, Timer &, bool maximizer, std::vector<Board> &);

private:
    MoveExecuter m_move_exec;
    MoveGenerator m_move_gen;
};

#endif // ALGORITHMS_HPP