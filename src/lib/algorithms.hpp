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

class MiniMax
{
public:
    MiniMax(MoveExecuter &, MoveGenerator &);
    ~MiniMax();

    int minimaxOrParanoidWithPruning(Board &, int, int, int8_t, uint8_t, bool, Timer &);
    std::vector<Board> generate_boards(Board &, uint8_t, Timer &);
    void init_best_board(Board &);
    Board get_best_coord(Board &, Timer &, bool);
    std::vector<std::bitset<2501>> sort_valid_moves(Board &, uint8_t, Timer &, bool maximizer);

private:
    MoveExecuter m_move_exec;
    MoveGenerator m_move_gen;
};

#endif // ALGORITHMS_HPP