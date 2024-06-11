#include "board.hpp"
#include "map.hpp"
#include "logging.hpp"

Board::Board(Map &map)
    : board_sets(),
      player_sets(map.get_player_count()),
      valid_moves(map.get_player_count()),
      wall_sets(),
      border_sets(1),
      overwrite_stones(map.get_player_count(), map.get_initial_overwrite_stones()),
      bombs(map.get_player_count(), map.get_initial_bombs()),
      communities(0),
      player_communities(map.get_player_count()),
      player_frames(map.get_player_count()),
      disqualified(map.get_player_count(), false),
      m_player_count(map.get_player_count()),
      m_num_of_fields(map.get_num_of_fields()),
      m_width(map.get_width()),
      m_height(map.get_height()),
      m_coord(0),
      m_spec(0),
      m_overwrite_move(map.get_player_count(), false),
      evaluation(0)
{
}

Board::Board(Board &board, uint16_t coord, uint8_t spec)
    : board_sets(board.board_sets),
      player_sets(board.player_sets),
      valid_moves(board.valid_moves),
      wall_sets(board.wall_sets),
      border_sets(board.border_sets),
      overwrite_stones(board.overwrite_stones),
      bombs(board.bombs),
      communities(board.communities),
      player_communities(board.player_communities),
      player_frames(board.player_frames),
      disqualified(board.disqualified),
      m_player_count(board.m_player_count),
      m_num_of_fields(board.m_num_of_fields),
      m_width(board.m_width),
      m_height(board.m_height),
      m_coord(coord),
      m_spec(spec),
      m_overwrite_move(board.m_overwrite_move),
      evaluation(board.evaluation)
{
}

Board::~Board() {}

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

uint8_t Board::get_player_count()
{
    return m_player_count;
}

uint16_t Board::get_num_of_fields()
{
    return m_num_of_fields;
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

bool Board::has_overwrite_stones(uint8_t player)
{
    return overwrite_stones[player] > 0;
}

bool Board::is_overwrite_move(uint8_t player)
{
    return m_overwrite_move[player];
}

void Board::reset_overwrite_moves()
{
    m_overwrite_move = std::vector<bool>(m_player_count, false);
}

void Board::one_dimension_2_second_dimension(uint16_t _1D_coord, uint8_t &x, uint8_t &y)
{
    _1D_coord % m_width == 0 ? x = m_width - 1 : x = _1D_coord % m_width - 1;
    y = (_1D_coord - (x + 1)) / m_width;
}

uint16_t Board::two_dimension_2_one_dimension(uint8_t x, uint8_t y)
{
    return (x + 1 + y * m_width);
}

std::string Board::get_color_string(Colors color)
{
    // #ifdef COLOR
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
    // #else
    //     return "";
    // #endif
}

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
    if (board_sets[0].test(c))
    {
        std::cout << "-";
        return true;
    }
    else if (board_sets[2].test(c))
    {
        std::cout << "i";
        return true;
    }
    else if (board_sets[3].test(c))
    {
        std::cout << "c";
        return true;
    }
    else if (board_sets[4].test(c))
    {
        std::cout << "b";
        return true;
    }
    // boards[1] is printed after i, c, b, because these fields are also 0 fields
    // and otherwise the i, c, b values would be overwritten
    else if (board_sets[1].test(c))
    {
        std::cout << "0";
        return true;
    }
    else if (board_sets[5].test(c))
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
                        // #ifdef COLOR
                        std::cout << "\033[0m";
                        // #endif
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

// For visualization of a bitset. Can probably be removed
void Board::print_bitset(std::bitset<2501> &bitset)
{
    for (uint16_t c = 1; c < m_num_of_fields; c++)
    {
        if (bitset.test(c))
        {
            std::cout << get_color_string(yellow) << "1 ";
            // #ifdef COLOR
            std::cout << "\033[0m";
            // #endif
        }
        else
            std::cout << "0 ";

        if (c % m_width == 0)
            std::cout << std::endl;
    }
    std::cout << std::endl;
}

void Board::remove_double_communities()
{
    std::vector<std::bitset<2501>> temp_communities;
    for (uint8_t i = 0; i < communities.size(); i++)
        for (uint8_t j = 0; j < communities.size(); j++)
            if (i != j && (communities[i] & communities[j]).count() != 0)
            {
                communities[i] |= communities[j];
                communities[j].reset();
            }
    for (auto &community : communities)
        if (community.count() != 0)
            temp_communities.push_back(community);

    communities = temp_communities;

    for (uint8_t p = 0; p < m_player_count; p++)
        player_communities[p].clear();

    for (auto &community : communities)
        for (uint8_t p = 0; p < m_player_count; p++)
            if ((community & player_sets[p]).count() != 0)
                player_communities[p].push_back(community);
}

void Board::reset_valid_moves(uint8_t player)
{
    for (auto &moves : valid_moves[player])
        moves.reset();
}

std::bitset<2501> Board::get_total_moves(uint8_t player)
{
    std::bitset<2501> total_moves;
    for (auto &moves : valid_moves[player])
        total_moves |= moves;
    return total_moves;
}

