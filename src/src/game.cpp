#include "game.hpp"

// initialize Game
Game::Game()
{
    Map m_map;
    Player m_player;
}

Game::~Game() {}

void Game::init_map(std::stringstream &ss)
{
    m_map.read_hash_map(ss);
}

void Game::init_players()
{
    for (uint8_t p = 0; p < m_map.m_player_count; p++)
    {
        Player player;
        player.init(m_map.m_initial_overwrite_stones, m_map.m_initial_bombs, (p + 1 + '0'));
        m_players.push_back(player);
    }
}

//////////////////////// NETWORK GAMES ////////////////////////

/**
 * @brief Maybe add something like this for network games:
 *
 * Process of a network game:
 * 1. establish TCP connection
 * 2. send groupnumber to server (type 1)
 * 3. receive Map (type 2)
 * 4. receive Playernumber (type 3)
 * 5. while (!type 8)
 * {
 *      receive turn notification (type 4)
 *      send turn (type 5)
 *      receive Players turn (type 6)
 *      (Maybe) receive disqualification of player (type 7)
 * }
 *
 *
 */

uint16_t Game::get_turn(uint8_t &spec, uint8_t &depth, uint8_t &game_phase)
{
    uint16_t coord = 0;
    char answer;

    h_res_clock::time_point start_time = h_res_clock::now();

    check_moves(m_map, m_players[m_player_number]);

    h_res_clock::time_point end_time = h_res_clock::now();
    std::chrono::duration<double, std::micro> elapsed_time =
        std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    std::cout << "check_moves unordered_map: " << std::endl;
    std::cout << "Elapsed time: " << elapsed_time.count() << " microseconds" << std::endl;

    if (m_players[m_player_number].m_valid_moves.size() > 0)
    {
        m_players[m_player_number].m_has_valid_moves = true;
        auto elem = m_players[m_player_number].m_valid_moves.begin();
        // bestpos = minimaxWithPruning(0, 10, -INFINITY, INFINITY, true, m_map, m_players.at(i));
        // depth = 3;
        coord = getBestPosition(m_player_number, m_map, depth, game_phase, *this);
        if (m_map.get_symbol(coord) == 'c')
        {
            std::cout << "Mit welchem Spieler wollen Sie tauschen?: ";
            std::cin >> spec;
            spec -= '0';
        }
        else if (m_map.get_symbol(coord) == 'b')
        {
            do
            {
                std::cout << "Wollen Sie eine Bombe(b) oder einen Überschreibstein(u)?: ";
                std::cin >> answer;
                if (answer == 'b')
                {
                    spec = 20;
                }
                else if (answer == 'u')
                {
                    spec = 21;
                }

            } while (answer != 'b' && answer != 'u');
        }
    }
    return coord;
}

uint16_t Game::get_bomb_throw()
{
    for (uint16_t c = 1; c < m_map.m_num_of_fields; c++)
    {
        if (m_map.get_symbol(c) == m_players[(m_player_number + 1) % m_map.m_player_count].m_symbol)
        {
            return c;
        }
    }
    // if the return doesn't happen, the next enemy has no more stones
    // @todo change calculation of throw
    // for now, this will throw a bomb at the first empty field
    for (uint16_t c = 1; c < m_map.m_num_of_fields; c++)
    {
        if (m_map.get_symbol(c) != m_players[m_player_number].m_symbol && m_map.get_symbol(c) != '-')
        {
            return c;
        }
    }
    std::cout << "something went wrong in bomb throw" << std::endl;
    return 0;
}

void Game::print_evaluation(Map &m)
{
    for (uint16_t c = 1; c < m_map.m_num_of_fields; c++)
    {
        std::cout << std::setw(4) << m.m_good_fields[c] << " ";
        if (c % m_map.m_width == 0)
        {
            std::cout << std::endl;
        }
    }
    std::cout << std::endl;
    for (uint16_t c = 1; c < m_map.m_num_of_fields; c++)
    {
        std::cout << std::setw(4) << m.m_bad_fields[c] << " ";
        if (c % m_map.m_width == 0)
        {
            std::cout << std::endl;
        }
    }
    std::cout << std::endl;
}

int Game::evaluate_board(uint8_t game_phase, Player &pl, Map &map)
{
    pl.m_board_value = 0;

    uint8_t value = 10;
    uint8_t bonus_value = 30;
    uint8_t choice_value = 20 * (game_phase + 1);
    uint8_t inversion_value = 30;
    uint8_t before_special_value = 30;
    std::array<int, 9> wall_values{0, 1, 2, 3, 16, 12, 8, 4, 0};
    for (uint16_t c = 1; c < map.m_num_of_fields; c++)
    {
        map.m_good_fields[c] = 0;
        map.m_bad_fields[c] = 0;
    }
    for (uint16_t c = 1; c < map.m_num_of_fields; c++)
    {
        bool special = false;
        char s = map.get_symbol(c);
        switch (s)
        {
        case 'b':
            map.m_good_fields[c] += bonus_value;
            special = true;
            break;
        case 'c':
            map.m_good_fields[c] += choice_value;
            special = true;
            break;
        case 'i':
            map.m_bad_fields[c] -= inversion_value;
            special = true;
            break;
        default:
            break;
        }
        uint8_t value_to_add = 0;
        uint8_t walls = 0;
        for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d++)
        {
            if (map.get_transition(c, d) == 0)
            {
                walls++;
                uint16_t trans = map.get_transition(c, (d + 4) % 8);
                if (trans != 0)
                {
                    map.m_bad_fields[trans] -= 10;
                }
            }
            if (special == true)
            {
                uint16_t trans = map.get_transition(c, d);
                if (trans != 0)
                {
                    map.m_bad_fields[trans] -= before_special_value;
                }
            }
        }
        map.m_good_fields[c] += wall_values[walls] * value;
    }
    // print_evaluation();
    for (uint16_t c = 1; c < map.m_num_of_fields; c++)
    {

        // if (m_map.get_symbol(c) == m_players[p].m_symbol)
        //{
        get_frontier_score(pl);
        pl.m_board_value += map.m_good_fields[c] + map.m_bad_fields[c];
        // pl.m_board_value += pl.m_frontier_score;
        // }
    }
    // std::cout << "Board value of Player " << pl.m_symbol << " right now: " << pl.m_board_value << std::endl;

    return pl.m_board_value;
}

void Game::get_frontier_score(Player &p)
{
    for (uint16_t c = 1; c < m_map.m_num_of_fields; c++)
    {
        if (m_map.get_symbol(c) == p.m_symbol)
        {
            p.m_frontier_score += check_frontier(m_map, c);
        }
    }
}

void Game::check_winner()
{
    for (uint16_t c = 1; c < m_map.m_num_of_fields; c++)
    {
        if (!check_empty_fields(c) && m_map.get_symbol(c) != 'x')
        {
            m_players[m_map.get_symbol(c) - '0' - 1].m_points += 1;
        }
    }
    uint16_t most_points = 0;
    char winner;
    for (auto &p : m_players)
    {
        std::cout << "Player " << p.m_symbol << " has " << p.m_points << " Points" << std::endl;
        if (p.m_points > most_points)
        {
            winner = p.m_symbol;
            most_points = p.m_points;
        }
    }
    std::cout << "\nThe Winner is: Player " << winner << "!" << std::endl;
}