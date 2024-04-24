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

    // void print_valid_moves(uint16_t);
    // void print_frontiers(Map &);
    // void check_protected_fields(Map &);
    // void check_before_before_protected_fields(Map &);
    // bool check_if_remove_border(Map &, uint16_t);
    // void update_borders(Map &);
    // void get_frontier_score(Map &);
    // void get_moves_score(Map &);
    // end coord          //special field(0-4)       //coords of the way
    
    std::unordered_map<uint16_t, int16_t> m_moves_scores;
    uint16_t get_turn(uint8_t &);
    uint16_t get_bomb_throw();
    Map m_map;
    std::vector<Player> m_players;
    uint8_t m_player_number;

    // std::vector<int16_t> staticMapEval;
    // std::unordered_set<uint16_t> m_protected_fields;
    // std::unordered_set<uint16_t> m_before_before_protected_fields;
    // std::unordered_set<uint16_t> m_borders;
    // std::unordered_set<uint16_t> m_before_borders;
    // std::unordered_set<uint16_t> m_before_before_borders;
    // int32_t m_map_value;
    // uint16_t m_frontier_score = 0;
};

#endif // GAME_HPP
