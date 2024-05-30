#ifndef ALGORITHMS_HPP
#define ALGORITHMS_HPP

#include <stdint.h>
#include <array>
#include <bitset>
#include <vector>

#include "move_generator.hpp"
#include "move_executer.hpp"

class Timer;
class MoveBoard;

class MiniMax
{
public:
    MiniMax(MoveExecuter &, MoveGenerator &);
    ~MiniMax();

    int minimaxOrParanoidWithPruning(MoveBoard &, int, int, int8_t, uint8_t, bool, Timer &);
    std::vector<MoveBoard> generate_boards(MoveBoard &, uint8_t, Timer &);
    void init_best_board(MoveBoard &);
    MoveBoard get_best_coord(MoveBoard &, Timer &, uint8_t, bool);

private:
    MoveExecuter m_move_exec;
    MoveGenerator m_move_gen;
};

#endif // ALGORITHMS_HPP