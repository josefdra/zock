#ifndef MOVE_GENERATOR_HPP
#define MOVE_GENERATOR_HPP

#define BYTE 8
#define TWO_BYTES 16

#include <stdint.h>
#include <vector>
#include <chrono>
#include <iostream>
#include <bitset>
#include <tuple>
#include <algorithm>

class Initializer;
class Board;
class Timer;
class Algorithms;

class MoveGenerator
{
public:
    MoveGenerator();
    MoveGenerator(Initializer &);
    ~MoveGenerator();

    uint16_t get_num_of_fields();
    uint8_t get_num_of_players();
    uint8_t get_reverse_direction(uint8_t);
    bool check_if_valid_move(Board &, uint16_t, uint8_t);
    void calculate_valid_no_overwrite_moves_from_player(Board &, uint8_t, uint16_t, uint8_t);
    void calculate_valid_overwrite_moves_from_player(Board &, uint8_t, uint16_t, uint8_t);
    void calculate_moves_from_player_no_ow(Board &, uint8_t, uint8_t);
    void calculate_moves_from_player_ow(Board &, uint8_t, Timer &, uint8_t);
    void calculate_moves_from_frame_no_ow(Board &, uint8_t, uint8_t);
    bool check_no_ow_moves_from_player(Board &, uint8_t, uint8_t);
    bool check_no_ow_moves_from_frame(Board &, uint8_t, uint8_t);
    bool check_if_player_has_no_overwrite_move(Board &, uint8_t, uint8_t);
    void calculate_valid_no_ow_moves(Board &, uint8_t, uint8_t &);
    void calculate_valid_ow_moves(Board &, uint8_t, Timer &, uint8_t &);
    uint32_t generate_move(Board &, Algorithms &, Timer &, bool);
    void get_affected_by_bomb(uint8_t, uint16_t &);
    void sort_players_by_stones(std::vector<std::pair<uint8_t, uint16_t>> &, Board &);
    void select_target_player(uint8_t &, uint8_t &, Board &, std::vector<std::pair<uint8_t, uint16_t>> &);
    uint32_t generate_bomb(Board &, Initializer &, Timer &);

    std::vector<std::vector<uint16_t>> next_coords;

private:
    uint16_t m_num_of_fields;
    uint8_t m_num_of_players;
};

#endif // MOVE_GENERATOR_HPP