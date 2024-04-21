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
    std::string m_map_name;
    Map m_map;
    std::vector<Player> m_players;
    void determine_winner();
    void calculate_map_value();
    void evaluate_board();
    void run();
    void move(uint16_t);

    // for network games
    void run_network_game();
    void init_player_clients(std::vector<Network> &player_net);
    void connect_players_and_send_groupnumbers(std::vector<Network> &player_net);
    void receive_map_data(std::vector<Network> &player_net);
    void receive_playernumber(std::vector<Network> &player_net);
};

#endif // GAME_HPP