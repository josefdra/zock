#include "game.hpp"
#include "helper.hpp"

// initialize Game
Game::Game(const std::string map_name) : m_map_name(map_name),
                                         m_map(map_name)
{
    // initialize players
    for (int i = 1; i < m_map.m_player_count + 1; i++)
    {
        Player player('0' + i, m_map.m_initial_overwrite_stones, m_map.m_initial_bombs);
        m_players.push_back(player);
    }
}

Game::~Game() {}

void Game::run()
{
    std::cout
        << "------------------------------ Start -------------------------------" << std::endl
        << std::endl;
    m_map.print_map();
    bool valid_moves = true;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, m_map.m_player_count - 1);
    uint16_t start_player = dis(gen);

    for (int j = 0; j < 9999999999; j++)
    {
        if (valid_moves)
        {
            valid_moves = false;
            for (int i = 0; i < m_map.m_player_count; i++)
            {
                move((start_player + i) % m_map.m_player_count);
            }
            for (auto player : m_players)
            {
                if (player.m_has_valid_moves)
                {
                    valid_moves = true;
                }
            }
        }
        else
        {
            break;
        }
    }
    std::cout
        << "------------------------------ Finish ------------------------------" << std::endl
        << std::endl;
    m_map.print_map();
    determine_winner();
}

void Game::move(uint16_t i)
{
    uint16_t coord;
    std::cout
        << "------------------------ Next Move: Player " << i + 1 << "------------------------" << std::endl
        << std::endl;
    h_res_clock::time_point start_time = h_res_clock::now();
    check_moves(m_map, m_players[i]);
    h_res_clock::time_point end_time = h_res_clock::now();
    std::chrono::duration<double, std::micro> elapsed_time =
        std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    std::cout << "Function: "
              << "check_moves" << std::endl;
    std::cout << "Elapsed time: " << elapsed_time.count() << " microseconds" << std::endl;

    if (m_players[i].m_valid_moves.size() > 0)
    {
        m_players[i].m_has_valid_moves = true;
        auto elem = m_players[i].m_valid_moves.begin();
        coord = elem->first;
        start_time = h_res_clock::now();
        execute_move(coord, m_players[i], m_map);
        h_res_clock::time_point end_time = h_res_clock::now();
        std::chrono::duration<double, std::micro> elapsed_time =
            std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
        std::cout << "Function: "
                  << "execute_move" << std::endl;
        std::cout << "Elapsed time: " << elapsed_time.count() << " microseconds" << std::endl;

        m_players[i].m_valid_moves.clear();
        m_map.print_map();
    }
    else
    {
        std::cout << "No valid moves for player " << m_players[i].m_symbol << std::endl
                  << std::endl;
        m_players[i].m_has_valid_moves = false;
    }
}

void Game::determine_winner()
{
    uint16_t possible_points = 0;
    for (int i = 1; i < (m_map.m_width * m_map.m_height + 1); i++)
    {
        if (m_map.m_symbol_and_transitions[i].symbol != '-')
        {
            possible_points++;
            if (48 < m_map.m_symbol_and_transitions[i].symbol < 57)
            {
                m_players[m_map.m_symbol_and_transitions[i].symbol - '0'].m_points++;
            }
        }
    }
    uint16_t winning_points = 0;
    char winner;
    std::cout << "These are the points:" << std::endl;
    for (auto p : m_players)
    {
        std::cout << "Player " << p.m_symbol << ": " << std::setw(4) << p.m_points << "/" << possible_points << std::endl;
        if (p.m_points > winning_points)
        {
            winning_points = p.m_points;
            winner = p.m_symbol;
        }
    }
    std::cout << std::endl
              << "The winner is: Player " << winner << " with " << winning_points << "/" << possible_points << std::endl
              << std::endl;
}