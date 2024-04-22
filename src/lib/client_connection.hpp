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
#include <vector>
#include <cstring>
#include <sstream>
#include <unistd.h>

/**
 * connect -i 127.0.0.1 -p 7777
 *
 */

class Network
{
public:
    bool m_client_initialized = false;
    bool m_server_initialized = false;

    int m_csocket;
    std::string m_ip;
    uint16_t m_port;
    uint8_t m_player;

    uint8_t m_type;
    uint32_t m_message_size;
    const uint8_t *m_message;

    uint32_t m_time = 0; // currently not used
    uint8_t m_search_depth = 0;

    struct sockaddr_in server_addr;

    Network(uint8_t player);
    ~Network();
};

extern std::vector<int> socket_vec;
extern std::vector<bool> socket_flags_vec;

bool handle_user_input(Network &net);
bool parse_connect_command(std::stringstream &stream, std::string &ipAddress, uint16_t &port, Network &net);
bool validate_port(uint16_t &port);

void init_socket(Network &net);
void init_server(Network &net);
void connect_to_server(Network &net);
void close_socket(Network &net);

uint8_t *format_data_to_byte_array(const uint8_t &type, const uint32_t &len_of_message, const std::vector<uint8_t> &message_vec);

void check_socket_acitivity(uint8_t num_of_players);

void get_type6_values(uint16_t &x, uint16_t &y, uint8_t &spec, uint8_t &player, Network &net);

void send_type_1(uint8_t playernum, Network &n);
void send_type_5(uint16_t x, uint16_t y, uint8_t spec, Network &net);

void send_data(const uint8_t &type, const uint32_t &len_of_message, const std::vector<uint8_t> &message_vec, Network &net);
void receive_type(Network &net);
void receive_data(Network &net);
#endif // CLIENT_CONNECTION_H
