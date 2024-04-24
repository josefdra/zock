#include "process_moves.hpp"

/**
 * @brief check_valid_moves.cpp is responsible for all calculations to verify valid moves
 *
 */

void check_coordinate(uint16_t c, Map &m, Player &p)
{
    bool valid = false, playerFound = false, foundSelf = false;
    uint16_t pos, nextTransition, currentDirection, special_move, temp;
    unsigned char symbol;
    std::unordered_set<uint16_t> tempCellSet;
    std::unordered_set<uint16_t> addSet;
    special_move = 0;

    // check whether the cell can be entered without certain stones or whether the required stones are available
    if (check_empty_fields(m.get_symbol(c)) || p.has_overwrite_stones())
    {
        pos = c;
        symbol = m.get_symbol(c);
        if (symbol == 'i')
        {
            special_move = 1;
        }
        else if (symbol == 'c')
        {
            special_move = 2;
        }
        else if (symbol == 'b')
        {
            special_move = 3;
        }
        for (int d = 0; d < NUM_OF_DIRECTIONS; d++)
        {
            playerFound = false;
            foundSelf = false;
            currentDirection = d;
            pos = c;
            tempCellSet.insert(pos);

            do
            {
                // get the coordinate of the next cell according to the current cell and direction of movement
                temp = pos;
                nextTransition = m.get_transition(pos, currentDirection);

                if (nextTransition == 0)
                {
                    tempCellSet.clear();
                    break;
                }

                currentDirection = m.get_direction(temp, currentDirection);

                if (nextTransition == c)
                {
                    foundSelf = true;
                    tempCellSet.clear();
                    break;
                }
                unsigned char s = m.get_symbol(nextTransition);
                // check if enemy found
                if (!check_empty_fields(s) && s != p.m_symbol)
                {
                    tempCellSet.insert(nextTransition);
                    playerFound = true;
                    pos = nextTransition;
                }
                // check whether other of our stones have been found
                else if (playerFound && s == p.m_symbol)
                {
                    if (!foundSelf)
                    {
                        valid = true;
                        for (auto &element : tempCellSet)
                        {
                            addSet.insert(element);
                        }
                    }
                    playerFound = false;
                    tempCellSet.clear();
                    break;
                }
                // check if other symbol is found
                else if (playerFound && check_empty_fields(s))
                {
                    tempCellSet.clear();
                    playerFound = false;
                }

            } while (playerFound);
        }
    }

    if (valid)
    {
        p.m_valid_moves[c] = std::make_tuple(special_move, addSet);
    }
}

void change_players(Map &m, unsigned char p1, unsigned char p2)
{
    for (uint16_t c = 1; c < m.m_num_of_fields + 1; c++)
    {
        unsigned char s = m.get_symbol(c);
        if (s == p1)
        {
            m.set_symbol(c, p2);
        }
        else if (s == p2)
        {
            m.set_symbol(c, p1);
        }
    }
}

void color(uint16_t c, char s, Map &m)
{
    std::unordered_set<uint16_t> to_color;
    to_color.insert(c);
    for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d++)
    {
        bool run = true;
        uint16_t temp_transition = m.get_transition(c, d);
        uint8_t temp_direction = m.get_direction(c, d);
        std::unordered_set<uint16_t> temp;
        char symbol;
        while (temp_transition != 0 && !check_empty_fields(symbol = m.get_symbol(temp_transition)) && run)
        {
            if (symbol == s)
            {
                for (auto &c : temp)
                {
                    to_color.insert(c);
                }
                run = false;
            }
            else
            {
                temp.insert(temp_transition);
                uint16_t next_transition = m.get_transition(temp_transition, temp_direction);
                uint8_t next_direction = m.get_direction(temp_transition, temp_direction);
                temp_transition = next_transition;
                temp_direction = next_direction;
            }
        }
    }
    for (auto &c : to_color)
    {
        m.set_symbol(c, s);
    }
}

void execute_move(uint16_t c, uint8_t special, Player &p, Map &m)
{
    char curr_symbol = m.get_symbol(c);
    if (!check_empty_fields(curr_symbol))
    {
        p.m_overwrite_stones--;
        color(c, p.m_symbol, m);
    }
    // normal
    else if (curr_symbol == '0')
    {
        color(c, p.m_symbol, m);
    }
    // inversion
    else if (curr_symbol == 'i')
    {
        color(c, p.m_symbol, m);
        uint16_t helper = p.m_symbol - '0' - 1;
        helper = (helper + 1) % m.m_player_count;
        unsigned char next_player = helper + 1 + '0';
        change_players(m, p.m_symbol, next_player);
        
    }
    // choice
    else if (curr_symbol == 'c')
    {
        color(c, p.m_symbol, m);
        change_players(m, p.m_symbol, ('0' + special));
    }
    // bonus
    else if (curr_symbol == 'b')
    {
        color(c, p.m_symbol, m);
        if (special == 20)
        {
            p.m_bombs += 1;
        }
        else if (special == 21)
        {
            p.m_overwrite_stones += 1;
        }
    }
    else
    {
        std::cout << "Invalid special value" << std::endl;
        return;
    }
}

void execute_bomb(uint16_t c, Map &m, Player &p)
{
    // @todo add strength and do something in case next player has no more fields;
    m.set_symbol(c, '-');
    for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d++)
    {
        uint16_t temp_transition = m.get_transition(c, d);
        uint8_t temp_direction = m.get_direction(c, d);
        m.set_transition(c, d, 0);
        m.set_transition(temp_transition, temp_direction, 0);
    }
    p.m_bombs -= 1;
}

/// @brief This function asks for a coordinate and checks if it's a valid move
/// @param map current map layout
/// @param player_number current player at turn
void check_moves(Map &m, Player &p)
{
    p.m_valid_moves.clear();
    for (uint16_t c = 1; c < m.m_num_of_fields + 1; c++)
    {
        unsigned char s = m.get_symbol(c);
        if (s != '-' && s != p.m_symbol)
        {
            if (check_empty_fields(s) || p.has_overwrite_stones())
            {
                check_coordinate(c, m, p);
            }
        }
    }
}
