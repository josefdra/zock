#ifndef MOVE_EXECUTER_HPP
#define MOVE_EXECUTER_HPP

#include <stdint.h>
#include <vector>
#include <unordered_set>
#include <bitset>

class Map;
class Board;
class Evaluator;
class Timer;

class MoveExecuter
{
public:
    MoveExecuter();
    MoveExecuter(Map &);
    ~MoveExecuter();

    uint16_t get_transition(uint16_t, uint8_t);
    uint8_t get_direction(uint16_t, uint8_t);
    uint16_t get_num_of_fields();
    uint8_t get_num_of_players();
    uint8_t get_player_num();
    void update_bits(std::bitset<2501> &, uint8_t, Board &);
    std::bitset<2501> get_bits_to_update(uint8_t, Board &);
    void update_boards(uint8_t, uint8_t, Board &);
    // void adjust_protected_fields(Board &, uint8_t);
    void exec_move(uint8_t, Board &);
    void get_bomb_coords(uint16_t, uint16_t, uint8_t, std::bitset<2501> &, Board &, std::bitset<2501> &);
    std::bitset<2501> get_fields_to_remove(Board &, uint16_t, uint8_t, std::bitset<2501> &);
    Board exec_bomb(uint8_t, Board, uint8_t);

private:
    std::vector<uint16_t> m_transitions;
    uint16_t m_num_of_fields;
    uint8_t m_num_of_players;
    uint8_t m_player_num;
};

#endif // MOVE_EXECUTER_HPP