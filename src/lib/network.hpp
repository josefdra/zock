#ifndef NETWORK_H
#define NETWORK_H

#define TYPE_RECEIVE_PLAYERNUM 3
#define TYPE_RECEIVE_TURN_REQUEST 4
#define TYPE_RECEIVE_PLAYER_TURN 6
#define TYPE_DISQUALIFICATION 7
#define TYPE_PHASE1_END 8
#define TYPE_GAME_END 9

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <string.h>
#include <stdint.h>
#include <vector>

class Network
{
public:
    Network(const char *, int);
    ~Network();

    bool init();
    void init_socket();
    bool init_server();
    bool connect_to_server();
    void close_socket();
    bool check_socket_acitivity();
    void send_group_number();
    void send_move(uint32_t);
    uint32_t get_actual_message_length(uint8_t &);
    std::stringstream receive_map();
    uint64_t receive_data();

private:
    const char *m_ip;
    int m_port;
    struct sockaddr_in m_server_addr;
    int m_csocket;
};

#endif // NETWORK_H
