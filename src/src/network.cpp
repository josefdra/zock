#include "network.hpp"

Network::Network(char *ip, uint16_t port, uint8_t g_n) : m_ip(ip), m_port(port), m_group_number(g_n)
{
    init_socket();
    init_server();
    connect_to_server();
    send_group_number(m_group_number);
    Game m_game;
    init_map_and_player();
    run_game();
}

Network::~Network()
{
    close_socket();
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
        std::cout << "Socket creation error" << std::endl;
        // @todo Fehlerbehandlung
        // ERROR
        return;
    }
}

void Network::init_server()
{
    m_server_addr.sin_family = AF_INET;
    m_server_addr.sin_port = htons(m_port);
    if (inet_pton(AF_INET, m_ip, &m_server_addr.sin_addr) <= 0)
    {
        std::cout << "Invalid address/ Address not supported" << std::endl;
        // @todo Fehlerbehandlung
        // ERROR
        return;
    }
}

void Network::connect_to_server()
{
    if ((status = connect(m_csocket, (struct sockaddr *)&m_server_addr,
                          sizeof(m_server_addr))) < 0)
    {
        std::cout << "Connection Failed" << std::endl;
        // @todo Fehlerbehandlung
        // ERROR
        return;
    }
}

void Network::close_socket()
{
    if (close(m_csocket) < 0)
    {
        // Handle error if closing fails (e.g., errno)
        std::cout << "Failed to close connection" << std::endl;
        // @todo Fehlerbehandlung
        // ERROR
        return;
    }
}

void Network::send_group_number(uint8_t group)
{
    char m_send_buffer[6];
    m_send_buffer[0] = 1;
    m_send_buffer[1] = 0;
    m_send_buffer[2] = 0;
    m_send_buffer[3] = 0;
    m_send_buffer[4] = 1;
    m_send_buffer[5] = group;
    send(m_csocket, m_send_buffer, 6, 0);
}

void Network::init_map_and_player()
{
    receive_data();
    receive_data();
}

bool Network::check_socket_acitivity()
{
    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(m_csocket, &read_fds);
    int activity = select(m_csocket + 1, &read_fds, NULL, NULL, NULL);
    if (FD_ISSET(m_csocket, &read_fds))
    {
        return true;
    }
    else
    {
        return false;
    }
}

void Network::run_game()
{
    m_game.m_map.print_map();
    uint16_t counter = 0;
    bool phase_updated = false;
    while (true)
    {
        if (check_socket_acitivity())
        {
            if (counter >= m_game.m_map.m_num_of_fields / 2 && !phase_updated)
            {
                m_game_phase = 1;
                phase_updated = true;
            }
            receive_data();
        }
        counter++;
    }
}

void Network::send_move(uint8_t x, uint8_t y, uint8_t s)
{
    char m_send_buffer[10];
    m_send_buffer[0] = 5;
    m_send_buffer[1] = 0;
    m_send_buffer[2] = 0;
    m_send_buffer[3] = 0;
    m_send_buffer[4] = 5;
    m_send_buffer[5] = 0;
    m_send_buffer[6] = x;
    m_send_buffer[7] = 0;
    m_send_buffer[8] = y;
    m_send_buffer[9] = s;
    send(m_csocket, m_send_buffer, 10, 0);
}

void Network::receive_map(uint32_t actual_message_length)
{
    char message[actual_message_length + 1];
    int recv_msg = recv(m_csocket, &message, actual_message_length, 0);
    message[actual_message_length] = '\0';
    std::stringstream ss(message);
    std::cout << "Received map, initializing map" << std::endl;
    m_game.init_map(ss);
}

void Network::receive_player_number(uint32_t actual_message_length)
{
    char message[actual_message_length];
    int recv_msg = recv(m_csocket, &message, actual_message_length, 0);
    m_game.m_player_number = message[0] - 1;
    std::cout << "Received player_number, initializing players, we are player " << (int)(m_game.m_player_number + 1) << std::endl;
    m_game.init_players();
}

