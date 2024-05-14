#include "game.hpp"
#define TIME_LIMIT 400
#define MAX_DEPTH 10
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
    bool panic = false;
    if (delta < 100)
    {
        panic = true;
    }
    // initialization of some variables is not necessary for a panic return, so here are just the most significant variables initialized
    // and in normal mode (panic == false) the variables get initialized after the timer started to get the most exact time
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
    h_res_clock::time_point start_time = h_res_clock::now();
    int bestEval = std::numeric_limits<int>::min();
    uint16_t bestCoord = 0;
    uint16_t tried_turns = 0;
    uint8_t nextPlayer = ((m_player_number + 1) % m_map.m_player_count);
    std::unordered_set<uint16_t> valid_moves;
    calculate_valid_moves(m_map, m_players[m_player_number], m_map.m_symbols, valid_moves);
    std::chrono::duration<double> duration_last_depth_search = std::chrono::duration<double>::zero();
    std::chrono::duration<double> time_left = std::chrono::duration<double>::zero();
    double duration_last_depth_search_milli = 0;

        for (uint8_t i = 1; i <= MAX_DEPTH; i++)
        {
            auto begin_iteration = std::chrono::high_resolution_clock::now();
            std::cout << "depth: " << static_cast<int>(i) << std::endl;
            std::cout << "delta: " << delta << std::endl;
            std::cout << "last depth evaluation took: " << duration_last_depth_search_milli << " milliseconds" << std::endl;
            if (duration_last_depth_search_milli * 2 < delta)
            {

                for (auto &possibleMove : valid_moves)
                {

                    if (m_map.m_symbols[possibleMove] == 'c')
                    {
                        for (auto &p : m_players)
                        {
                            if (p.m_symbol != m_players[m_player_number].m_symbol)
                            {
                                try {
                                    std::vector<char> next_map = temp_color(possibleMove, m_players[m_player_number].m_symbol, m_map, m_map.m_symbols);
                                    change_players(next_map, m_players[m_player_number].m_symbol, p.m_symbol);
                                    int currEval = minimaxTimer(m_map, m_players, i - 1, -INT32_MAX, INT32_MAX, next_map, nextPlayer, game_phase, tried_turns, m_toSort, delta);
                                    if (currEval > bestEval)
                                    {
                                        m_choice_value = p.m_symbol - '0';
                                        bestEval = currEval;
                                        bestCoord = possibleMove;
                                    }
                                }
                                catch(std::runtime_error e){
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
                        }
                    }
                    else
                    {
                        try{
                        std::vector<char> next_map = temp_color(possibleMove, m_players[m_player_number].m_symbol, m_map, m_map.m_symbols);
                        int currEval = minimaxTimer(m_map, m_players, i - 1, -INT32_MAX, INT32_MAX, next_map, nextPlayer, game_phase, tried_turns, m_toSort, delta);
                        if (currEval > bestEval)
                        {
                            m_choice_value = 0;
                            bestEval = currEval;
                            bestCoord = possibleMove;
                        }
                        }
                        catch(std::runtime_error e){
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
                    auto pause = std::chrono::high_resolution_clock::now();
                    time_left = pause - begin_iteration;
                    double time_left_mil = time_left.count() * 1000;
                    std::cout << "time left for next move evaluation: " << delta - time_left_mil << std::endl;
                    if (delta - time_left_mil < TIME_LIMIT)
                    {
                        std::cout << "TIMES UP! RETURNING LAST BEST MOVE!" << std::endl;
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
                auto end_of_calc = std::chrono::high_resolution_clock::now();
                duration_last_depth_search = end_of_calc - begin_iteration;
                duration_last_depth_search_milli = duration_last_depth_search.count() * 1000;
                std::cout << duration_last_depth_search_milli << std::endl;
                std::cout << "time left after last depth search: " << delta - duration_last_depth_search_milli << std::endl;
                delta -= duration_last_depth_search_milli;
            }
            else
            {
                break;
            }
        }
        /*std::cout << "valid positions: " << m_players[m_player_number].m_valid_moves.size() << std::endl;
        h_res_clock::time_point end_time = h_res_clock::now();
        std::chrono::duration<double, std::micro> elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    */
    
    #ifdef RELEASING
        std::cout << "minimax/paranoid: tried " << tried_turns << " turns" << std::endl;
        //std::cout << "Elapsed time: " << elapsed_time.count() << " microseconds" << std::endl;
    #endif
        // @todo add special evaluation
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
    if (winner == m_players[m_player_number].m_symbol)
    {
        m_winner = 1;
    }
    return;
}