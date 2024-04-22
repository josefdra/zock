#include "client_connection.hpp"
#include <iostream>
#include <stdexcept>

Network::Network(uint8_t p) : m_player(p) {}
Network::~Network() {}

std::vector<int> socket_vec;
std::vector<bool> socket_flags_vec;

bool handle_user_input(Network &net)
{
    std::cout << "CLIENT FOR PLAYER " << std::to_string(net.m_player) << " READY" << std::endl;

    while (true)
    {
        std::string inputLine;
        std::getline(std::cin, inputLine);
        std::stringstream stream(inputLine);

        std::string command;
        std::string ipAddress;
        uint16_t port;

        stream >> command;

        if (command == "connect")
        {
            if (parse_connect_command(stream, ipAddress, port, net))
            {
                std::cout << "Data set successfully.\nREADY TO CONNECT\n\nCONNECTING\n\n"
                          << std::endl;
                return true;
            }
            else
            {
                std::cerr << "Incomplete command. Please enter IP address using '-i <ip>' and port using '-p <port>'." << std::endl;
            }
        }
        else
        {
            std::cerr << "Invalid command. Please use 'connect'." << std::endl;
        }
    }
}

bool parse_connect_command(std::stringstream &stream, std::string &ipAddress, uint16_t &port, Network &net)
{
    bool hasIPAddress = false;
    bool hasPort = false;

    std::string option;

    while (stream >> std::ws >> option)
    {
        if (option == "-i")
        {
            stream >> ipAddress;
            hasIPAddress = true;
            net.m_ip = ipAddress.c_str();
        }
        else if (option == "-p")
        {
            stream >> port;
            if (validate_port(port))
            {
                hasPort = true;
                net.m_port = port;
            }
            else
            {
                std::cerr << "Invalid port number. Please enter a valid port between 0 and 65535." << std::endl;
                return false;
            }
        }
        else
        {
            std::cerr << "Invalid option. Please use '-i <ip>' or '-p <port>'." << std::endl;
            return false;
        }
    }

    if (hasIPAddress && hasPort)
    {
        return true;
    }

    if (!hasIPAddress)
    {
        std::cerr << "Missing IP address. Please use '-i <ip>'." << std::endl;
    }

    if (!hasPort)
    {
        std::cerr << "Missing port number. Please use '-p <port>'." << std::endl;
    }

    while (!(hasIPAddress && hasPort))
    {
        std::string inputLine;

        std::getline(std::cin, inputLine);

        stream.clear();
        stream.str(inputLine);

        while (stream >> std::ws >> option)
        {
            if (option == "-i")
            {
                stream >> ipAddress;
                hasIPAddress = true;
            }
            else if (option == "-p")
            {
                stream >> port;
                if (validate_port(port))
                {
                    hasPort = true;
                }
                else
                {
                    std::cerr << "Invalid port number. Please enter a valid port between 1023 and 65535." << std::endl;
                }
            }
            else
            {
                std::cerr << "Invalid option. Please use '-i <ip>' or '-p <port>'." << std::endl;
            }
        }
    }
    return true;
}

bool validate_port(uint16_t &port)
{
    return port >= 1023 && port <= 65535;
}

void init_socket(Network &net)
{
    /**
     * @param AF_INET means ipv4 connection
     * @param SOCK_STREAM communication type is a byte stream
     *
     */
    net.m_csocket = socket(AF_INET, SOCK_STREAM, 0);
    if (net.m_csocket < 0)
    {
        // @todo Fehlerbehandlung
        // ERROR
        return;
    }
    net.m_client_initialized = true;
    socket_vec.push_back(net.m_csocket);
    socket_flags_vec.push_back(false);
}

void init_server(Network &net)
{
    net.server_addr.sin_family = AF_INET;
    net.server_addr.sin_port = htons(net.m_port);
    net.server_addr.sin_addr.s_addr = inet_addr(net.m_ip.c_str());
    net.m_server_initialized = true;
}

