#include "game.hpp"

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

void Game::determine_winner()
{
    uint16_t possible_points = 0;
    for (int i = 1; i < (m_map.m_width * m_map.m_height + 1); i++)
    {
        if (m_map.m_symbol_and_transitions[i].symbol != '-')
        {
            possible_points++;
            if (check_players(m_map.m_symbol_and_transitions[i].symbol))
            {
                m_players[m_map.m_symbol_and_transitions[i].symbol - '0' - 1].m_points += 1;
            }
        }
    }
    uint16_t winning_points = 0;
    char winner;
    std::cout << "These are the points:" << std::endl;
    for (auto &p : m_players)
    {
        std::cout << getColorString(Colors(p.m_symbol - '0')) << "Player " << p.m_symbol << ": " << std::setw(4) << p.m_points << "/" << possible_points << std::endl
                  << "\e[0m";
        if (p.m_points > winning_points)
        {
            winning_points = p.m_points;
            winner = p.m_symbol;
        }
    }
    std::cout << std::endl
              << "The winner is: " << getColorString(Colors(winner - '0')) << "Player " << winner << " with " << winning_points << "/" << possible_points << std::endl
              << std::endl
              << "\e[0m";
}

void Game::calculate_map_value()
{
    // @todo add defines
    int corner = 256;
    int before_corner = -256;
    int before_before_corner = 128;
    int border = 96;
    int before_border = -96;
    int before_before_border = 48;
    int special = 256;
    int before_special = -256;
    int before_before_special = 128;
    int cost_of_overwrite_stone = -128;
    int field_value;
    bool player_field = false;
    for (auto &p : m_players)
    {
        p.m_map_value = 0;
        std::cout << getColorString(Colors(p.m_symbol - '0')) << "Output for Player " << p.m_symbol << ":" << std::endl
                  << "\e[0m" << std::endl;
        for (int i = 1; i < (m_map.m_height * m_map.m_width + 1); i++)
        {
            if (std::find(p.m_protected_fields.begin(), p.m_protected_fields.end(), i) != p.m_protected_fields.end() || std::find(m_map.m_corners.begin(), m_map.m_corners.end(), i) != m_map.m_corners.end())
            {
                field_value = corner;
            }
            else if (std::find(m_map.m_before_protected_fields.begin(), m_map.m_before_protected_fields.end(), i) != m_map.m_before_protected_fields.end())
            {
                field_value = before_corner;
            }
            else if (std::find(p.m_before_before_protected_fields.begin(), p.m_before_before_protected_fields.end(), i) != p.m_before_before_protected_fields.end())
            {
                field_value = before_before_corner;
            }
            else if (std::find(p.m_borders.begin(), p.m_borders.end(), i) != p.m_borders.end())
            {
                field_value = border;
            }
            else if (std::find(p.m_before_borders.begin(), p.m_before_borders.end(), i) != p.m_before_borders.end())
            {
                field_value = before_border;
            }
            else if (std::find(p.m_before_before_borders.begin(), p.m_before_before_borders.end(), i) != p.m_before_before_borders.end())
            {
                field_value = before_before_border;
            }
            else
            {
                field_value = 0;
            }
            if (std::find(m_map.m_special_fields.begin(), m_map.m_special_fields.end(), i) != m_map.m_special_fields.end())
            {
                field_value += special;
            }
            else if (std::find(m_map.m_before_special_fields.begin(), m_map.m_before_special_fields.end(), i) != m_map.m_before_special_fields.end())
            {
                field_value += before_special;
            }
            else if (std::find(m_map.m_before_before_special_fields.begin(), m_map.m_before_before_special_fields.end(), i) != m_map.m_before_before_special_fields.end())
            {
                field_value += before_before_special;
            }
            if (m_map.m_symbol_and_transitions[i].symbol == p.m_symbol)
            {
                p.m_map_value += field_value;
                player_field = true;
            }
            if (player_field)
            {
                player_field = false;
                std::cout << getColorString(Colors(p.m_symbol - '0')) << std::setw(4) << field_value << " "
                          << "\e[0m";
            }
            else
            {
                std::cout << std::setw(4) << field_value << " ";
            }
            if ((i % m_map.m_width) == 0)
            {
                std::cout << std::endl;
            }
        }
        p.m_map_value += p.m_frontier_score;
        for (auto &m : p.m_moves_scores)
        {
            p.m_map_value += m.second;
        }
        std::cout << std::endl;
        std::cout << getColorString(Colors(p.m_symbol - '0')) << "Total map value for Player " << p.m_symbol << ": " << p.m_map_value << std::endl
                  << std::endl
                  << "\e[0m";
    }
}

void Game::evaluate_board()
{
    m_map.m_protected_fields.clear();
    m_map.check_corners_borders_special_fields();
    m_map.check_before_before_borders();
    m_map.check_before_before_special_fields();
    for (auto &p : m_players)
    {
        p.check_protected_fields(m_map);
    }
    m_map.check_before_protected_fields(m_players);
    for (auto &p : m_players)
    {
        p.check_before_before_protected_fields(m_map);
    }
    for (auto &p : m_players)
    {
        p.update_borders(m_map);
    }
    calculate_map_value();
}

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
    for (int j = 0; j < 15; j++)
    {
        if (valid_moves)
        {
            valid_moves = false;
            for (int i = 0; i < m_map.m_player_count; i++)
            {

                move((start_player + i) % m_map.m_player_count);
            }
            for (auto &player : m_players)
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
    for (auto &p : m_players)
    {
        check_moves(m_map, p);
        p.get_moves_score(m_map);
        p.print_frontiers(m_map);
    }
    m_map.print_m_frontier_scores(m_players);
    h_res_clock::time_point start_time = h_res_clock::now();
    evaluate_board();
    h_res_clock::time_point end_time = h_res_clock::now();
    std::chrono::duration<double, std::micro> elapsed_time =
        std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    std::cout << "Function: "
              << "evaluate_board" << std::endl;
    std::cout << "Elapsed time: " << elapsed_time.count() << " microseconds" << std::endl;
}

void Game::move(uint16_t i)
{
    uint16_t coord;
    std::cout
        << "------------------------ Next Move: Player " << i + 1 << "------------------------" << std::endl
        << std::endl;
    h_res_clock::time_point start_time = h_res_clock::now();
    check_moves(m_map, m_players[i]);
    uint16_t bestpos;
    // m_map.setFieldValue(m_players.at(i));
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
        // bestpos = minimaxWithPruning(0, 10, -INFINITY, INFINITY, true, m_map, m_players.at(i));
        coord = elem->first;
        start_time = h_res_clock::now();
        std::cout << "coord: " << coord << std::endl;
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
