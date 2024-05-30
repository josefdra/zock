#include "board.hpp"
#include "map.hpp"
#include "move_board.hpp"

MoveBoard::MoveBoard(MoveBoard &move_board, uint16_t coord, uint8_t spec) : Board(move_board)
{
    *this = move_board;
    m_coord = coord;
    m_spec = spec;
}

MoveBoard::MoveBoard(const MoveBoard &other)
    : Board(other),
      m_spec(other.m_spec),
      m_overwrite_move(other.m_overwrite_move),
      m_valid_moves(other.m_valid_moves),
      m_wall_sets(other.m_wall_sets),
      m_border_sets(other.m_border_sets),
      m_protected_fields(other.m_protected_fields) {}

MoveBoard::MoveBoard(const Board &other)
    : Board(other),
      m_spec(0),
      m_overwrite_move(std::vector<bool>(m_player_count, false)),
      m_valid_moves(std::vector<std::bitset<2501>>(m_player_count)),
      m_wall_sets(std::array<std::bitset<2501>, 8>()),
      m_border_sets(std::vector<std::bitset<2501>>(1)),
      m_protected_fields(std::vector<std::bitset<2501>>()) {}

MoveBoard::MoveBoard(MoveBoard &&other) noexcept
    : Board(std::move(other)),
      m_spec(std::move(other.m_spec)),
      m_overwrite_move(std::move(other.m_overwrite_move)),
      m_valid_moves(std::move(other.m_valid_moves)),
      m_wall_sets(std::move(other.m_wall_sets)),
      m_border_sets(std::move(other.m_border_sets)),
      m_protected_fields(std::move(other.m_protected_fields)) {}

MoveBoard &MoveBoard::operator=(const MoveBoard &other)
{
    if (this != &other)
    {
        Board::operator=(other);
        m_spec = other.m_spec;
        m_overwrite_move = other.m_overwrite_move;
        m_valid_moves = other.m_valid_moves;
        m_wall_sets = other.m_wall_sets;
        m_border_sets = other.m_border_sets;
        m_protected_fields = other.m_protected_fields;
    }
    return *this;
}

MoveBoard &MoveBoard::operator=(MoveBoard &&other) noexcept
{
    if (this != &other)
    {
        Board::operator=(std::move(other));
        m_spec = std::move(other.m_spec);
        m_overwrite_move = std::move(other.m_overwrite_move);
        m_valid_moves = std::move(other.m_valid_moves);
        m_wall_sets = std::move(other.m_wall_sets);
        m_border_sets = std::move(other.m_border_sets);
        m_protected_fields = std::move(other.m_protected_fields);
    }
    return *this;
}

MoveBoard::~MoveBoard() {}

void MoveBoard::set_spec(uint8_t spec)
{
    m_spec = spec;
}

void MoveBoard::set_overwrite_stones(uint8_t player, uint16_t _m_overwrite_stones)
{
    m_overwrite_stones[player] = _m_overwrite_stones;
}

void MoveBoard::set_bombs(uint8_t player, uint16_t bombs)
{
    m_bombs[player] = bombs;
}

void MoveBoard::set_overwrite_move(uint8_t player)
{
    m_overwrite_move[player] = true;
}

uint16_t MoveBoard::get_overwrite_stones(uint8_t player)
{
    return m_overwrite_stones[player];
}

uint8_t MoveBoard::get_spec()
{
    return m_spec;
}

uint8_t MoveBoard::get_border_set_size()
{
    return m_border_sets.size();
}

std::bitset<2501> &MoveBoard::get_valid_moves(uint8_t player)
{
    return m_valid_moves[player];
}

std::bitset<2501> &MoveBoard::get_wall_set(uint8_t index)
{
    return m_wall_sets[index];
}

std::bitset<2501> &MoveBoard::get_border_set(uint8_t player)
{
    return m_border_sets[player];
}

std::vector<std::bitset<2501>> &MoveBoard::get_border_sets()
{
    return m_border_sets;
}

void MoveBoard::increment_overwrite_stones(uint8_t player)
{
    m_overwrite_stones[player]++;
}

void MoveBoard::increment_bombs(uint8_t player)
{
    m_bombs[player]++;
}

void MoveBoard::decrement_overwrite_stones(uint8_t player)
{
    m_overwrite_stones[player]--;
}

void MoveBoard::decrement_bombs(uint8_t player)
{
    m_bombs[player]--;
}

bool MoveBoard::has_overwrite_stones(uint8_t player)
{
    return m_overwrite_stones[player] > 0;
}

bool MoveBoard::is_overwrite_move(uint8_t player)
{
    return m_overwrite_move[player];
}

void MoveBoard::reset_overwrite_moves()
{
    m_overwrite_move = std::vector<bool>(m_player_count, false);
}

void MoveBoard::print(uint8_t player)
{
    bool our_player = player == m_our_player;
    print_upper_outlines();
    for (uint16_t y = 0; y < get_height(); y++)
    {
        std::cout << " " << std::setw(2) << y << " | ";
        for (uint16_t x = 0; x < get_width(); x++)
        {
            uint16_t c = two_dimension_2_one_dimension(x, y);
            if (!print_m_board_sets(c))
            {
                for (uint16_t i = 0; i < m_player_count; i++)
                {
                    if (m_player_sets[i].test(c))
                    {
                        std::cout << get_color_string(Colors(i + 1)) << i + 1;
#ifdef COLOR
                        std::cout << "\033[0m";
#endif
                    }
                }
            }
            if (our_player && m_valid_moves[player].test(c))
            {
                std::cout << "'";
            }
            else
            {
                std::cout << " ";
            }
        }
        std::cout << std::endl;
    }
    std::cout << "Calculated valid moves: " << m_valid_moves[player].count() << std::endl;
    std::cout << std::endl;
}
