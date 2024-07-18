#include "game.hpp"
#include "initializer.hpp"
#include "network.hpp"
#include "board.hpp"
#include "timer.hpp"
#include "logging.hpp"
#include "statistics.hpp"

/**
 *
 * @brief this cpp handles the whole logic around the game and is needed to receive and send all types of information between server and client
 *
 */

/**
 *
 * HERE ARE JUST INITIALIZATIONS AND SETTER AND GETTER
 *
 */

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
    LOG_INFO("Player " + std::to_string(player_number + 1) + " is disqualified");
}

void Game::set_bomb_phase()
{
    LOG_INFO("Bomb phase starting\n");
    m_bomb_phase = true;
}

/// @brief prints each players status at the end of the game either if they're disqualified or their points
/// @param board current board layout - end state board
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

/// @brief handles prints at the end of the game
/// @param board current board layout
/// @param player_number our player number
void Game::end(Board &board, uint8_t player_number)
{
    if (board.disqualified[player_number])
        LOG_WARNING("We are disqualified");

    else
        calculate_winner(board);

    // print_total_time_statistics();
    // print_total_evaluation_statistics();
}

/// @brief handles information exchange if we've got a turn request
/// @param net network object to send move to
/// @param data includes message of data sent by the server
/// @param init current initializer
/// @param board current board layout
/// @param sorting bool if sorting is enabled or disabled
/// @param bomb_phase bool if we're at bomb phase or not
void Game::turn_request(Network &net, uint64_t &data, Initializer &init, Board &board, bool sorting, bool bomb_phase)
{
    if (((data >> BYTE) & FOUR_SET_BYTES) != 0)
        m_initial_time_limit = ((data >> BYTE) & FOUR_SET_BYTES);
    Timer timer(m_initial_time_limit);

    if (!bomb_phase)
    {
        if ((board.special_moves.size() > 2 && board.board_sets[X].test(board.special_coord)) || (board.special_moves.size() > 1 && board.player_sets[board.get_our_player()].test(board.special_coord)))
        {
            net.send_move(board.special_moves.back());
            board.special_moves.pop_back();
        }
        else
            net.send_move(move_gen.generate_move(board, algorithms, timer, sorting));
    }
    else
    {
        if (board.special_moves.size() > 0)
        {
            net.send_move(board.special_moves.back());
            board.special_moves.pop_back();
        }
        else
            net.send_move(move_gen.generate_bomb(board, init, timer));
    }
}

/// @brief handles a executed move sent by the server and sends the information deeper to execute the move locally
/// @param init current initializer
/// @param data contains message sent by the server (which player did which move)
/// @param board current board layout
/// @param bomb_phase bool if we're in bomb_phase
void Game::receive_turn(Initializer &init, uint64_t &data, Board &board, bool bomb_phase)
{
    Timer timer(m_initial_time_limit);
    board.set_coord(init.two_dimension_2_one_dimension((data >> FOUR_BYTES) & ONE_SET_BYTE, (data >> TWO_BYTES) & ONE_SET_BYTE));
    board.set_spec((data >> BYTE) & ONE_SET_BYTE);
    uint8_t player = (data & ONE_SET_BYTE) - 1;
    board.calc_occupied_percentage(init.get_sum_possible_fields());
    // LOG_INFO("Current map progress: " + std::to_string(board.occupied_percentage) + "% " + "of fields occupied");
    if (!bomb_phase)
    {
        // LOG_INFO("Overwrites: " + std::to_string(board.get_overwrite_stones(player)) + " | Bombs: " + std::to_string(board.get_bombs(player)));
        // LOG_INFO("Player " + std::to_string((int)player + 1) + " moved to " + std::to_string((int)((data >> FOUR_BYTES) & ONE_SET_BYTE)) + ", " + std::to_string((int)((data >> TWO_BYTES) & ONE_SET_BYTE)));
        uint8_t temp_index = MAX_INDEX;
        move_exec.exec_move(player, board, temp_index);
        board.occupied_fields++;
    }
    else
    {
        // LOG_INFO("Bombs: " + std::to_string(board.get_bombs(player)));
        // LOG_INFO("Player " + std::to_string((int)player + 1) + " threw bomb at " + std::to_string((int)((data >> FOUR_BYTES) & ONE_SET_BYTE)) + ", " + std::to_string((int)((data >> TWO_BYTES) & ONE_SET_BYTE)));
        board = move_exec.exec_bomb(player, board, init.get_strength());
    }

    if (!board.corners_and_walls.test(board.get_coord()))
        board.static_evaluation[board.get_coord()] = 0;

    move_exec.calculate_before_protected_fields(board, player);

#ifdef DEBUG
    board.print(player, (board.get_our_player() == player));
#endif // DEBUG
}

void Game::print_static_evaluation(Board &board)
{
    for (uint16_t c = 1; c < board.get_num_of_fields(); c++)
    {
        std::cout << std::setw(4) << board.static_evaluation[c] << " ";

        if (c % board.get_width() == 0)
            std::cout << std::endl;
    }
    std::cout << std::endl;
}

/// @brief handles whole information exchange and is a while loop which only ends if the game is over or we're disqualified
/// @param net network object
/// @param sorting bool if sorting is enabled/disabled
void Game::run(Network &net, bool sorting)
{
    Initializer init;
    init.read_map(net.receive_map());
    Timer board_setup_timer;
    Board board = init.init_boards_and_players();
    // LOG_INFO("Board setup time: " + std::to_string(board_setup_timer.get_elapsed_time()));
    board.print(0, false);
    Timer move_gen_and_exec_setup_timer;
    move_gen = MoveGenerator(init);
    move_exec = MoveExecuter(init);
    // LOG_INFO("Move generator and executer setup time: " + std::to_string(move_gen_and_exec_setup_timer.get_elapsed_time()));
    board.calculate_scaling_factor(init.get_sum_possible_fields());
    // LOG_INFO("current scaling factor: " + std::to_string(board.scaling_factor) + "\n");
    print_static_evaluation(board);

    while (!is_game_over() && !board.disqualified[board.get_our_player()])
    {
        uint64_t data = net.receive_data();
        switch (data >> SEVEN_BYTES)
        {
        case TYPE_RECEIVE_PLAYERNUM:
        {
            board.set_our_player(data & ONE_SET_BYTE);
            Timer algorithms_setup_timer;
            algorithms = Algorithms(move_exec, move_gen);
            // LOG_INFO("Algorithms setup time: " + std::to_string(algorithms_setup_timer.get_elapsed_time()));
            break;
        }
        case TYPE_RECEIVE_TURN_REQUEST:
        {
            turn_request(net, data, init, board, sorting, m_bomb_phase);
            break;
        }
        case TYPE_RECEIVE_PLAYER_TURN:
        {

            receive_turn(init, data, board, m_bomb_phase);
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
            // LOG_ERROR("Unknown message type");
            // LOG_ERROR("Data: " + std::to_string(data));
            exit(1);
        }
        }
    }
    end(board, board.get_our_player());
}
