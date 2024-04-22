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

// unsigned char message_type;
// unsigned int message_length;
// unsigned int message;

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
    const char *m_ip;
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

    bool handle_user_input();
    bool parse_connect_command(std::stringstream &stream, std::string &ipAddress, uint16_t &port);
    bool validate_port(uint16_t &port);

    void init_socket();
    void init_server();
    void connect_to_server();
    void close_socket();
    void send_data(const uint8_t &type, const uint32_t &len_of_message, const std::vector<uint8_t> &message_vec);
    uint8_t *format_data_to_byte_array(const uint8_t &type, const uint32_t &len_of_message, const std::vector<uint8_t> &message_vec);
    void receive_type();
    void receive_data();

    void get_type6_values(uint16_t &x, uint16_t &y, uint8_t &spec, uint8_t &player);

    void send_type_1(uint8_t playernum);
    void send_type_5(uint16_t x, uint16_t y, uint8_t spec);
};

#endif // CLIENT_CONNECTION_H