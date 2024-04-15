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
    for (int i = 0; i < empty_fields.size(); i++)
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
    for (int i = 0; i < players.size(); i++)
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
    for (int i = 0; i < special.size(); i++)
    {
        if (c == special[i])
        {
            var = true;
        }
    }
    return var;
}

/**
 * @brief Measures how long it takes, to execute a function.
 * @brief Example Usage:
 * @brief auto func = [&map]() { map.read_hash_map("../../maps/boeseMap09.map"); };
 * @brief timer_function(func, "read_hash_map");
 * @brief for member functions the object must be referenzed in []
 * @param func
 * @param func_name
 */

// Timer function removed, because it won't work. Just copy code

/*
template <typename F>
void timer_function(F func, std::string func_name)
{
    h_res_clock::time_point start_time = h_res_clock::now();
    func();
    h_res_clock::time_point end_time = h_res_clock::now();

    std::chrono::duration<double, std::micro> elapsed_time =
        std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

    std::cout << "Function: " << func_name << std::endl;
    std::cout << "Elapsed time: " << elapsed_time.count() << " microseconds" << std::endl;
}
*/

uint16_t check_frontier(Map &m, uint16_t coord)
{
    uint16_t score = 8;
    for (int j = 0; j < NUM_OF_DIRECTIONS; j++)
    {
        if (check_empty_fields(m.m_symbol_and_transitions[m.m_symbol_and_transitions[coord].transitions[j] / 10].symbol))
        {
            score--;
        }
        }
    return score;
}

EvalOfField evalFieldSymbol(char c)
{
    if (c == '0')
        return EvalOfField::empty;
    else if (c == 'b')
        return EvalOfField::bomb;
    else if (c == 'i')
        return EvalOfField::inversion;
    else if (c == 'c')
        return EvalOfField::choice;
    for (int i = 0; i < players.size(); i++)
        if (c == players.at(i))
            return EvalOfField::enemy;
}
