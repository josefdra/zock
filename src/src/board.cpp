#include "board.hpp"
#include "initializer.hpp"
#include "logging.hpp"

/**
 * @brief this cpp contains all needed information about the current board state
 *
 */

Board::Board(Initializer &init)
    : board_sets(),
      player_sets(init.get_player_count()),
      valid_moves(init.get_player_count()),
      static_evaluation(init.get_num_of_fields(), 0),
      fixed_protected_fields(),
      protected_fields(init.get_player_count()),
      before_protected_fields(init.get_player_count()),
      corners_and_walls(),
      overwrite_stones(init.get_player_count(), init.get_initial_overwrite_stones()),
      bombs(init.get_player_count(), init.get_initial_bombs()),
      communities(0),
      frames(0),
      num_of_players_in_community(0),
      start_end_communities(0),
      start_end_frames(0),
      disqualified(init.get_player_count(), false),
      before_bonus_fields(),
      before_choice_fields(),
      special_moves(),
      special_coords{0, 0, 0},
      m_our_player(0),
      m_player_count(init.get_player_count()),
      m_num_of_fields(init.get_num_of_fields()),
      m_num_of_not_minus_fields(init.get_num_of_fields() - 1),
      m_width(init.get_width()),
      m_height(init.get_height()),
      m_coord(0),
      m_spec(0),
      m_overwrite_move(init.get_player_count(), false),
      evaluation(0),
      final_state(false),
      m_bonus_field(false),
      m_choice_field(false)
{
}

Board::Board(Board &board, uint16_t coord, uint8_t spec)
    : board_sets(board.board_sets),
      player_sets(board.player_sets),
      valid_moves(board.valid_moves),
      static_evaluation(board.static_evaluation),
      fixed_protected_fields(board.fixed_protected_fields),
      protected_fields(board.protected_fields),
      before_protected_fields(board.before_protected_fields),
      corners_and_walls(board.corners_and_walls),
      overwrite_stones(board.overwrite_stones),
      bombs(board.bombs),
      communities(board.communities),
      frames(board.frames),
      num_of_players_in_community(board.num_of_players_in_community),
      start_end_communities(board.start_end_communities),
      start_end_frames(board.start_end_frames),
      disqualified(board.disqualified),
      before_bonus_fields(board.before_bonus_fields),
      before_choice_fields(board.before_choice_fields),
      special_moves(board.special_moves),
      special_coords(board.special_coords),
      m_our_player(board.m_our_player),
      m_player_count(board.m_player_count),
      m_num_of_fields(board.m_num_of_fields),
      m_num_of_not_minus_fields(board.m_num_of_not_minus_fields),
      m_width(board.m_width),
      m_height(board.m_height),
      m_coord(coord),
      m_spec(spec),
      m_overwrite_move(board.m_overwrite_move),
      evaluation(board.evaluation),
      final_state(board.final_state),
      m_bonus_field(board.m_bonus_field),
      m_choice_field(board.m_choice_field)
{
}

Board::~Board() {}

/**
 *
 * HERE ARE JUST SETTER AND GETTER
 *
 */

void Board::set_our_player(uint8_t player)
{
    m_our_player = player - 1;
    LOG_INFO("Our player: " + std::to_string(m_our_player + 1));
}

void Board::set_coord(uint16_t coord)
{
    m_coord = coord;
}

void Board::set_spec(uint8_t spec)
{
    m_spec = spec;
}

void Board::set_overwrite_stones(uint8_t player, uint16_t _overwrite_stones)
{
    overwrite_stones[player] = _overwrite_stones;
}

void Board::set_bombs(uint8_t player, uint16_t _bombs)
{
    bombs[player] = _bombs;
}

void Board::set_overwrite_move(uint8_t player)
{
    m_overwrite_move[player] = true;
}

void Board::set_final_state()
{
    final_state = true;
}

void Board::set_bonus_field()
{
    m_bonus_field = true;
}

void Board::set_choice_field()
{
    m_choice_field = true;
}

uint8_t Board::get_our_player()
{
    return m_our_player;
}

uint8_t Board::get_player_count()
{
    return m_player_count;
}

uint16_t Board::get_num_of_fields()
{
    return m_num_of_fields;
}

uint16_t Board::get_num_of_not_minus_fields()
{
    return m_num_of_not_minus_fields;
}

uint8_t Board::get_width()
{
    return m_width;
}

uint8_t Board::get_height()
{
    return m_height;
}

uint16_t Board::get_overwrite_stones(uint8_t player)
{
    return overwrite_stones[player];
}

uint16_t Board::get_bombs(uint8_t player)
{
    return bombs[player];
}

uint16_t Board::get_coord()
{
    return m_coord;
}

uint8_t Board::get_spec()
{
    return m_spec;
}

int Board::get_evaluation()
{
    return evaluation;
}

uint8_t Board::get_num_of_communities()
{
    return communities.size();
}

void Board::increment_overwrite_stones(uint8_t player)
{
    overwrite_stones[player]++;
}

void Board::increment_bombs(uint8_t player)
{
    bombs[player]++;
}

void Board::decrement_overwrite_stones(uint8_t player)
{
    overwrite_stones[player]--;
}

void Board::decrement_bombs(uint8_t player)
{
    bombs[player]--;
}

void Board::decrement_not_minus_fields()
{
    m_num_of_not_minus_fields--;
}

bool Board::has_overwrite_stones(uint8_t player)
{
    return overwrite_stones[player] > 0;
}

bool Board::is_overwrite_move(uint8_t player)
{
    return m_overwrite_move[player];
}

bool Board::is_final_state()
{
    return final_state;
}

bool Board::check_bonus_field()
{
    return m_bonus_field;
}

