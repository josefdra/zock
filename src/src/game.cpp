#include "game.hpp"

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

bool Game::run()
{
    m_map.print_map();
    while (1)
    {
        uint8_t counter = 0;
        uint8_t game_phase = 0;
        for (auto &p : m_players)
        {
            std::cout << "Player " << p.m_symbol << std::endl;
            m_map.m_symbols = get_turn(game_phase, p);
            if (p.m_valid_moves.size() < 1)
            {
                std::cout << "no valid move" << std::endl;
                counter++;
            }
            else
            {
                m_map.print_map();
            }
        }
        if (counter == m_players.size())
        {
            break;
        }
    }
    if (check_winner() == m_pos)
    {
        return true;
    }
    else
    {
        return false;
    }
}

std::vector<char> Game::get_turn(uint8_t &game_phase, Player &pl)
{
    std::vector<char> return_map = m_map.m_symbols;
    uint8_t depth = 1;
    int bestEval = std::numeric_limits<int>::min();
    std::unordered_set<uint16_t> valid_moves;
    calculate_valid_moves(m_map, pl, m_map.m_symbols, valid_moves);

    for (auto &possibleMove : valid_moves)
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
                        bestEval = currEval;
                        return_map = next_map;
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
                bestEval = currEval;
                return_map = next_map;
            }
        }
    }
    return return_map;
}

uint8_t Game::check_winner()
{
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
        if (p.m_points > most_points)
        {
            winner = p.m_symbol;
            most_points = p.m_points;
        }
    }
    return winner - '0' - 1;
}