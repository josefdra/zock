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
    h_res_clock::time_point start_time = h_res_clock::now();
    int bestEval = std::numeric_limits<int>::min();
    uint16_t bestCoord = 0;
    uint16_t tried_turns = 0;
    uint8_t nextPlayer = ((m_player_number + 1) % m_map.m_player_count);
    calculate_valid_moves(m_map, m_players[m_player_number], m_map.m_symbols);
    // int test_depth = 4;

    for (auto &possibleMove : m_players[m_player_number].m_valid_moves)
    {
        std::vector<char> next_map = temp_color(possibleMove, m_players[m_player_number].m_symbol, m_map, m_map.m_symbols);
        int currEval = minimaxOrParanoidWithPruning(m_map, m_players, depth - 1, -INT32_MAX, INT32_MAX, next_map, nextPlayer, game_phase, tried_turns);
        if (currEval > bestEval)
        {
            bestEval = currEval;
            bestCoord = possibleMove;
        }
    }
    h_res_clock::time_point end_time = h_res_clock::now();
    std::chrono::duration<double, std::micro> elapsed_time =
        std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    std::cout << "minimax/paranoid: tried " << tried_turns << " turns" << std::endl;
    std::cout << "Elapsed time: " << elapsed_time.count() << " microseconds" << std::endl;
    // @todo add special evaluation
    switch (m_map.m_symbols[bestCoord])
    {
    case 'c':
        spec = m_player_number + 1;
        break;
    case 'b':
        spec = 20;
        break;
    default:
        spec = 0;
        break;
    }
    return bestCoord;
}

uint16_t Game::get_bomb_throw()
{
    for (uint16_t c = 1; c < m_map.m_num_of_fields; c++)
    {
        if(check_players(m_map.m_symbols[c]) && m_map.m_symbols[c] != m_map.m_player_number + 1 + '0'){
            return c;
        }
    }
    std::cout << "something went wrong in bomb throw" << std::endl;
    return 0;
}

void Game::check_winner()
{
    for (auto &p : m_players)
    {
        p.m_points = 0;
    }
    for (uint16_t c = 1; c < m_map.m_num_of_fields; c++)
    {
        if (check_players(m_map.m_symbols[c]))
        {
            m_players[m_map.m_symbols[c] - '0' - 1].m_points += 1;
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
    return;
}