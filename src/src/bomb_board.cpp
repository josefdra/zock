#include "bomb_board.hpp"
#include "map.hpp"
#include "move_board.hpp"

BombBoard::BombBoard(Map &map) : Board(map)
{
    m_fields_to_remove = std::vector<std::bitset<2501>>(m_num_of_fields);
    m_transitions_to_remove = std::vector<std::bitset<20000>>(m_num_of_fields);
    m_transitions = map.get_transitions();
    m_strength = map.get_strength();
}

BombBoard::BombBoard(const BombBoard &other)
    : Board(other),
      m_fields_to_remove(other.m_fields_to_remove),
      m_transitions_to_remove(other.m_transitions_to_remove),
      m_transitions(other.m_transitions),
      m_strength(other.m_strength) {}

BombBoard::BombBoard(BombBoard &&other) noexcept
    : Board(std::move(other)),
      m_fields_to_remove(std::move(other.m_fields_to_remove)),
      m_transitions_to_remove(std::move(other.m_transitions_to_remove)),
      m_transitions(std::move(other.m_transitions)),
      m_strength(std::move(other.m_strength)) {}

BombBoard &BombBoard::operator=(const BombBoard &other)
{
    if (this != &other)
    {
        Board::operator=(other);
        m_fields_to_remove = other.m_fields_to_remove;
        m_transitions_to_remove = other.m_transitions_to_remove;
        m_transitions = other.m_transitions;
        m_strength = other.m_strength;
    }
    return *this;
}

BombBoard &BombBoard::operator=(BombBoard &&other) noexcept
{
    if (this != &other)
    {
        Board::operator=(std::move(other));
        m_fields_to_remove = std::move(other.m_fields_to_remove);
        m_transitions_to_remove = std::move(other.m_transitions_to_remove);
        m_transitions = std::move(other.m_transitions);
        m_strength = std::move(other.m_strength);
    }
    return *this;
}

BombBoard::~BombBoard() {}

void BombBoard::set_transition(uint16_t c, uint8_t d, uint16_t t)
{
    m_transitions[(c - 1) * 8 + d] = t;
}

void BombBoard::set_zero_transition(uint16_t c)
{
    m_transitions[c] = 0;
}

void BombBoard::set_values(MoveBoard &move_board)
{
    m_board_sets = move_board.get_board_sets();
    m_player_sets = move_board.get_player_sets();
    m_bombs = move_board.get_all_bombs();
    m_disqualified = move_board.get_disqualified();
}

uint8_t BombBoard::get_strength()
{
    return m_strength;
}

uint16_t BombBoard::get_transition(uint16_t c, uint8_t d)
{
    return m_transitions[(c - 1) * 8 + d] / 10;
}

uint8_t BombBoard::get_direction(uint16_t c, uint8_t d)
{
    return m_transitions[(c - 1) * 8 + d] % 10;
}

std::bitset<20000> &BombBoard::get_transitions_to_remove(uint16_t c)
{
    return m_transitions_to_remove[c];
}

std::bitset<2501> &BombBoard::get_fields_to_remove(uint16_t c)
{
    return m_fields_to_remove[c];
}

void BombBoard::decrement_bombs(uint8_t player)
{
    m_bombs[player]--;
}

void BombBoard::print()
{
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
            std::cout << " ";
        }
        std::cout << std::endl;
    }
}

void BombBoard::init_bomb_phase_boards()
{
    // for (uint16_t c = 1; c < m_num_of_fields; c++)
    // {
    //     if (!m_board_sets[0].test(c))
    //     {
    //         m_fields_to_remove[c].set(c);

    //         for (uint8_t d = 0; d < 8; d++)
    //         {
    //             if (m_strength == 0)
    //             {
    //                 uint16_t t = get_transition(c, d);
    //                 if (t != 0)
    //                 {
    //                     m_transitions_to_remove[c].set((c - 1) * 8 + d);
    //                     m_transitions_to_remove[c].set((t - 1) * 8 + (d + 4) % 8);
    //                 }
    //             }
    //             else
    //             {
    //             }
    //         }
    //     }
    // }
}
