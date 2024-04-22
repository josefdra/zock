#include "client_connection.hpp"
#include <iostream>
#include <stdexcept>

Network::Network(uint8_t p) : m_player(p) {}
Network::~Network() {}

bool Network::handle_user_input()
{
    std::cout << "CLIENT FOR PLAYER " << std::to_string(m_player) << " READY" << std::endl;

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
            if (parse_connect_command(stream, ipAddress, port))
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

bool Network::parse_connect_command(std::stringstream &stream, std::string &ipAddress, uint16_t &port)
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
            m_ip = ipAddress.c_str();
        }
        else if (option == "-p")
        {
            stream >> port;
            if (validate_port(port))
            {
                hasPort = true;
                m_port = port;
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

bool Network::validate_port(uint16_t &port)
{
    return port >= 1023 && port <= 65535;
}

void Network::init_socket()
{
    /**
     * @param AF_INET means ipv4 connection
     * @param SOCK_STREAM communication type is a byte stream
     *
     */
    m_csocket = socket(AF_INET, SOCK_STREAM, 0);
    if (m_csocket < 0)
    {
        // @todo Fehlerbehandlung
        // ERROR
        return;
    }
    m_client_initialized = true;
}

void Network::init_server()
{
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(m_port);
    server_addr.sin_addr.s_addr = inet_addr(m_ip);
    m_server_initialized = true;
}

void Network::connect_to_server()
{
    if (m_client_initialized && m_server_initialized)
    {
        int connection_result = connect(m_csocket, (struct sockaddr *)&server_addr, sizeof(server_addr));
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

void Network::close_socket()
{
    int result = close(m_csocket);
    if (result < 0)
    {
        // Handle error if closing fails (e.g., errno)
    }
}
void Network::send_data(const uint8_t &type, const uint32_t &len_of_message, const std::vector<uint8_t> &message_vec)
{
    if (len_of_message != message_vec.size())
    {
        throw std::runtime_error("IN: client_connection.cpp/send_data()\nInvalid length of data! Assure that length of data and actual data size are the same!");
    }
    uint8_t *message = format_data_to_byte_array(type, len_of_message, message_vec);
    size_t message_size = sizeof(type) + sizeof(len_of_message) + len_of_message;
    int bytes_to_send = send(m_csocket, message, message_size, 0);
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

uint8_t *Network::format_data_to_byte_array(const uint8_t &type, const uint32_t &len_of_message, const std::vector<uint8_t> &message_vec)
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
void Network::receive_type()
{
    uint8_t type = 0;
    int recv_type = recv(m_csocket, &type, sizeof(type), 0);

    if (recv_type > 0)
    {
        m_type = type;
        std::cout << "received Header type: " << std::to_string(m_type) << std::endl;
    }
    else
    {
        // @todo Fehlerbehandlung
    }
}
void Network::receive_data()
{
    uint32_t message_length = 0;
    int recv_msg_length = recv(m_csocket, &message_length, sizeof(message_length), 0);
    uint32_t actual_message_length;
    if (recv_msg_length > 0)
    {
        actual_message_length = ntohl(message_length); // converts the unsigned integer from network byte order to host byte order.
        m_message_size = actual_message_length;
        std::cout << "received message length: " << std::to_string(actual_message_length) << std::endl;
    }
    else
    {
        // @todo Fehlerbehandlung
    }

    uint8_t message[actual_message_length];
    int recv_msg = recv(m_csocket, &message, sizeof(message), 0);
    if (recv_msg_length > 0)
    {
        m_message = message;
    }
    else
    {
        // @todo Fehlerbehandlung
    }

    if (m_type == TYPE_RECEIVE_TURN_REQUEST)
    {
        // set time value
        for (int i = 0; i < sizeof(uint32_t); i++)
        { // little endian calculation, maybe with if also for big endian systems
            m_time |= (m_message[i] << (sizeof(uint8_t) * i));
        }
        // depth = last Byte of message
        m_search_depth = m_message[sizeof(m_message)];
    }
}

void Network::get_type6_values(uint16_t &x, uint16_t &y, uint8_t &spec, uint8_t &player)
{
    // message order: x_high[0] x_low[1] y_high[2] y_low[3] special[4] player[5]
    x |= (m_message[0] << sizeof(uint8_t)) | m_message[1];
    y |= (m_message[2] << sizeof(uint8_t)) | m_message[3];
    spec = m_message[4];
    player = m_message[5];
}

void Network::send_type_1(uint8_t playernum)
{
    send_data(TYPE_SENDING_GROUP, sizeof(uint8_t), std::vector<uint8_t>{playernum});
}

void Network::send_type_5(uint16_t x, uint16_t y, uint8_t spec)
{
    std::vector<uint8_t> message;
    uint8_t x_byte[sizeof(uint16_t)] = {(x >> 8) & 0xFF, x & 0xFF};
    message.insert(message.end(), x_byte, x_byte + sizeof(uint16_t));
    uint8_t y_byte[sizeof(uint16_t)] = {(y >> 8) & 0xFF, y & 0xFF};
    message.insert(message.end(), y_byte, y_byte + sizeof(uint16_t));
    message.push_back(spec);
    send_data(TYPE_SEND_TURN, sizeof(uint16_t) * 2 + sizeof(uint8_t), message);
}