#ifndef GAME_H
#define GAME_H

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
    bool m_game_over = false;
    bool m_bomb_phase = false;
    uint32_t m_initial_time_limit = 1000000;
};

#endif // GAME_H
