#ifndef CLIENT_CONNECTION_H
#define CLIENT_CONNECTION_H

#define TYPE_SENDING_GROUP 1
#define TYPE_RECEIVE_MAP 2
#define TYPE_RECEIVE_PLAYERNUM 3
#define TYPE_RECEIVE_TURN_REQUEST 4
#define TYPE_SEND_TURN 5
#define TYPE_RECEIVE_PLAYER_TURN 6
#define TYPE_DISQUALIFICATION 7
#define TYPE_PHASE1_END 8
#define TYPE_GAME_END 9

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "game.hpp"

class Network
{
public:
    Network(const char *, uint16_t, uint8_t, uint8_t, uint8_t, uint8_t, bool);
    ~Network();

    void init_socket();
    void init_server();
    void connect_to_server();
    void close_socket();
    void send_group_number(uint8_t);
    void init_map_and_player(uint8_t, uint8_t, uint8_t);
    bool check_socket_acitivity();
    void run_game();
    void send_move(uint8_t, uint8_t, uint8_t);
    void receive_map(uint32_t);
    void receive_player_number(uint32_t);
    void receive_move_prompt();
    void receive_move(uint32_t);
    void receive_disqualification(uint32_t);
    void receive_end_of_phase_1();
    void receive_end_of_game();
    void receive_data();

    uint16_t m_port;
    const char *m_ip;
    uint8_t m_group_number;
    struct sockaddr_in m_server_addr;
    int m_csocket;
    int status;
    bool m_socket_flag = false;
    uint8_t m_game_phase = 0;
    Game m_game;
    uint32_t m_time = 0;
    uint8_t m_search_depth = 0;
};

#endif // CLIENT_CONNECTION_H
