#include "process_moves.hpp"

/**
 * @brief check_valid_moves.cpp is responsible for all calculations to verify valid moves
 *
 */
std::array<unsigned char, 4> empty_fields{'0', 'i', 'c', 'b'};
std::array<unsigned char, 8> players{'1', '2', '3', '4', '5', '6', '7', '8'};

void print_valid_moves(std::unordered_map<uint16_t, std::tuple<uint16_t, std::unordered_set<uint16_t>>> &moves, uint16_t width)
{
    uint16_t x, y, coord;
    for (auto elem : moves)
    {
        coord = elem.first;
        coord % width == 0 ? x = width - 1 : x = coord % width - 1;
        y = (coord - (x + 1)) / width;
        std::cout << "(" << x << " , " << y << ")" << std::endl;
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

void check_coordinate(uint16_t coord, Map &map, Player &p, bool paint)
{
    bool valid = false, inversion = false, choice = false, bonus = false, expansion = false, playerFound = false, foundSelf = false;
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
            inversion = true;
            special_move = 1;
        }
        else if (map.m_symbol_and_transitions.at(position).symbol == 'c')
        {
            choice = true;
            special_move = 2;
        }
        else if (map.m_symbol_and_transitions.at(position).symbol == 'b')
        {
            bonus = true;
            special_move = 3;
        }
        else if (map.m_symbol_and_transitions.at(position).symbol == 'x')
        {
            expansion = true;
            special_move = 4;
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
                if (!check_empty_fields(map.m_symbol_and_transitions.at(nextTransition).symbol) && map.m_symbol_and_transitions.at(nextTransition).symbol != p.m_player_symbol)
                {
                    tempCellSet.insert(nextTransition);
                    playerFound = true;
                    position = nextTransition;
                }
                // check whether other of our stones have been found
                else if (playerFound && map.m_symbol_and_transitions.at(nextTransition).symbol == p.m_player_symbol)
                {
                    if (!foundSelf)
                    {
                        valid = true;
                        for (auto element : tempCellSet)
                        {
                            addSet.insert(element);
                        }
                    }
                    playerFound = false;
                    break;
                }
                // check if other symbol is found
                else if (playerFound && check_empty_fields(map.m_symbol_and_transitions.at(nextTransition).symbol))
                {
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
        p.decrement_overwrite_stone();
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
            for (uint16_t field : fields)
            {
                m.m_symbol_and_transitions.at(field).symbol = p.m_player_symbol;
            }
        }
        // inversion
        else if (special == 1)
        {
            for (uint16_t field : fields)
            {
                m.m_symbol_and_transitions.at(field).symbol = p.m_player_symbol;
            }
            char next_player = (char)((int)p.m_player_symbol + 1);
            change_players(m, p.m_player_symbol, next_player);
        }
        // choice
        else if (special == 2)
        {
            std::cout << "Mit welchem Spieler wollen Sie tauschen?: ";
            char next_player;
            std::cin >> next_player;
            for (uint16_t field : fields)
            {
                m.m_symbol_and_transitions.at(field).symbol = p.m_player_symbol;
            }
            change_players(m, p.m_player_symbol, next_player);
        }
        // bonus
        else if (special == 3)
        {
        }
        // expansion
        else if (special == 4)
        {
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
    std::cout << "Das sind alle möglichen Züge für Spieler " << player.m_player_symbol << ":" << std::endl;
    print_valid_moves(player.m_valid_moves, map.m_width);
    std::cout << "Wo wollen Sie setzen? Format (x y): " << std::endl;
    std::cin >> column >> row;
    coord = column + 1 + row * map.m_width;
    execute_move(coord, player, map);
    map.print_map();
}

void paint_cells(std::unordered_set<uint16_t> &set, unsigned char player_number, Map &map)
{
    for (auto elem : set)
    {
        map.m_symbol_and_transitions.at(elem).symbol = player_number;
    }
}