void Network::receive_move_prompt(uint32_t actual_message_length)
{
    std::cout << "Received move prompt, calculating move" << std::endl;
    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
    m_game.evaluate_board(m_game_phase);
    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "Evaluation-Time: " << duration.count() << "s" << std::endl;
    std::cout << std::endl;
    char message[actual_message_length];
    int recv_msg = recv(m_csocket, &message, actual_message_length, 0);
    memcpy(&m_time, message, sizeof(m_time));
    m_search_depth = message[4];
    uint8_t spec = 0; // special stone value needs to be added
    uint16_t turn;
    if (m_game_phase != 2)
    {
        turn = m_game.get_turn(spec);
        std::cout << "Placing stone at: ";
    }
    else
    {
        turn = m_game.get_bomb_throw();
        std::cout << "Throwing bomb at: ";
    }
    uint8_t x = 0, y = 0;
    one_dimension_2_second_dimension(turn, x, y, m_game.m_map);
    std::cout << (int)x << ", " << (int)y << ", Special value: " << (int)spec << std::endl;
    send_move(x, y, spec);
}

void Network::receive_move(uint32_t actual_message_length)
{
    char message[actual_message_length + 1];
    int recv_msg = recv(m_csocket, &message, actual_message_length, 0);
    uint8_t x, y;
    uint8_t spec, player;
    x = message[1];
    y = message[3];
    spec = message[4];
    player = message[5];
    uint16_t coord;
    two_dimension_2_one_dimension(coord, x, y, m_game.m_map);
    std::cout << "Player " << (int)player << " has " << m_game.m_players[player - 1].m_overwrite_stones << " overrides and " << m_game.m_players[player - 1].m_bombs << " bombs" << std::endl;
    if (m_game_phase != 2)
    {
        std::cout << "Received move at: ";
        execute_move(coord, spec, m_game.m_players[player - 1], m_game.m_map);
    }
    else
    {
        std::cout << "Received bomb at: ";
        execute_bomb(coord, m_game.m_map, m_game.m_players[player - 1]);
    }
    std::cout << (int)x << ", " << (int)y << ", Special value: " << (int)spec << " from player " << (int)player << std::endl;
    std::cout << std::endl;
    m_game.m_map.print_map();
}

void Network::receive_disqualification(uint32_t actual_message_length)
{
    char message[actual_message_length];
    int recv_msg = recv(m_csocket, &message, actual_message_length, 0);
    std::cout << "Player " << (int)message[0] << " is disqulified" << std::endl;
    if (message[0] == m_game.m_player_number + 1)
    {
        return;
    }
}

void Network::receive_end_of_phase_1()
{
    m_game_phase = 2;
    std::cout << "Entering phase 2" << std::endl;
}

void Network::receive_end_of_game()
{
    std::cout << "The game ended" << std::endl;
    m_game.check_winner();
    return;
}

void Network::receive_data()
{
    uint8_t type = 0;
    int recv_type = recv(m_csocket, &type, sizeof(type), 0);
    uint32_t message_length = 0;
    int recv_msg_length = recv(m_csocket, &message_length, sizeof(message_length), 0);
    uint32_t actual_message_length;
    if (recv_msg_length > 0)
    {
        actual_message_length = ntohl(message_length); // converts the unsigned integer from network byte order to host byte order.
        switch (type)
        {
        case TYPE_RECEIVE_MAP:
            receive_map(actual_message_length);
            break;
        case TYPE_RECEIVE_PLAYERNUM:
            receive_player_number(actual_message_length);
            break;
        case TYPE_RECEIVE_TURN_REQUEST:
            receive_move_prompt(actual_message_length);
            break;
        case TYPE_RECEIVE_PLAYER_TURN:
            receive_move(actual_message_length);
            break;
        case TYPE_DISQUALIFICATION:
            receive_disqualification(actual_message_length);
            break;
        case TYPE_PHASE1_END:
            receive_end_of_phase_1();
            break;
        case TYPE_GAME_END:
            receive_end_of_game();
            break;
        default:
            std::cout << "Invalid message type" << std::endl;
            // @todo Fehlerbehandlung
            // ERROR
            return;
        }
    }
    else
    {
        // @todo Fehlerbehandlung
    }
}
