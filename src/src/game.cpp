#include "game.hpp"
#include "map.hpp"
#include "network.hpp"
#include "board.hpp"
#include "timer.hpp"
#include "logging.hpp"

Game::Game()
{
    m_game_over = false;
    m_bomb_phase = false;
    m_initial_time_limit = INITIAL_TIME_LIMIT;
}

Game::~Game() {}

bool Game::is_game_over()
{
    return m_game_over;
}

bool Game::is_bomb_phase()
{
    return m_bomb_phase;
}

void Game::set_game_over()
{
    m_game_over = true;
}

void Game::set_disqualified(Board &board, uint8_t player_number)
{
    board.disqualified[player_number] = true;
}

void Game::set_bomb_phase()
{
    LOG_INFO("\nbomb phase starting\n");
    m_bomb_phase = true;
}

void Game::calculate_winner(Board &board)
{
    uint16_t max = 0;
    uint8_t winner = 0;
    LOG_INFO("Scores:");

    for (uint8_t i = 0; i < board.get_player_count(); i++)
    {
        if (board.disqualified[i])
            LOG_INFO("Player " + std::to_string(i + 1) + ": disqualified");

        else
        {
            LOG_INFO("Player " + std::to_string(i + 1) + ": " + std::to_string(board.player_sets[i].count()) + " points");
            if (board.player_sets[i].count() >= max)
            {
                max = board.player_sets[i].count();
                winner = i + 1;
            }
        }
    }
    LOG_INFO("The winner is player " + std::to_string(winner) + " with " + std::to_string(max) + " points");
}

void Game::end(Board &board, uint8_t player_number)
{
    if (board.disqualified[player_number])
        LOG_WARNING("We are disqualified");

    else
        calculate_winner(board);
}

void Game::turn_request(Network &net, uint64_t &data, Map &map, Board &board, bool sorting, bool bomb_phase)
{
    if (((data >> BYTE) & FOUR_SET_BYTES) != 0)
        m_initial_time_limit = ((data >> BYTE) & FOUR_SET_BYTES);
    Timer timer(m_initial_time_limit);
    if (!bomb_phase)
        net.send_move(move_gen.generate_move(board, algorithms, timer, sorting));
    else
        net.send_move(move_gen.generate_bomb(board, map, timer));
}

void Game::receive_turn(Map &map, uint64_t &data, Board &board, bool bomb_phase)
{
    Timer timer(m_initial_time_limit);
    board.set_coord(map.two_dimension_2_one_dimension((data >> FOUR_BYTES) & ONE_SET_BYTE, (data >> TWO_BYTES) & ONE_SET_BYTE));
    board.set_spec((data >> BYTE) & ONE_SET_BYTE);
    uint8_t player = (data & ONE_SET_BYTE) - 1;
    if (!bomb_phase)
    {
        LOG_INFO("Overwrites: " + std::to_string(board.get_overwrite_stones(player)) + " | Bombs: " + std::to_string(board.get_bombs(player)));
        LOG_INFO("Player " + std::to_string((int)player + 1) + " moved to " + std::to_string((int)((data >> FOUR_BYTES) & ONE_SET_BYTE)) + ", " + std::to_string((int)((data >> TWO_BYTES) & ONE_SET_BYTE)));
        uint8_t temp_index = MAX_INDEX;
        move_exec.exec_move(player, board, temp_index);
    }
    else
    {
        LOG_INFO("Bombs: " + std::to_string(board.get_bombs(player)));
        LOG_INFO("Player " + std::to_string((int)player + 1) + " threw bomb at " + std::to_string((int)((data >> FOUR_BYTES) & ONE_SET_BYTE)) + ", " + std::to_string((int)((data >> TWO_BYTES) & ONE_SET_BYTE)));
        board = move_exec.exec_bomb(player, board, map.get_strength());
    }

#ifdef DEBUG
    board.print(player, (map.get_player_number() == player));
#endif // DEBUG
}

void Game::run(Network &net, bool sorting)
{
    Map map;
    Timer timer;
    map.read_map(net.receive_map());
    Board board = map.init_boards_and_players();
    board.print(0, false);
    move_gen = MoveGenerator(map);
    move_exec = MoveExecuter(map);
    algorithms = Algorithms(move_exec, move_gen);
    std::cout << "Time: " << timer.get_elapsed_time() << std::endl;

    while (!is_game_over() && !board.disqualified[map.get_player_number()])
    {
        uint64_t data = net.receive_data();
        switch (data >> SEVEN_BYTES)
        {
        case TYPE_RECEIVE_PLAYERNUM:
        {
            board.set_our_player(data & ONE_SET_BYTE);
            break;
        }
        case TYPE_RECEIVE_TURN_REQUEST:
        {

            turn_request(net, data, map, board, sorting, m_bomb_phase);
            break;
        }
        case TYPE_RECEIVE_PLAYER_TURN:
        {

            receive_turn(map, data, board, m_bomb_phase);
            break;
        }
        case TYPE_DISQUALIFICATION:
        {
            board.print(0, false);
            set_disqualified(board, (data & ONE_SET_BYTE) - 1);
            break;
        }
        case TYPE_PHASE1_END:
        {
            set_bomb_phase();
            break;
        }
        case TYPE_GAME_END:
        {
            set_game_over();
            break;
        }
        default:
        {
            LOG_ERROR("Unknown message type");
            LOG_ERROR("Data: " + std::to_string(data));
            exit(1);
        }
        }
    }
    end(board, map.get_player_number());
}
