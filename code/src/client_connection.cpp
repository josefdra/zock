#include "client_connection.hpp"
#include <iostream>
#include <cstring>
#include <stdexcept>
#define SWAP_BYTES(x) ((x) >> 8) | ((x) << 8)
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
    server_addr.sin_port = htons(65432);
    server_addr.sin_addr.s_addr = inet_addr("192.168.178.27");
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
template <typename DataType>
void Network::send_data(const uint8_t &type, const uint32_t &len_of_message, const std::vector<DataType> &message_vec)
{
    if (len_of_message != (message_vec.size() * sizeof(DataType)))
    {
        throw std::runtime_error("IN: client_connection.cpp/send_data()\nInvalid length of data! Assure that length of data and actual data size are the same!");
    }
    uint8_t *message = format_data_to_byte_array(type, len_of_message, message_vec);
    size_t message_size = sizeof(type) + sizeof(len_of_message) + len_of_message;
    int bytes_to_send = send(m_csocket, message, message_size, 0);
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

template void Network::send_data<uint8_t>(const uint8_t &type, const uint32_t &len_of_message, const std::vector<uint8_t> &message_vec);
template void Network::send_data<uint16_t>(const uint8_t &type, const uint32_t &len_of_message, const std::vector<uint16_t> &message_vec);
template void Network::send_data<uint32_t>(const uint8_t &type, const uint32_t &len_of_message, const std::vector<uint32_t> &message_vec);

template <typename DataType>
uint8_t *Network::format_data_to_byte_array(const uint8_t &type, const uint32_t &len_of_message, const std::vector<DataType> &message_vec)
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

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    if (typeid(DataType) == typeid(uint16_t))
    {
        DataType helper = message_to_send[sizeof(type) + sizeof(len_of_message_hex_buffer)];
        message_to_send[sizeof(type) + sizeof(len_of_message_hex_buffer)] = message_to_send[sizeof(type) + sizeof(len_of_message_hex_buffer) + 1];
        message_to_send[sizeof(type) + sizeof(len_of_message_hex_buffer) + 1] = helper;
    }
    else
    {
        for (size_t i = 0; i < len_of_message; i += sizeof(DataType))
        {
            for (size_t j = 0; j < sizeof(DataType) / 2; ++j)
            {
                uint8_t *data_ptr1 = &message_to_send[sizeof(type) + sizeof(len_of_message_hex_buffer) + i + j];
                uint8_t *data_ptr2 = &message_to_send[sizeof(type) + sizeof(len_of_message_hex_buffer) + i + sizeof(DataType) - 1 - j];

                // Swap bytes using bitwise operations
                uint8_t temp = *data_ptr1;
                *data_ptr1 = *data_ptr2;
                *data_ptr2 = temp;
            }
        }
    }
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    continue;
#else
#error "Unknown endian"
#endif
    return message_to_send;
}
template uint8_t *Network::format_data_to_byte_array<uint8_t>(const uint8_t &type, const uint32_t &len_of_message, const std::vector<uint8_t> &message_vec);
template uint8_t *Network::format_data_to_byte_array<uint16_t>(const uint8_t &type, const uint32_t &len_of_message, const std::vector<uint16_t> &message_vec);
template uint8_t *Network::format_data_to_byte_array<uint32_t>(const uint8_t &type, const uint32_t &len_of_message, const std::vector<uint32_t> &message_vec);

void Network::receive_data()
{
    uint8_t type;
    uint32_t message_length;

    // receive Header type
    int recv_type = recv(m_csocket, &type, sizeof(type), 0);
    if (recv_type > 0)
    {
        std::cout << "received Header type: " << type << std::endl;
    }
    else
    {
        // @todo Fehlerbehandlung
    }

    int recv_msg_length = recv(m_csocket, &message_length, sizeof(message_length), 0);
    if (recv_msg_length > 0)
    {
        std::cout << "received message length: " << message_length << std::endl;
    }
    else
    {
        // @todo Fehlerbehandlung
    }

    char message[message_length];
    int recv_msg = recv(m_csocket, &message, sizeof(message), 0);
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