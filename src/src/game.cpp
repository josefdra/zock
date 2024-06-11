#include "game.hpp"
#include "map.hpp"
#include "network.hpp"
#include "move_generator.hpp"
#include "move_executer.hpp"
#include "board.hpp"
#include "timer.hpp"
#include "logging.hpp"

Game::Game()
{
    m_game_over = false;
    m_bomb_phase = false;
    m_initial_time_limit = 1000000;
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
    uint16_t winner = 0;
    LOG_INFO("Scores:");

    for (uint16_t i = 0; i < board.get_player_count(); i++)
    {
        if (board.disqualified[i])
            LOG_INFO("Player " + std::to_string(i + 1) + ": disqualified");

        else
            LOG_INFO("Player " + std::to_string(i + 1) + ": " + std::to_string(board.player_sets[i].count()) + " points");

        if (board.player_sets[i].count() > max)
        {
            max = board.player_sets[i].count();
            winner = i + 1;
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

    if (((data >> 8) & 0xFFFFFFFF) != 0)
        m_initial_time_limit = ((data >> 8) & 0xFFFFFFFF);

    Timer timer(m_initial_time_limit);
    // uint8_t search_depth = data & 0xFF;
    MoveGenerator move_gen(map);
    if (!bomb_phase)
        net.send_move(move_gen.generate_move(board, map, timer, sorting));
    else
        net.send_move(move_gen.generate_bomb(board, map, timer));
}

void Game::receive_turn(Map &map, uint64_t &data, Board &board, bool bomb_phase)
{
    Timer timer(m_initial_time_limit);
    board.set_coord(map.two_dimension_2_one_dimension((data >> 32) & 0xFF, (data >> 16) & 0xFF));
    board.set_spec((data >> 8) & 0xFF);
    uint8_t player = (data & 0xFF) - 1;
    MoveExecuter move_exec(map);
    if (!bomb_phase)
    {
        LOG_INFO("Overwrites: " + std::to_string(board.get_overwrite_stones(player)) + " | Bombs: " + std::to_string(board.get_bombs(player)));
        LOG_INFO("Player " + std::to_string((int)player + 1) + " moved to " + std::to_string((int)((data >> 32) & 0xFF)) + ", " + std::to_string((int)((data >> 16) & 0xFF)));
        move_exec.exec_move(player, board);
    }
    else
    {
        LOG_INFO("Bombs: " + std::to_string(board.get_bombs(player)));
        LOG_INFO("Player " + std::to_string((int)player + 1) + " threw bomb at " + std::to_string((int)((data >> 32) & 0xFF)) + ", " + std::to_string((int)((data >> 16) & 0xFF)));
        board = move_exec.exec_bomb(player, board, map.get_strength());
    }
    std::cout << "Frames:" << std::endl;
    for (auto &frame : board.player_frames[player])
        board.print_bitset(frame);
    std::cout << "Communities:" << std::endl;
    for (auto &community : board.player_communities[player])
        board.print_bitset(community);
    

#ifdef DEBUG
    board.print(player, (map.get_player_number() == player));
#endif // DEBUG
}

void Game::run(Network &net, bool sorting)
{
    Map map;
    map.read_map(net.receive_map());
    Board board = map.init_boards_and_players();
    board.print(0, false);

    while (!is_game_over() && !board.disqualified[map.get_player_number()])
    {
        uint64_t data = net.receive_data();
        switch (data >> 56)
        {
        case TYPE_RECEIVE_PLAYERNUM:
        {
            map.set_player_number(data & 0xFF);
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
            LOG_INFO("Board print 0, false");
            set_disqualified(board, (data & 0xFF) - 1);
            break;
        }
        case TYPE_PHASE1_END:
        {
            // board = Board(board, map.fields_to_remove);
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