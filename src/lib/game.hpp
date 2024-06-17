#ifndef GAME_H
#define GAME_

#define INITIAL_TIME_LIMIT 1000000
#define BYTE 8
#define TWO_BYTES 16
#define FOUR_BYTES 32
#define SEVEN_BYTES 56
#define ONE_SET_BYTE 0xFF
#define FOUR_SET_BYTES 0xFFFFFFFF

#include <stdint.h>
#include <vector>
#include <timer.hpp>

class Network;
class Map;
class Board;

class Game
{
public:
    Game();
    ~Game();

    bool is_game_over();
    bool is_bomb_phase();
    void set_game_over();
    void set_disqualified(Board &, uint8_t);
    void set_bomb_phase();
    void calculate_winner(Board &);
    void end(Board &, uint8_t);
    void turn_request(Network &, uint64_t &, Map &, Board &, bool, bool);
    void receive_turn(Map &, uint64_t &, Board &, bool);
    void run(Network &, bool);

private:
    bool m_game_over;
    bool m_bomb_phase;
    uint32_t m_initial_time_limit;
};

#endif // GAME_H
