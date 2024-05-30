#include "game.hpp"
#include "map.hpp"
#include "network.hpp"
#include "move_generator.hpp"
#include "move_executer.hpp"
#include "board.hpp"
#include "timer.hpp"

Game::Game() {}

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
    std::cout << std::endl
              << "bomb phase starting" << std::endl
              << std::endl;
    m_bomb_phase = true;
}

void Game::calculate_winner(Board &board)
{
    uint16_t max = 0;
    uint16_t winner = 0;
    std::cout << "Scores:" << std::endl;

    for (uint16_t i = 0; i < board.get_player_count(); i++)
    {
        std::cout << "Player " << i + 1 << ": ";
        if (board.disqualified[i])
            std::cout << "disqualified" << std::endl;
        else
        {
            std::cout << board.player_sets[i].count() << " points" << std::endl;
        }
        if (board.player_sets[i].count() > max)
        {
            max = board.player_sets[i].count();
            winner = i + 1;
        }
    }
    std::cout << "The winner is player " << winner << " with " << max << " points" << std::endl;
}

void Game::end(Board &board, uint8_t player_number)
{
    if (board.disqualified[player_number])
    {
        std::cout << "We are disqualified" << std::endl;
    }
    else
    {
        calculate_winner(board);
    }
}

void Game::turn_request(Network &net, uint64_t &data, Map &map, Board &board, bool sorting, bool bomb_phase)
{
    if (((data >> 8) & 0xFFFFFFFF) != 0)
    {
        m_initial_time_limit = ((data >> 8) & 0xFFFFFFFF);
    }
    Timer timer(m_initial_time_limit);
    uint8_t search_depth = data & 0xFF;
    MoveGenerator move_gen(map);
    if (!bomb_phase)
        net.send_move(move_gen.generate_move(board, map, timer, search_depth, sorting));
    else
        net.send_move(move_gen.generate_bomb(board, map, timer, search_depth, sorting));
}

void Game::receive_turn(Map &map, uint64_t &data, Board &board, bool bomb_phase)
{
    board.set_coord(map.two_dimension_2_one_dimension((data >> 32) & 0xFF, (data >> 16) & 0xFF));
    board.set_spec((data >> 8) & 0xFF);
    uint8_t player = (data & 0xFF) - 1;
    MoveExecuter move_exec(map);
    Timer timer(m_initial_time_limit);
    if (!bomb_phase)
    {
        std::cout << "Overwrites: " << board.get_overwrite_stones(player) << " | Bombs: " << board.get_bombs(player) << std::endl;
        std::cout << "Player " << (int)player + 1 << " moved to " << (int)((data >> 32) & 0xFF) << ", " << (int)((data >> 16) & 0xFF) << std::endl;
        board = move_exec.exec_move(player, board, timer);
    }
    else
    {
        std::cout << "Bombs: " << board.get_bombs(player) << std::endl;
        std::cout << "Player " << (int)player + 1 << " threw bomb at " << (int)((data >> 32) & 0xFF) << ", " << (int)((data >> 16) & 0xFF) << std::endl;
        board = move_exec.exec_bomb(player, board, timer);
    }
    board.print(player, (map.get_player_number() == player));
}

void Game::run(Network &net, bool sorting)
{
    Map map;
    map.read_map(net.receive_map());
    Board board = map.init_boards_and_players();
    map.init_bomb_phase_boards();
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
            set_disqualified(board, (data & 0xFF) - 1);
            break;
        }
        case TYPE_PHASE1_END:
        {
            board = Board(board, map.fields_to_remove);
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
            std::cout << "Unknown message type" << std::endl;
            std::cout << "Data: " << data << std::endl;
            exit(1);
        }
        }
    }
    end(board, map.get_player_number());
}