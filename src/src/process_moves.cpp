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

void execute_move(uint16_t c, Player &p, Map &m)
{
    if (!check_empty_fields(m.get_symbol(c)))
    {
        p.m_overwrite_stones--;
    }
    uint16_t special;
    std::unordered_set<uint16_t> fields;
    std::tie(special, fields) = p.m_valid_moves[c];

    // normal
    if (special == 0)
    {
        for (auto &field : fields)
        {
            m.set_symbol(field, p.m_symbol);
        }
    }
    // inversion
    else if (special == 1)
    {
        for (auto &field : fields)
        {
            m.set_symbol(field, p.m_symbol);
        }
        uint16_t helper = p.m_symbol - '0' - 1;
        helper = (helper + 1) % m.m_player_count;
        unsigned char next_player = helper + 1 + '0';
        change_players(m, p.m_symbol, next_player);
        m.check_before_before_special_fields();
    }
    // choice
    else if (special == 2)
    {
        m.print_map();
        uint16_t next_player;
        do
        {
            std::cout << "Mit welchem Spieler wollen Sie tauschen?: (Muss die selbe Antwort wie zuvor sein)";
            std::cin >> next_player;
            if (next_player > m.m_player_count)
            {
                std::cout << "not a valid player, try again" << std::endl;
            }
        } while (next_player > m.m_player_count);
        for (auto &field : fields)
        {
            m.set_symbol(field, p.m_symbol);
        }
        change_players(m, p.m_symbol, ('0' + next_player));
        m.check_before_before_special_fields();
    }
    // bonus
    else if (special == 3)
    {
        char bonus;
        do
        {
            std::cout << "Wollen Sie eine Bombe(b) oder einen Überschreibstein(u)?: (Muss die selbe Antwort)";
            std::cin >> bonus;
            if (bonus == 'b')
            {
                p.m_bombs++;
            }
            else if (bonus == 'u')
            {
                p.m_overwrite_stones++;
            }
            else
            {
                std::cout << "not a valid input, try again" << std::endl;
            }
        } while (bonus != 'b' && bonus != 'u');
        for (auto &field : fields)
        {
            m.set_symbol(field, p.m_symbol);
        }
        m.check_before_before_special_fields();
    }
    else
    {
        std::cout << "invalid special value" << std::endl;
    }
}

/// @brief This function asks for a coordinate and checks if it's a valid move
/// @param map current map layout
/// @param player_number current player at turn
void check_moves(Map &m, Player &p)
{
    for (uint16_t c = 1; c < m.m_num_of_fields + 1; c++)
    {
        unsigned char s = m.get_symbol(c);
        if (s != '-' && s != p.m_symbol)
        {
            if (check_empty_fields(s))
            {
                check_coordinate(c, m, p);
            }
            else if (p.has_overwrite_stones())
            {
                check_coordinate(c, m, p);
            }
        }
    }
}

void paint_cells(std::unordered_set<uint16_t> &set, unsigned char player_number, Map &map)
{
    for (auto &elem : set)
    {
        map.set_symbol(elem, player_number);
    }
}
