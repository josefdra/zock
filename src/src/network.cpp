#include "network.hpp"
#include "logging.hpp"

Network::Network(const char *ip, int port) : m_ip(ip), m_port(port)
{
    m_server_addr = {};
    m_csocket = 0;
}

Network::~Network()
{
    close_socket();
}

bool Network::init()
{
    init_socket();
    if (!init_server())
        return false;

    if (!connect_to_server())
        return false;

    send_group_number();
    return true;
}

void Network::init_socket()
{
    /**
     * @param AF_INET means ipv4 connection
     * @param SOCK_STREAM communication type is a byte stream
     *
     */
    if ((m_csocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        LOG_ERROR("Socket creation error");
        return;
    }
}

bool Network::init_server()
{
    memset(&m_server_addr, 0, sizeof(m_server_addr)); // Sichere Initialisierung
    m_server_addr.sin_family = AF_INET;
    m_server_addr.sin_port = htons(m_port);
    if (inet_pton(AF_INET, m_ip, &m_server_addr.sin_addr) <= 0)
    {
        LOG_ERROR("Invalid address/ Address not supported");
        return false;
    }
    return true;
}

bool Network::connect_to_server()
{
    if ((connect(m_csocket, (struct sockaddr *)&m_server_addr,
                 sizeof(m_server_addr))) < 0)
    {
        LOG_ERROR("Connection Failed");
        return false;
    }
    return true;
}

void Network::close_socket()
{
    if (close(m_csocket) < 0)
    {
        LOG_ERROR("Failed to close connection");
        return;
    }
}

bool Network::check_socket_acitivity()
{
    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(m_csocket, &read_fds);
    select(m_csocket + 1, &read_fds, NULL, NULL, NULL);
    if (FD_ISSET(m_csocket, &read_fds))
        return true;

    else
        return false;
}

void Network::send_group_number()
{
    uint8_t g_n = 1;
    char m_send_buffer[6];
    m_send_buffer[0] = 1;
    m_send_buffer[1] = 0;
    m_send_buffer[2] = 0;
    m_send_buffer[3] = 0;
    m_send_buffer[4] = 1;
    m_send_buffer[5] = g_n;
    send(m_csocket, m_send_buffer, 6, 0);
}

void Network::send_move(uint32_t move)
{
    char m_send_buffer[10];
    m_send_buffer[0] = 5;
    m_send_buffer[1] = 0;
    m_send_buffer[2] = 0;
    m_send_buffer[3] = 0;
    m_send_buffer[4] = 5;
    m_send_buffer[5] = 0;
    m_send_buffer[6] = (move >> 16) & 0xFF;
    m_send_buffer[7] = 0;
    m_send_buffer[8] = (move >> 8) & 0xFF;
    m_send_buffer[9] = move & 0xFF;
    send(m_csocket, m_send_buffer, 10, 0);
}

uint32_t Network::get_actual_message_length(uint8_t &type)
{
    while (!check_socket_acitivity())
        usleep(10);

    recv(m_csocket, &type, sizeof(type), 0);
    uint32_t message_length = 0;
    recv(m_csocket, &message_length, sizeof(message_length), 0);
    return ntohl(message_length);
}

std::stringstream Network::receive_map()
{
    uint8_t type = 0;
    uint32_t actual_message_length = get_actual_message_length(type);
    std::vector<char> message(actual_message_length + 1);
    recv(m_csocket, message.data(), actual_message_length, 0);
    message[actual_message_length] = '\0';
    std::stringstream ss(message.data());
    LOG_INFO("Received map");
    return ss;
}

uint64_t Network::receive_data()
{
    uint8_t type = 0;
    uint32_t actual_message_length = get_actual_message_length(type);
    if (actual_message_length > 10000)
        actual_message_length = 1;
    uint64_t game_data = 0;
    uint64_t big_type = type;
    unsigned char message[3000];
    recv(m_csocket, &message, actual_message_length, 0);
    for (uint32_t i = 0; i < actual_message_length; i++)
    {
        game_data <<= 8;
        game_data |= message[i];
    }
    return (game_data | (big_type << 56));
}