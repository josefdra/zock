#ifndef CLIENT_CONNECTION_H
#define CLIENT_CONNECTION_H

#include <sys/socket.h>
#include <arpa/inet.h>
#include <vector>

// unsigned char message_type;
// unsigned int message_length;
// unsigned int message;

class Network
{
public:
    bool m_client_initialized = false;
    bool m_server_initialized = false;

    int m_csocket;

    struct sockaddr_in server_addr;

    void init_socket();
    void init_server();
    void connect_to_server();
    template <typename DataType>
    void send_data(const uint8_t &type, const uint32_t &len_of_message, const std::vector<DataType> &message_vec);
    template <typename DataType>
    uint8_t *format_data_to_byte_array(const uint8_t &type, const uint32_t &len_of_message, const std::vector<DataType> &message_vec);
    void receive_data();
};

#endif // CLIENT_CONNECTION_H