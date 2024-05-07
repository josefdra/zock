#include "helper.hpp"
#include "player.hpp"

std::array<unsigned char, 4> empty_fields{'0', 'i', 'c', 'b'};
std::array<unsigned char, 8> players{'1', '2', '3', '4', '5', '6', '7', '8'};
std::array<unsigned char, 3> special{'c', 'i', 'b'};

std::string getColorString(Colors color)
{
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
}

/// @brief checks if input can be interpreted as an empty cell like choice, inversion, bomb or an empty field
/// @param c current symbol at a certain coordinate
/// @return true/false
bool check_empty_fields(unsigned char c)
{
    bool var = false;
    for (uint8_t i = 0; i < empty_fields.size(); i++)
    {
        if (c == empty_fields[i])
        {
            var = true;
        }
    }
    return var;
}

/// @brief check if input is a player char
/// @param c current symbol at a certain coordinate
/// @return true/false
bool check_players(unsigned char c)
{
    bool var = false;
    for (uint8_t i = 0; i < players.size(); i++)
    {
        if (c == players[i])
        {
            var = true;
        }
    }
    return var;
}

/// @brief check if input is a special char
/// @param c current symbol at a certain coordinate
/// @return true/false
bool check_special(unsigned char c)
{
    bool var = false;
    for (uint8_t i = 0; i < special.size(); i++)
    {
        if (c == special[i])
        {
            var = true;
        }
    }
    return var;
}

uint16_t check_frontier(Map &m, uint16_t c, std::vector<char> &currMap)
{
    uint16_t score = 8;
    for (int d = 0; d < NUM_OF_DIRECTIONS; d++)
    {
        if (check_empty_fields(currMap[m.get_transition(c, d)]))
        {
            score--;
        }
    }
    return score;
}

EvalOfField evalFieldSymbol(unsigned char c)
{
    if (c == '0')
        return EvalOfField::empty;
    else if (c == 'b')
        return EvalOfField::bomb;
    else if (c == 'i')
        return EvalOfField::inversion;
    else if (c == 'c')
        return EvalOfField::choice;
    // @todo implement else case
    else
        // This return is wrong and has to be removed. Is just here to get rid of the warning.
        return EvalOfField::enemy;
    for (uint8_t i = 0; i < players.size(); i++)
    {
        if (c == players.at(i))
            return EvalOfField::enemy;
    }
}

void one_dimension_2_second_dimension(uint16_t &_1D_coord, uint8_t &x, uint8_t &y, Map &m)
{
    _1D_coord % m.m_width == 0 ? x = m.m_width - 1 : x = _1D_coord % m.m_width - 1;
    y = (_1D_coord - (x + 1)) / m.m_width;
}

void two_dimension_2_one_dimension(uint16_t &_1D_coord, uint8_t &x, uint8_t &y, Map &m)
{
    _1D_coord = x + 1 + y * m.m_width;
}

// currently returning static value, evaluation of whats best, needs to be done
uint8_t checkForSpecial(char &c)
{
    // add count of player stones to add functionality to switch stones automatically if enemy has more stones and maybe has more abilities to move
    if (c == 'c')
    {
        return 1;
    }
    else if (c == 'b')
    {
        return 20;
    }
    else
    {
        return 0;
    }
}
