#include "client_connection.hpp"
#include <iostream>
#include <cstring>

void Network::init_socket()
{
    /**
     * @param AF_INET means ipv4 connection
     * @param SOCK_STREAM communication type is a byte stream
     *
     */
    csocket = socket(AF_INET, SOCK_STREAM, 0);
    if (csocket < 0)
    {
        // @todo Fehlerbehandlung
        // ERROR
        return;
    }
    client_initialized = true;
}

void Network::init_server()
{
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(65432);
    server_addr.sin_addr.s_addr = inet_addr("192.168.178.27");
    server_initialized = true;
}

void Network::connect_to_server()
{
    if (client_initialized && server_initialized)
    {
        int connection_result = connect(csocket, (struct sockaddr *)&server_addr, sizeof(server_addr));
        if (connection_result == 0)
        {
            std::cout << "Successfully connected to server!" << std::endl;
        }
        else
        {
            int error_code = errno; // Get the error code
            std::cerr << "Error connecting to server: " << strerror(error_code) << std::endl;
        }
    }
    else
    {
        std::cerr << "Error: Socket or server address not initialized." << std::endl;
    }
}

void Network::send_data(uint8_t type, uint32_t len_of_message, char *message)
{
    // @todo convert input to bytestream
    char *message_to_send;

    int bytes_to_send = send(csocket, message_to_send, len_of_message, 0);
    if (bytes_to_send > 0)
    {
        std::cout << "Sent " << bytes_to_send << " bytes with message:\n"
                  << message << std::endl;
    }
    else
    {
        // @todo Fehlerbehandlung
    }
}

void Network::receive_data()
{
    uint8_t type;
    uint32_t message_length;

    // receive Header type
    int recv_type = recv(csocket, &type, sizeof(type), 0);
    if (recv_type > 0)
    {
        std::cout << "received Header type: " << type << std::endl;
    }
    else
    {
        // @todo Fehlerbehandlung
    }

    int recv_msg_length = recv(csocket, &message_length, sizeof(message_length), 0);
    if (recv_msg_length > 0)
    {
        std::cout << "received message length: " << message_length << std::endl;
    }
    else
    {
        // @todo Fehlerbehandlung
    }

    char message[message_length];
    int recv_msg = recv(csocket, &message, sizeof(message), 0);
    if (recv_msg_length > 0)
    {
        std::cout << "received message:\n"
                  << message << std::endl;
    }
    else
    {
        // @todo Fehlerbehandlung
    }
}