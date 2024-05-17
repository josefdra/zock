#include "game.hpp"
#define TIME_LIMIT 200
#define MAX_DEPTH 10
#define MIL 1000
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
void Game::init_sorting(const bool toSort)
{
    m_toSort = toSort;
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

uint16_t Game::get_turn(uint8_t &spec, uint8_t &depth, uint8_t &game_phase, double &delta)
{
    bool panic = delta < 100;
    auto start_time = std::chrono::high_resolution_clock::now();
    if (panic)
    {
        std::unordered_set<uint16_t> valid_moves;
        calculate_valid_moves(m_map, m_players[m_player_number], m_map.m_symbols, valid_moves);
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, valid_moves.size() - 1);
        auto it = valid_moves.begin();
        std::advance(it, dis(gen));
        return *it;
    }

    int bestEval = std::numeric_limits<int>::min();
    uint16_t bestCoord = 0;
    uint16_t tried_turns = 0;
    uint8_t nextPlayer = (m_player_number + 1) % m_map.m_player_count;
    std::unordered_set<uint16_t> valid_moves;
    calculate_valid_moves(m_map, m_players[m_player_number], m_map.m_symbols, valid_moves);
    double duration_last_depth_search_milli = 0;

    for (uint8_t i = 1; i <= MAX_DEPTH; i++)
    {
        auto begin_depth_iteration = std::chrono::high_resolution_clock::now();
        std::cout << "depth: " << static_cast<int>(i) << std::endl;
        std::cout << "delta: " << delta << std::endl;
        std::cout << "last depth evaluation took: " << duration_last_depth_search_milli << " milliseconds" << std::endl;

        if (duration_last_depth_search_milli * 2 < delta)
        {
            try
            {
                double remaining_time = delta;
                for (auto &possibleMove : valid_moves)
                {
                    auto begin_move_iteration = std::chrono::high_resolution_clock::now();

                    std::vector<char> next_map = temp_color(possibleMove, m_players[m_player_number].m_symbol, m_map, m_map.m_symbols);

                    if (m_map.m_symbols[possibleMove] == 'c')
                    {
                        for (auto &p : m_players)
                        {
                            if (p.m_symbol != m_players[m_player_number].m_symbol)
                            {
                                change_players(next_map, m_players[m_player_number].m_symbol, p.m_symbol);
                                int currEval = minimaxOrParanoidWithPruning(m_map, m_players, i - 1, -INT32_MAX, INT32_MAX, next_map, nextPlayer, game_phase, tried_turns, m_toSort, begin_move_iteration, remaining_time);
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
                        int currEval = minimaxOrParanoidWithPruning(m_map, m_players, i - 1, -INT32_MAX, INT32_MAX, next_map, nextPlayer, game_phase, tried_turns, m_toSort, begin_move_iteration, remaining_time);
                        if (currEval > bestEval)
                        {
                            m_choice_value = 0;
                            bestEval = currEval;
                            bestCoord = possibleMove;
                        }
                    }

                    auto end_move_iteration = std::chrono::high_resolution_clock::now();
                    auto iteration_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_move_iteration - begin_move_iteration).count();
                    remaining_time -= iteration_time;
                    std::cout << "remaining time from game: " << remaining_time << std::endl;
                }

                auto end_depth_iteration = std::chrono::high_resolution_clock::now();
                auto depth_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_depth_iteration - begin_depth_iteration).count();
                duration_last_depth_search_milli = depth_duration;
                delta -= depth_duration;
                std::cout << "time left after last depth search: " << delta << std::endl;

                if (delta < TIME_LIMIT)
                {
                    throw TimeoutException("TIMES UP IN EVALUATING MOVE! RETURNING LAST BEST MOVE!");
                }
            }
            catch (const TimeoutException &e)
            {
                std::cerr << "Timeout Exception! " << e.what() << std::endl;
                switch (m_map.m_symbols[bestCoord])
                {
                case 'c':
                    spec = m_choice_value;
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
        }
        else
        {
            break;
        }
    }

    std::cout << "valid positions: " << m_players[m_player_number].m_valid_moves.size() << std::endl;
    auto end_time = std::chrono::high_resolution_clock::now();
    auto elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

#ifdef RELEASING
    std::cout << "minimax/paranoid: tried " << tried_turns << " turns" << std::endl;
    std::cout << "Elapsed time: " << elapsed_time.count() << " microseconds" << std::endl;
#endif

    switch (m_map.m_symbols[bestCoord])
    {
    case 'c':
        spec = m_choice_value;
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
        if (check_players(m_map.m_symbols[c]) && m_map.m_symbols[c] != m_map.m_player_number + 1 + '0')
        {
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
            m_players[m_map.m_symbols[c] - '0' - 1].m_points = m_players[m_map.m_symbols[c] - '0' - 1].m_points + 1;
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
    if (winner == m_players[m_player_number].m_symbol)
    {
        m_winner = 1;
    }
    return;
}