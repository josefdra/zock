#ifndef GAME_HPP
#define GAME_HPP

#include <random>
#include <chrono>

#include "map.hpp"
#include "player.hpp"
#include "algorithms.hpp"
#include "helper.hpp"

class Game
{
public:
    Game();
    ~Game();
    void init_map(std::stringstream &);
    void init_players();
    uint16_t get_turn(uint8_t &);
    uint16_t get_bomb_throw();
    void print_evaluation();
    int evaluate_board(uint8_t);
    void get_frontier_score(Player &);
    // end coord          //special field(0-4)       //coords of the way
    std::unordered_map<uint16_t, int16_t> m_moves_scores;
    std::unordered_map<uint16_t, uint16_t> m_good_fields;
    std::unordered_map<uint16_t, int16_t> m_bad_fields;
    Map m_map;
    std::vector<Player> m_players;
    uint8_t m_player_number;
};

#endif // GAME_HPP
