#include "process_moves.hpp"

/**
 * @brief check_valid_moves.cpp is responsible for all calculations to verify valid moves
 *
 */

void check_coordinate(uint16_t coord, Map &map, Player &p, bool paint)
{
    bool valid = false, playerFound = false, foundSelf = false;
    uint16_t position, nextTransition, currentDirection, special_move;
    std::unordered_set<uint16_t> tempCellSet;
    std::unordered_set<uint16_t> addSet;
    special_move = 0;

    // check whether the cell can be entered without certain stones or whether the required stones are available
    if (check_empty_fields(map.m_symbol_and_transitions.at(coord).symbol) || p.has_overwrite_stones())
    {
        position = coord;
        if (map.m_symbol_and_transitions.at(position).symbol == 'i')
        {
            special_move = 1;
        }
        else if (map.m_symbol_and_transitions.at(position).symbol == 'c')
        {
            special_move = 2;
        }
        else if (map.m_symbol_and_transitions.at(position).symbol == 'b')
        {
            special_move = 3;
        }
        for (int i = 0; i < NUM_OF_DIRECTIONS; i++)
        {
            playerFound = false;
            foundSelf = false;
            currentDirection = i;
            position = coord;
            tempCellSet.insert(position);

            do
            {
                // get the coordinate of the next cell according to the current cell and direction of movement
                nextTransition = map.m_symbol_and_transitions.at(position).transitions.at(currentDirection);

                if (nextTransition == 0)
                {
                    tempCellSet.clear();
                    break;
                }

                currentDirection = nextTransition % 10;
                nextTransition /= 10;

                if (nextTransition == coord)
                {
                    foundSelf = true;
                    tempCellSet.clear();
                    break;
                }

                // check if enemy found
                if (!check_empty_fields(map.m_symbol_and_transitions.at(nextTransition).symbol) && map.m_symbol_and_transitions.at(nextTransition).symbol != p.m_symbol)
                {
                    tempCellSet.insert(nextTransition);
                    playerFound = true;
                    position = nextTransition;
                }
                // check whether other of our stones have been found
                else if (playerFound && map.m_symbol_and_transitions.at(nextTransition).symbol == p.m_symbol)
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
                else if (playerFound && check_empty_fields(map.m_symbol_and_transitions.at(nextTransition).symbol))
                {
                    tempCellSet.clear();
                    playerFound = false;
                }

            } while (playerFound);
        }
    }

    if (valid)
    {
        p.m_valid_moves[coord] = std::make_tuple(special_move, addSet);
    }
}

void change_players(Map &m, char p1, char p2)
{
    for (uint16_t i = 1; i < (m.m_width * m.m_height + 1); i++)
    {
        if (m.m_symbol_and_transitions.at(i).symbol == p1)
        {
            m.m_symbol_and_transitions.at(i).symbol = p2;
        }
        else if (m.m_symbol_and_transitions.at(i).symbol == p2)
        {
            m.m_symbol_and_transitions.at(i).symbol = p1;
        }
    }
}

void execute_move(uint16_t coord, Player &p, Map &m)
{

    if (!check_empty_fields(m.m_symbol_and_transitions.at(coord).symbol))
    {
        // p.m_overwrite_stones--;
    }

    auto elem = p.m_valid_moves.find(coord);

    if (elem != p.m_valid_moves.end())
    {
        uint16_t special;
        std::unordered_set<uint16_t> fields;
        std::tie(special, fields) = elem->second;

        // normal
        if (special == 0)
        {
            for (auto &field : fields)
            {
                m.m_symbol_and_transitions.at(field).symbol = p.m_symbol;
            }
        }
        // inversion
        else if (special == 1)
        {
            for (auto &field : fields)
            {
                m.m_symbol_and_transitions.at(field).symbol = p.m_symbol;
            }
            uint16_t helper = p.m_symbol - 1 - '0';
            helper = helper % m.m_player_count;
            char next_player = helper + 1 + '0';
            change_players(m, p.m_symbol, next_player);
            m.check_before_before_special_fields();
        }
        // choice
        else if (special == 2)
        {
            m.print_map();
            int next_player;
            do
            {
                std::cout << "Mit welchem Spieler wollen Sie tauschen?: ";
                std::cin >> next_player;
                if (next_player > m.m_player_count)
                {
                    std::cout << "not a valid player, try again" << std::endl;
                }
            } while (next_player > m.m_player_count);
            for (auto &field : fields)
            {
                m.m_symbol_and_transitions.at(field).symbol = p.m_symbol;
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
                std::cout << "Wollen Sie eine Bombe(b) oder einen Überschreibstein(u)?: ";
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
                m.m_symbol_and_transitions.at(field).symbol = p.m_symbol;
            }
            m.check_before_before_special_fields();
        }
        else
        {
            std::cout << "invalid special value" << std::endl;
        }
    }
    else
    {
        std::cout << "invalid move" << std::endl;
    }
}

/// @brief This function asks for a coordinate and checks if it's a valid move
/// @param map current map layout
/// @param player_number current player at turn
void check_moves(Map &map, Player &player)
{
    uint16_t coord, row, column;
    for (int i = 1; i < (map.m_width * map.m_height) + 1; i++)
    {
        if (map.m_symbol_and_transitions[i].symbol != '-')
        {
            check_coordinate(i, map, player, false);
        }
    }
    // std::cout << "Das sind alle möglichen Züge für Spieler " << player.get_symbol() << ":" << std::endl;
    // player.print_valid_moves(map.m_width);
    // std::cout << "Wo wollen Sie setzen? Format (x y): " << std::endl;
    // std::cin >> column >> row;
    // coord = column + 1 + row * map.m_width;
    // execute_move(coord, player, map);
    // map.print_map();
}

void paint_cells(std::unordered_set<uint16_t> &set, unsigned char player_number, Map &map)
{
    for (auto &elem : set)
    {
        map.m_symbol_and_transitions.at(elem).symbol = player_number;
    }
}

float eveluate(uint16_t coord, Player &p, Map &m)
{
    float return_value = -INFINITY;
    return return_value;
}