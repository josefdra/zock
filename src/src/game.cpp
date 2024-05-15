#include "game.hpp"

std::mutex file_mutex;

void print_to_file(const std::string &message)
{
    std::string root_directory = "/home/josefdra/ZOCK/g01";
    std::string filename = root_directory + "/ges_log.txt";
    std::lock_guard<std::mutex> lock(file_mutex);
    std::ofstream file(filename.c_str(), std::ios::app);
    file << message << std::endl;
}

// initialize Game
Game::Game(const std::string &map_name, uint8_t best_mult1, uint8_t best_mult2, uint8_t best_mult3, uint8_t mult1, uint8_t mult2, uint8_t mult3, uint8_t pos)
{
    m_pos = pos;
    Map m_map;
    init_map(map_name);
    init_players(best_mult1, best_mult2, best_mult3, mult1, mult2, mult3);
}

Game::~Game() {}

void Game::init_map(const std::string &map_name)
{
    std::ifstream inputFile(map_name);
    std::stringstream mapfile;
    mapfile << inputFile.rdbuf();
    inputFile.close();
    m_map.read_hash_map(mapfile);
}

void Game::init_players(uint8_t best_mult1, uint8_t best_mult2, uint8_t best_mult3, uint8_t mult1, uint8_t mult2, uint8_t mult3)
{
    for (uint8_t p = 0; p < m_map.m_player_count; p++)
    {
        Player player;
        player.init(m_map.m_initial_overwrite_stones, m_map.m_initial_bombs, (p + 1 + '0'));
        if (p == m_pos)
        {
            player.init_mults(mult1, mult2, mult3);
        }
        else
        {
            player.init_mults(best_mult1, best_mult2, best_mult3);
        }
        m_players.push_back(player);
    }
}

bool Game::run(bool &stop)
{
    while (!stop)
    {
        uint8_t counter = 0;
        uint8_t game_phase = 0;
        for (auto &p : m_players)
        {
            uint16_t coord = get_turn(game_phase, p);
            if (coord != 0)
                execute_move(coord, m_spec, p, m_map);
            if (p.m_valid_moves.size() < 1)
            {
                counter++;
            }
        }
        if (counter == m_players.size())
        {
            break;
        }
    }
    if (!stop && check_winner() == m_pos)
    {
        return true;
    }
    else
    {
        return false;
    }
}

uint16_t Game::get_turn(uint8_t &game_phase, Player &pl)
{
    int bestEval = std::numeric_limits<int>::min();
    uint16_t bestCoord = 0;
    calculate_valid_moves(m_map, pl, m_map.m_symbols);

    for (auto &possibleMove : pl.m_valid_moves)
    {
        if (m_map.m_symbols[possibleMove] == 'c')
        {
            for (auto &p : m_players)
            {
                if (p.m_symbol != pl.m_symbol)
                {
                    std::vector<char> next_map = temp_color(possibleMove, pl.m_symbol, m_map, m_map.m_symbols);
                    change_players(next_map, pl.m_symbol, p.m_symbol);
                    int currEval = evaluate_board(game_phase, pl, next_map, m_map, m_players);
                    if (currEval > bestEval)
                    {
                        m_choice_value = p.m_symbol - '0';
                        bestEval = currEval;
                        bestCoord = possibleMove;
                    }
                }
            }
        }
        else
        {
            std::vector<char> next_map = temp_color(possibleMove, pl.m_symbol, m_map, m_map.m_symbols);
            int currEval = evaluate_board(game_phase, pl, next_map, m_map, m_players);
            if (currEval > bestEval)
            {
                m_choice_value = 0;
                bestEval = currEval;
                bestCoord = possibleMove;
            }
        }
    }
    switch (m_map.m_symbols[bestCoord])
    {
    case 'c':
        m_spec = m_choice_value;
        break;
    case 'b':
        m_spec = 20;
        break;
    default:
        m_spec = 0;
        break;
    }
    return bestCoord;
}

uint8_t Game::check_winner()
{
    std::string root_directory = "/home/josefdra/ZOCK/g01";
    std::string filename = root_directory + "/ges_log.txt";
    std::lock_guard<std::mutex> lock(file_mutex);
    std::ofstream file(filename.c_str(), std::ios::app);
    file << std::endl;
    for (auto &p : m_players)
    {
        p.m_points = 0;
    }
    for (uint16_t c = 1; c < m_map.m_num_of_fields; c++)
    {
        if (check_players(m_map.m_symbols[c]))
        {
            m_players[m_map.m_symbols[c] - '0' - 1].m_points = m_players[m_map.m_symbols[c] - '0' - 1].m_points + 1;
        }
    }
    uint16_t most_points = 0;
    char winner;
    for (auto &p : m_players)
    {
        file << "Player " << p.m_symbol << ": " << p.m_points << std::endl;
        if (p.m_points > most_points)
        {
            winner = p.m_symbol;
            most_points = p.m_points;
        }
    }
    file << std::endl
         << "We are Player " << m_pos + 1 << std::endl;
    file << "The Winner is: Player " << winner << std::endl;
    return winner - '0' - 1;
}