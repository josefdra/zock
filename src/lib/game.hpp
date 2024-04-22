#ifndef GAME_HPP
#define GAME_HPP

#include <random>
#include <chrono>

#include "map.hpp"
#include "player.hpp"
#include "process_moves.hpp"
#include "algorithms.hpp"
#include "helper.hpp"
#include "client_connection.hpp"

class Game
{
public:
    Game(const std::string);
    ~Game();
    Map m_map;
    std::vector<Player> m_players;
    void determine_winner();
    void calculate_map_value();
    void evaluate_board();
    void run(uint16_t);
    void move(uint16_t);

    // for network games
    void run_network_game();
    void init_player_clients(std::vector<Network> &player_net);
    void connect_players_and_send_groupnumbers(std::vector<Network> &player_net);
    void receive_map_data(std::vector<Network> &player_net);
    void receive_playernumber(std::vector<Network> &player_net);
    uint16_t get_turn(std::vector<Network> &player_net, uint16_t &currPlayer, uint8_t &spec);
    void execute_last_players_turn_local(uint16_t &x, uint16_t &y, uint8_t &spec, uint8_t &players_turn);
};

#endif // GAME_HPP