#ifndef GAME_H
#define GAME_H

#include <stdint.h>
#include <vector>

class Network;
class Map;
class Board;
class BombBoard;

class Game
{
public:
    Game();
    ~Game();

    bool is_game_over();
    bool is_bomb_phase();
    void set_game_over();
    void set_disqualified(Board &, uint8_t);
    void set_bomb_phase(BombBoard &, Map &, Board &);
    void calculate_winner(Board &, BombBoard &);
    void end(Board &, uint8_t, BombBoard &);
    void move_request(Network &, uint64_t &, Map &, Board &, bool);
    void bomb_request(Network &, uint64_t &, Map &, BombBoard &, bool);
    void receive_move(Map &, uint64_t &, Board &);
    void receive_bomb(Map &, uint64_t &, BombBoard &);
    void run(Network &, bool);

private:
    bool m_game_over = false;
    bool m_bomb_phase = false;
    uint32_t m_initial_time_limit = 1000000;
};

#endif // GAME_H
