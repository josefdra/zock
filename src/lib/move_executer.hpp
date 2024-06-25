#ifndef MOVE_EXECUTER_HPP
#define MOVE_EXECUTER_HPP

#define MAX_NUM_OF_FIELDS 2501
#define BOMB_SPEC 20
#define OVERWRITE_SPEC 21

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
    void update_bits(std::bitset<MAX_NUM_OF_FIELDS> &, uint8_t, Board &);
    std::bitset<MAX_NUM_OF_FIELDS> get_bits_to_update(uint8_t, Board &);
    void update_communities_and_frames(std::bitset<MAX_NUM_OF_FIELDS> &, Board &);
    void update_players_in_communities_count(Board &);
    void merge_communities(Board &, uint8_t &);
    void check_if_protected_field_with_extending(Board &, uint8_t, uint16_t);    
    void expand_protected_fields(Board &, uint8_t);
    void recalculate_protected_fields(Board &, std::bitset<MAX_NUM_OF_FIELDS> &);
    void extend_protected_fields(Board &, uint8_t, std::bitset<MAX_NUM_OF_FIELDS> &);
    void update_boards(uint8_t, uint8_t, Board &, uint8_t &, bool);
    void exec_move(uint8_t, Board &, uint8_t &);
    void get_bomb_coords(uint16_t, uint16_t, uint8_t, std::bitset<MAX_NUM_OF_FIELDS> &, Board &, std::bitset<MAX_NUM_OF_FIELDS> &);
    std::bitset<MAX_NUM_OF_FIELDS> get_fields_to_remove(Board &, uint16_t, uint8_t, std::bitset<MAX_NUM_OF_FIELDS> &);
    Board exec_bomb(uint8_t, Board, uint8_t);

private:
    std::vector<uint16_t> m_transitions;
    uint16_t m_num_of_fields;
    uint8_t m_num_of_players;
    uint8_t m_player_num;
};

#endif // MOVE_EXECUTER_HPP