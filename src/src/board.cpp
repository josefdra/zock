#include "board.hpp"
#include "map.hpp"

Board::Board(Map &map)
    : m_board_sets(),
      m_player_sets(map.get_player_count()),
      m_overwrite_stones(map.get_player_count(), map.get_initial_overwrite_stones()),
      m_bombs(map.get_player_count(), map.get_initial_bombs()),
      m_disqualified(map.get_player_count(), false),
      m_our_player(0),
      m_player_count(map.get_player_count()),
      m_num_of_fields(map.get_num_of_fields()),
      m_width(map.get_width()),
      m_height(map.get_height()),
      m_coord(0),
      evaluation(-INT32_MAX) {}

Board::Board(const Board &other)
    : m_board_sets(other.m_board_sets),
      m_player_sets(other.m_player_sets),
      m_overwrite_stones(other.m_overwrite_stones),
      m_bombs(other.m_bombs),
      m_disqualified(other.m_disqualified),
      m_our_player(other.m_our_player),
      m_player_count(other.m_player_count),
      m_num_of_fields(other.m_num_of_fields),
      m_width(other.m_width),
      m_height(other.m_height),
      m_coord(other.m_coord),
      evaluation(other.evaluation) {}

Board::Board(Board &&other) noexcept
    : m_board_sets(std::move(other.m_board_sets)),
      m_player_sets(std::move(other.m_player_sets)),
      m_overwrite_stones(std::move(other.m_overwrite_stones)),
      m_bombs(std::move(other.m_bombs)),
      m_disqualified(std::move(other.m_disqualified)),
      m_our_player(other.m_our_player),
      m_player_count(other.m_player_count),
      m_num_of_fields(other.m_num_of_fields),
      m_width(other.m_width),
      m_height(other.m_height),
      m_coord(other.m_coord),
      evaluation(other.evaluation)
{
    // Setze andere auf gültige Zustände zurück
    other.m_our_player = 0;
    other.m_player_count = 0;
    other.m_num_of_fields = 0;
    other.m_width = 0;
    other.m_height = 0;
    other.m_coord = 0;
    other.evaluation = -INT32_MAX;
}

Board &Board::operator=(const Board &other)
{
    if (this != &other)
    {
        m_board_sets = other.m_board_sets;
        m_player_sets = other.m_player_sets;
        m_overwrite_stones = other.m_overwrite_stones;
        m_bombs = other.m_bombs;
        m_disqualified = other.m_disqualified;
        m_our_player = other.m_our_player;
        m_player_count = other.m_player_count;
        m_num_of_fields = other.m_num_of_fields;
        m_width = other.m_width;
        m_height = other.m_height;
        m_coord = other.m_coord;
        evaluation = other.evaluation;
    }
    return *this;
}

Board &Board::operator=(Board &&other) noexcept
{
    if (this != &other)
    {
        m_board_sets = std::move(other.m_board_sets);
        m_player_sets = std::move(other.m_player_sets);
        m_overwrite_stones = std::move(other.m_overwrite_stones);
        m_bombs = std::move(other.m_bombs);
        m_disqualified = std::move(other.m_disqualified);
        m_our_player = other.m_our_player;
        m_player_count = other.m_player_count;
        m_num_of_fields = other.m_num_of_fields;
        m_width = other.m_width;
        m_height = other.m_height;
        m_coord = other.m_coord;
        evaluation = other.evaluation;

        // Setze andere auf gültige Zustände zurück
        other.m_our_player = 0;
        other.m_player_count = 0;
        other.m_num_of_fields = 0;
        other.m_width = 0;
        other.m_height = 0;
        other.m_coord = 0;
        other.evaluation = -INT32_MAX;
    }
    return *this;
}

Board::~Board() = default;

void Board::set_coord(uint16_t coord)
{
    m_coord = coord;
}

void Board::set_disqualified(uint8_t player)
{
    m_disqualified[player] = true;
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

uint16_t Board::get_bombs(uint8_t player)
{
    return m_bombs[player];
}

std::vector<uint16_t> &Board::get_all_bombs()
{
    return m_bombs;
}

uint16_t Board::get_coord()
{
    return m_coord;
}

uint8_t Board::get_player_num()
{
    return m_our_player;
}

int Board::get_evaluation()
{
    return evaluation;
}

std::vector<bool> &Board::get_disqualified()
{
    return m_disqualified;
}

std::bitset<2501> &Board::get_board_set(uint8_t set)
{
    return m_board_sets[set];
}

std::array<std::bitset<2501>, 7> &Board::get_board_sets()
{
    return m_board_sets;
}

std::bitset<2501> &Board::get_player_set(uint8_t player)
{
    return m_player_sets[player];
}

std::vector<std::bitset<2501>> &Board::get_player_sets()
{
    return m_player_sets;
}

bool Board::is_disqualified(uint8_t player)
{
    return m_disqualified[player];
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
#ifdef COLOR
    switch (color)
    {
    case orange:
        return "\e[38;5;208m";
    case red:
        return "\e[91m";
    case green:
        return "\e[92m";
    case yellow:
        return "\e[93m";
    case blue:
        return "\e[94m";
    case magenta:
        return "\e[95m";
    case cyan:
        return "\e[96m";
    case dark_black:
        return "\e[30m";
    default:
        return "\e[37m";
    }
#else
    return "";
#endif
}

void Board::print_upper_outlines()
{
    std::cout << "     ";
    for (uint16_t i = 0; i < get_width(); i++)
    {
        std::cout << std::setw(2) << i;
    }
    std::cout << std::endl
              << "    /";
    for (uint16_t i = 0; i < get_width(); i++)
    {
        std::cout << "--";
    }
    std::cout << std::endl;
}

bool Board::print_m_board_sets(uint16_t c)
{
    if (m_board_sets[0].test(c))
    {
        std::cout << "-";
        return true;
    }
    else if (m_board_sets[2].test(c))
    {
        std::cout << "i";
        return true;
    }
    else if (m_board_sets[3].test(c))
    {
        std::cout << "c";
        return true;
    }
    else if (m_board_sets[4].test(c))
    {
        std::cout << "b";
        return true;
    }
    // boards[1] is printed after i, c, b, because these fields are also 0 fields
    // and otherwise the i, c, b values would be overwritten
    else if (m_board_sets[1].test(c))
    {
        std::cout << "0";
        return true;
    }
    else if (m_board_sets[5].test(c))
    {
        std::cout << "x";
        return true;
    }
    return false;
}

// For visualization of a bitset. Can probably be removed
void Board::print_bitset(std::bitset<2501> &bitset)
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
        {
            std::cout << "0 ";
        }
        if (c % m_width == 0)
        {
            std::cout << std::endl;
        }
    }
    std::cout << std::endl;
}
