#include "game.hpp"
#include "map.hpp"
#include "network.hpp"
#include "move_generator.hpp"
#include "move_executer.hpp"
#include "move_board.hpp"
#include "timer.hpp"
#include "bomb_board.hpp"
#include "board.hpp"

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

void Game::calculate_winner(Board &board)
{
    uint16_t max = 0;
    uint16_t winner = 0;
    std::cout << "Scores:" << std::endl;

    for (uint16_t i = 0; i < board.get_player_count(); i++)
    {
        std::cout << "Player " << i + 1 << ": ";
        if (board.is_disqualified(i))
            std::cout << "disqualified" << std::endl;
        else
        {
            std::cout << board.get_player_set(i).count() << " points" << std::endl;
        }
        if (board.get_player_set(i).count() > max)
        {
            max = board.get_player_set(i).count();
            winner = i + 1;
        }
    }
    std::cout << "The winner is player " << winner << " with " << max << " points" << std::endl;
}

void Game::end(Board board)
{
    if (board.is_disqualified(board.get_player_num()))
    {
        std::cout << "We are disqualified" << std::endl;
    }
    else
    {
        calculate_winner(board);
    }
}

void Game::move_request(Network &net, uint64_t &data, Map &map, MoveBoard &move_board, bool sorting)
{
    if (((data >> 8) & 0xFFFFFFFF) != 0)
    {
        m_initial_time_limit = ((data >> 8) & 0xFFFFFFFF);
    }
    Timer timer(m_initial_time_limit);
    uint8_t search_depth = data & 0xFF;
    MoveGenerator move_gen(map);
    net.send_move(move_gen.generate_move(move_board, map, timer, search_depth, sorting));
}

void Game::bomb_request(Network &net, uint64_t &data, Map &map, BombBoard &bomb_board, bool sorting)
{
    if (((data >> 8) & 0xFFFFFFFF) != 0)
    {
        m_initial_time_limit = ((data >> 8) & 0xFFFFFFFF);
    }
    Timer timer(m_initial_time_limit);
    uint8_t search_depth = data & 0xFF;
    MoveGenerator move_gen(map);
    net.send_move(move_gen.generate_bomb(bomb_board, map, timer, search_depth, sorting));
}

void Game::receive_move(Map &map, uint64_t &data, MoveBoard &move_board)
{
    move_board.set_coord(map.two_dimension_2_one_dimension((data >> 32) & 0xFF, (data >> 16) & 0xFF));
    move_board.set_spec((data >> 8) & 0xFF);
    uint8_t player = (data & 0xFF) - 1;
    MoveExecuter move_exec(map);
    Timer timer(m_initial_time_limit);
    std::cout << "Overwrites: " << move_board.get_overwrite_stones(player) << " | Bombs: " << move_board.get_bombs(player) << std::endl;
    std::cout << "Player " << (int)player + 1 << " moved to " << (int)((data >> 32) & 0xFF) << ", " << (int)((data >> 16) & 0xFF) << std::endl;
    move_board = move_exec.exec_move(player, move_board, timer);
    move_board.print(player);
}

void Game::receive_bomb(Map &map, uint64_t &data, BombBoard &bomb_board)
{
    bomb_board.set_coord(map.two_dimension_2_one_dimension((data >> 32) & 0xFF, (data >> 16) & 0xFF));
    uint8_t player = (data & 0xFF) - 1;
    MoveExecuter move_exec(map);
    Timer timer(m_initial_time_limit);
    std::cout << "Bombs: " << bomb_board.get_bombs(player) << std::endl;
    std::cout << "Player " << (int)player + 1 << " threw bomb at " << (int)((data >> 32) & 0xFF) << ", " << (int)((data >> 16) & 0xFF) << std::endl;
    bomb_board = move_exec.exec_bomb(player, bomb_board, timer);
    bomb_board.print();
}

void Game::run(Network &net, bool sorting)
{
    Map map;
    map.read_map(net.receive_map());
    MoveBoard move_board = map.init_boards_and_players();
    BombBoard bomb_board(map);

    while (!is_game_over() && !move_board.is_disqualified(map.get_player_number()) && !move_board.is_disqualified(map.get_player_number()))
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
            if (!is_bomb_phase())
                move_request(net, data, map, move_board, sorting);
            else
                bomb_request(net, data, map, bomb_board, sorting);
            break;
        }
        case TYPE_RECEIVE_PLAYER_TURN:
        {
            if (!is_bomb_phase())
                receive_move(map, data, move_board);
            else
                receive_bomb(map, data, bomb_board);
            break;
        }
        case TYPE_DISQUALIFICATION:
        {
            if (!is_bomb_phase())
                move_board.set_disqualified((data & 0xFF) - 1);
            else
                bomb_board.set_disqualified((data & 0xFF) - 1);
            break;
        }
        case TYPE_PHASE1_END:
        {
            m_bomb_phase = true;
            bomb_board.set_values(move_board);
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
    if (!is_bomb_phase())
        end(Board(move_board));
    else
        end(Board(bomb_board));
}