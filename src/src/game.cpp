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

uint16_t Game::get_turn(uint8_t &spec)
{
    uint16_t coord = 0;
    char answer;

    check_moves(m_map, m_players[m_player_number]);
    if (m_players[m_player_number].m_valid_moves.size() > 0)
    {
        m_players[m_player_number].m_has_valid_moves = true;
        auto elem = m_players[m_player_number].m_valid_moves.begin();
        // bestpos = minimaxWithPruning(0, 10, -INFINITY, INFINITY, true, m_map, m_players.at(i));
        coord = elem->first;
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
    for (uint16_t c = 0; c < m_map.m_num_of_fields; c++)
    {
        if (m_map.get_symbol(c) == m_players[(m_player_number + 1) % m_map.m_player_count].m_symbol)
        {
            return c;
        }
        else
        {
            std::cout << "something went wrong in bomb throw 1" << std::endl;
            return 0;
        }
    }
    std::cout << "something went wrong in bomb throw 2" << std::endl;
    return 0;
}

/*
void Game::print_valid_moves(uint16_t width)
{
    uint16_t x, y, c;
    for (auto &elem : m_valid_moves)
    {
        c = elem.first;
        c % width == 0 ? x = width - 1 : x = c % width - 1;
        y = (c - (x + 1)) / width;
        std::cout << "(" << x << " , " << y << ")" << std::endl;
    }
}


void Game::print_frontiers(Map &m)
{
    for (uint16_t c = 1; c < m.m_num_of_fields + 1; c++)
    {
        if (m.get_symbol(c) == m_symbol)
        {
            std::cout << getColorString(Colors((m.get_symbol(c) - '0'))) << std::setw(3) << check_frontier(m, c) << " "
                      << "\e[0m";
        }
        else
        {
            std::cout << std::setw(3) << m.get_symbol(c) << " ";
        }
        if (c % m.m_width == 0)
        {
            std::cout << std::endl;
        }
    }
    std::cout << std::endl;
}

void Game::check_protected_fields(Map &m)
{
    m_protected_fields.clear();
    for (auto &c : m.m_corners)
    {
        m.m_protected_fields.insert(c);
        if (check_empty_fields(m.get_symbol(c)))
        {
            m_protected_fields.insert(c);
        }
        else if (m.get_symbol(c) == m_symbol)
        {
            m_protected_fields.insert(c);
            for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d += 2)
            {
                if (m.get_transition(c, d) != 0)
                {
                    m_protected_fields.insert(m.get_transition(c, d));
                    m.m_protected_fields.insert(m.get_transition(c, d));
                }
            }
        }
    }
}

void Game::check_before_before_protected_fields(Map &m)
{
    m_before_before_protected_fields.clear();
    for (auto &c : m.m_before_protected_fields)
    {
        for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d++)
        {
            if (m_protected_fields.find(m.get_transition(c, d)) != m_protected_fields.end())
            {
                m_before_before_protected_fields.insert(m.get_transition(c, (d + 4) % 8) / 10);
                m_before_before_protected_fields.insert(m.get_transition(c, (d + 3) % 8) / 10);
                m_before_before_protected_fields.insert(m.get_transition(c, (d + 5) % 8) / 10);
            }
        }
    }
    m_before_before_protected_fields.erase(0);
}

bool Game::check_if_remove_border(Map &m, uint16_t c)
{
    bool val = false;
    if (m.m_protected_fields.find(c) != m.m_protected_fields.end())
    {
        val = true;
    }
    else if (m.m_before_protected_fields.find(c) != m.m_before_protected_fields.end())
    {
        val = true;
    }
    else if (m_before_before_protected_fields.find(c) != m_before_before_protected_fields.end())
    {
        val = true;
    }
    return val;
}

void Game::update_borders(Map &m)
{
    m_borders.clear();
    m_before_borders.clear();
    m_before_before_borders.clear();
    for (auto &c : m.m_borders)
    {
        if (!check_if_remove_border(m, c))
        {
            m_borders.insert(c);
        }
    }
    for (auto &c : m.m_before_borders)
    {
        if (!check_if_remove_border(m, c))
        {
            m_before_borders.insert(c);
        }
    }
    for (auto &c : m.m_before_before_borders)
    {
        if (!check_if_remove_border(m, c))
        {
            m_before_before_borders.insert(c);
        }
    }
}

void Game::get_frontier_score(Map &m)
{
    for (uint16_t c = 1; c < m.m_num_of_fields + 1; c++)
    {
        if (m.get_symbol(c) == m_symbol)
        {
            m_frontier_score += check_frontier(m, c);
        }
    }
}

void Game::get_moves_score(Map &m)
{
    std::cout << getColorString(Colors(m_symbol - '0')) << "Number of moves for Player " << m_symbol << ": " << m_valid_moves.size() << "\e[0m" << std::endl;
    std::cout << getColorString(Colors(m_symbol - '0')) << std::endl
              << "-----------Scores:"
              << "\e[0m" << std::endl
              << std::endl;
    std::string msg = "";
    for (auto &move : m_valid_moves)
    {
        uint16_t special = std::get<0>(move.second);
        if (special == 0) // normal
        {
            if (check_empty_fields(m.get_symbol(move.first))) // empty field (0)
            {
                m_moves_scores[move.first] = 1;
                msg = "(0)";
            }
            else if (check_players(m.get_symbol(move.first))) // enemy player
            {
                m_moves_scores[move.first] = -20;
                msg = "enemy player";
            }
            else // expansion-stone
            {
                m_moves_scores[move.first] = -10;
                msg = "expansion-stone";
            }
        }
        else if (special == 1) // inversion
        {
            m_moves_scores[move.first] = 25;
            msg = "inversion";
        }
        else if (special == 2) // choice
        {
            m_moves_scores[move.first] = 25;
            msg = "choice";
        }
        else if (special == 3) // bonus
        {
            m_moves_scores[move.first] = 25;
            msg = "bonus";
        }
        else
        {
            std::cout << "invalid special value" << std::endl;
        }
        std::cout << getColorString(Colors(m_symbol - '0')) << move.first << "-Move (" << move.first % m.m_width - 1 << "," << move.first / m.m_width << "): "
                  << m_moves_scores[move.first] << " - " << msg << "\e[0m" << std::endl;
    }
    std::cout << std::endl;
}
*/