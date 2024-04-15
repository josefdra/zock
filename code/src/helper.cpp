#include "helper.hpp"

std::array<unsigned char, 4> empty_fields{'0', 'i', 'c', 'b'};
std::array<unsigned char, 8> players{'1', '2', '3', '4', '5', '6', '7', '8'};

std::string getColorString(Colors color)
{
    switch (color)
    {
    case black:
        return "\e[90m";
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

/**
 * @brief checks if there are any free transitions, matches them with a second one and prints all of them
 *
 * @param m
 */
void transition_generate(Map &m)
{
    std::vector<std::array<uint16_t, 2>> tr;
    std::vector<std::array<uint16_t, 6>> output;
    std::array<uint16_t, 2> temp = {0, 0};
    for (int i = 1; i < (m.m_height * m.m_width + 1); i++)
    {
        if (m.m_symbol_and_transitions[i].symbol != '-')
        {
            for (int j = 0; j < 8; j++)
            {
                if (m.m_symbol_and_transitions[i].transitions[j] == 0)
                {
                    temp[0] = i;
                    temp[1] = j;
                    tr.push_back(temp);
                }
            }
        }
    }
    std::random_device rd;
    std::mt19937 g(rd());
    shuffle(tr.begin(), tr.end(), g);
    std::array<uint16_t, 6> temp_bigger = {0, 0, 0, 0, 0, 0};
    uint16_t temp_size = tr.size() / 2;
    for (int i = 0; i < temp_size; i++)
    {
        tr.back()[0] % m.m_width == 0 ? temp_bigger[0] = m.m_width : temp_bigger[0] = tr.back()[0] % m.m_width;
        temp_bigger[1] = (tr.back()[0] - 1) / m.m_width + 1;
        temp_bigger[2] = tr.back()[1];
        tr.pop_back();
        tr.back()[0] % m.m_width == 0 ? temp_bigger[3] = m.m_width : temp_bigger[3] = tr.back()[0] % m.m_width;
        temp_bigger[4] = (tr.back()[0] - 1) / m.m_width + 1;
        temp_bigger[5] = tr.back()[1];
        tr.pop_back();
        if (!(temp_bigger[0] == 0 && temp_bigger[1] == 0 && temp_bigger[3] == 0 && temp_bigger[4] == 0))
        {
            output.push_back(temp_bigger);
        }
        temp_bigger = {0, 0, 0, 0, 0, 0};
    }
    for (auto elem : output)
    {
        std::cout << elem[0] - 1 << " " << elem[1] - 1 << " " << elem[2] << " <-> " << elem[3] - 1 << " " << elem[4] - 1 << " " << elem[5] << " " << std::endl;
    }
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
