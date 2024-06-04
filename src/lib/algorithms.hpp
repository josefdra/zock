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
    std::vector<std::bitset<2501>> sort_valid_moves(Board &, uint8_t, Timer &, std::vector<Board> &, bool maximizer);
    double calculate_average_branching_factor(uint16_t, uint32_t, bool);
    double estimate_runtime_next_depth(uint8_t &, Timer &);
    uint32_t track_number_of_nodes(bool);

private:
    MoveExecuter m_move_exec;
    MoveGenerator m_move_gen;
    double average_branching_factor = 0;
};

#endif // ALGORITHMS_HPP