bool Board::check_choice_field()
{
    return m_choice_field;
}

void Board::reset_overwrite_moves()
{
    m_overwrite_move = std::vector<bool>(m_player_count, false);
}

void Board::reset_bonus_field()
{
    m_bonus_field = false;
}

void Board::reset_choice_field()
{
    m_choice_field = false;
}

/// @brief calcualtes our dimension layout to the dimension used by the server
/// @param _1D_coord 1D coordinate
/// @param x x value to set
/// @param y y value to set
void Board::one_dimension_2_second_dimension(uint16_t _1D_coord, uint8_t &x, uint8_t &y)
{
    _1D_coord % m_width == 0 ? x = m_width - 1 : x = _1D_coord % m_width - 1;
    y = (_1D_coord - (x + 1)) / m_width;
}

/// @brief calculates server dimension to our dimension
/// @param x x coordinate
/// @param y y coordinate
/// @return 1D coordinate used by our program
uint16_t Board::two_dimension_2_one_dimension(uint8_t x, uint8_t y)
{
    return (x + 1 + y * m_width);
}

/// @brief is used to get coloured prints for players in local terminal
/// @param color enum of the used color
/// @return escape encoding of needed color
std::string Board::get_color_string(Colors color)
{
#ifdef COLOR
    switch (color)
    {
    case orange:
        return "\033[38;5;208m";
    case red:
        return "\033[91m";
    case green:
        return "\033[92m";
    case yellow:
        return "\033[93m";
    case blue:
        return "\033[94m";
    case magenta:
        return "\033[95m";
    case cyan:
        return "\033[96m";
    case black:
        return "\033[90m";
    default:
        return "\033[37m";
    }
#else
    return "";
#endif
}

/**
 *
 * HERE ARE JUST FUNCTIONS THAT ARE NEEDED TO PRINT THE CURRENT BOARD IN TERMINAL
 *
 */
void Board::print_upper_outlines()
{
    std::cout << "     ";
    for (uint16_t i = 0; i < get_width(); i++)
        std::cout << std::setw(2) << i;

    std::cout << std::endl
              << "    /";
    for (uint16_t i = 0; i < get_width(); i++)
        std::cout << "--";

    std::cout << std::endl;
}

bool Board::print_board_sets(uint16_t c)
{
    if (board_sets[MINUS].test(c))
    {
        std::cout << "-";
        return true;
    }
    else if (board_sets[I].test(c))
    {
        std::cout << "i";
        return true;
    }
    else if (board_sets[C].test(c))
    {
        std::cout << "c";
        return true;
    }
    else if (board_sets[B].test(c))
    {
        std::cout << "b";
        return true;
    }
    // boards[1] is printed after i, c, b, because these fields are also 0 fields
    // and otherwise the i, c, b values would be overwritten
    else if (board_sets[EMPTY].test(c))
    {
        std::cout << "0";
        return true;
    }
    else if (board_sets[X].test(c))
    {
        std::cout << "x";
        return true;
    }
    return false;
}

void Board::print(uint8_t player, bool our_player)
{
    print_upper_outlines();
    for (uint16_t y = 0; y < get_height(); y++)
    {
        std::cout << " " << std::setw(2) << y << " | ";
        for (uint16_t x = 0; x < get_width(); x++)
        {
            uint16_t c = two_dimension_2_one_dimension(x, y);
            if (!print_board_sets(c))
                for (uint16_t i = 0; i < m_player_count; i++)
                    if (player_sets[i].test(c))
                    {
                        std::cout << get_color_string(Colors(i + 1)) << (uint16_t)(i + 1);
#ifdef COLOR
                        std::cout << "\033[0m";
#endif
                    }
            if (our_player && get_total_moves(player).test(c))
                std::cout << "'";

            else
                std::cout << " ";
        }
        std::cout << std::endl;
    }
    LOG_INFO("Calculated valid moves: " + std::to_string(get_total_moves(player).count()));
    std::cout << std::endl;
}

void Board::print_bitset(std::bitset<MAX_NUM_OF_FIELDS> &bitset)
{
    for (uint16_t c = 1; c < m_num_of_fields; c++)
    {
        if (bitset.test(c))
        {
            std::cout << get_color_string(yellow) << "1 ";
#ifdef COLOR
            std::cout << "\033[0m";
#endif
        }
        else
            std::cout << "0 ";

        if (c % m_width == 0)
            std::cout << std::endl;
    }
    std::cout << std::endl;
}

void Board::reset_valid_moves(uint8_t player)
{
    for (auto &moves : valid_moves[player])
        moves.reset();
}

/// @brief adds valid moves of set with - or without overwrite stones to a total_moves set
/// @param player current player
/// @return total_moves with and without overwrite stones
std::bitset<MAX_NUM_OF_FIELDS> Board::get_total_moves(uint8_t player)
{
    std::bitset<MAX_NUM_OF_FIELDS> total_moves;
    for (auto &moves : valid_moves[player])
        total_moves |= moves;

    return total_moves;
}

/// @brief calculates the percentage of the map that is already occupied and can't be played anymore (doesn't include overwrite stones - is not necessary)
/// @param possible_fields actual fields than can initially be put player stones on
void Board::calc_occupied_percentage(uint16_t possible_fields)
{
    occupied_percentage = (occupied_fields * 100) / possible_fields;
}

/// @brief calculates the scaling factor according to total size of map and actual playable fields
/// @param playable_fields
void Board::calculate_scaling_factor(uint16_t playable_fields)
{
    LOG_INFO("playable fields: " + std::to_string(playable_fields));
    scaling_factor = double(playable_fields) / double(MAX_NUM_OF_FIELDS);
    // Apply a square root scaling for a more gradual change
    std::sqrt(scaling_factor);
}