void connect_to_server(Network &net)
{
    if (net.m_client_initialized && net.m_server_initialized)
    {
        int connection_result = connect(net.m_csocket, (struct sockaddr *)&net.server_addr, sizeof(net.server_addr));
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

void close_socket(Network &net)
{
    int result = close(net.m_csocket);
    if (result < 0)
    {
        // Handle error if closing fails (e.g., errno)
    }
}
void send_data(const uint8_t &type, const uint32_t &len_of_message, const std::vector<uint8_t> &message_vec, Network &net)
{
    if (len_of_message != message_vec.size())
    {
        throw std::runtime_error("IN: client_connection.cpp/send_data()\nInvalid length of data! Assure that length of data and actual data size are the same!");
    }
    uint8_t *message = format_data_to_byte_array(type, len_of_message, message_vec);
    size_t message_size = sizeof(type) + sizeof(len_of_message) + len_of_message;
    int bytes_to_send = send(net.m_csocket, message, message_size, 0);
    if (bytes_to_send > 0)
    {
        std::cout << "Sent " << bytes_to_send << " bytes with message:\n"
                  << std::endl;
    }
    else
    {
        // @todo Fehlerbehandlung
    }
}

uint8_t *format_data_to_byte_array(const uint8_t &type, const uint32_t &len_of_message, const std::vector<uint8_t> &message_vec)
{
    // @todo convert input to bytestream
    uint8_t *message_to_send = new uint8_t[sizeof(type) + sizeof(len_of_message) + len_of_message];

    uint8_t len_of_message_hex_buffer[4];
    len_of_message_hex_buffer[0] = (len_of_message >> 24) & 0xFF;
    len_of_message_hex_buffer[1] = (len_of_message >> 16) & 0xFF;
    len_of_message_hex_buffer[2] = (len_of_message >> 8) & 0xFF;
    len_of_message_hex_buffer[3] = len_of_message & 0xFF;

    std::memcpy(message_to_send, &type, sizeof(type));
    std::memcpy(message_to_send + sizeof(type), &len_of_message_hex_buffer, sizeof(len_of_message_hex_buffer));
    std::memcpy(message_to_send + sizeof(type) + sizeof(len_of_message_hex_buffer), message_vec.data(), len_of_message);
    std::cout << message_to_send[sizeof(type) + sizeof(len_of_message_hex_buffer)] << std::endl;

    return message_to_send;
}
void receive_type(Network &net)
{
    uint8_t type = 0;
    int recv_type = recv(net.m_csocket, &type, sizeof(type), 0);

    if (recv_type > 0)
    {
        net.m_type = type;
        std::cout << "received Header type: " << std::to_string(net.m_type) << std::endl;
    }
    else
    {
        // @todo Fehlerbehandlung
    }
}
void receive_data(Network &net)
{
    uint32_t message_length = 0;
    int recv_msg_length = recv(net.m_csocket, &message_length, sizeof(message_length), 0);
    uint32_t actual_message_length;
    if (recv_msg_length > 0)
    {
        actual_message_length = ntohl(message_length); // converts the unsigned integer from network byte order to host byte order.
        net.m_message_size = actual_message_length;
        std::cout << "received message length: " << std::to_string(actual_message_length) << std::endl;
    }
    else
    {
        // @todo Fehlerbehandlung
    }

    uint8_t message[actual_message_length];
    int recv_msg = recv(net.m_csocket, &message, sizeof(message), 0);
    if (recv_msg_length > 0)
    {
        net.m_message = message;
    }
    else
    {
        // @todo Fehlerbehandlung
    }

    if (net.m_type == TYPE_RECEIVE_TURN_REQUEST)
    {
        // set time value
        for (int i = 0; i < sizeof(uint32_t); i++)
        { // little endian calculation, maybe with if also for big endian systems
            net.m_time |= (net.m_message[i] << (sizeof(uint8_t) * i));
        }
        // depth = last Byte of message
        net.m_search_depth = net.m_message[sizeof(net.m_message)];
    }
}

void check_socket_acitivity(uint8_t num_of_players)
{
    fd_set read_fds;
    FD_ZERO(&read_fds);
    int max_fd = -1;

    // Add all player sockets to the set
    for (size_t i = 0; i < num_of_players; ++i)
    {
        FD_SET(socket_vec[i], &read_fds);
        max_fd = std::max(max_fd, socket_vec[i]);
    }

    // Wait for activity on any of the player sockets
    int activity = select(max_fd + 1, &read_fds, NULL, NULL, NULL);
    if (activity < 0)
    {
        // Handle error
        perror("select");
        return;
    }
    // Check each player socket for activity
    for (size_t i = 0; i < socket_vec.size(); ++i)
    {
        if (FD_ISSET(socket_vec[i], &read_fds))
        {
            // Activity on this player's socket
            // Set flag to indicate activity
            socket_flags_vec[i] = true;
        }
        else
        {
            socket_flags_vec[i] = false;
        }
    }
}

void get_type6_values(uint16_t &x, uint16_t &y, uint8_t &spec, uint8_t &player, Network &net)
{
    // message order: x_high[0] x_low[1] y_high[2] y_low[3] special[4] player[5]
    x |= (net.m_message[0] << sizeof(uint8_t)) | net.m_message[1];
    y |= (net.m_message[2] << sizeof(uint8_t)) | net.m_message[3];
    spec = net.m_message[4];
    player = net.m_message[5];
}

void send_type_1(uint8_t playernum, Network &net)
{
    send_data(TYPE_SENDING_GROUP, sizeof(uint8_t), std::vector<uint8_t>{playernum}, net);
}

void send_type_5(uint16_t x, uint16_t y, uint8_t spec, Network &net)
{
    std::vector<uint8_t> message;
    uint8_t x_byte[sizeof(uint16_t)] = {(x >> 8) & 0xFF, x & 0xFF};
    message.insert(message.end(), x_byte, x_byte + sizeof(uint16_t));
    uint8_t y_byte[sizeof(uint16_t)] = {(y >> 8) & 0xFF, y & 0xFF};
    message.insert(message.end(), y_byte, y_byte + sizeof(uint16_t));
    message.push_back(spec);
    send_data(TYPE_SEND_TURN, sizeof(uint16_t) * 2 + sizeof(uint8_t), message, net);
}
