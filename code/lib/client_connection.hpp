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
    bool client_initialized = false;
    bool server_initialized = false;

    int csocket;

    struct sockaddr_in server_addr;

    void init_socket();
    void init_server();
    void connect_to_server();
    void send_data(uint8_t type, uint32_t len_of_message, char *message);
    void receive_data();
};

#endif // CLIENT_CONNECTION_H