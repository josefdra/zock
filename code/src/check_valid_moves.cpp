#include "check_valid_moves.hpp"

/**
 * @brief check_valid_moves.cpp is responsible for all calculations to verify valid moves
 *
 */
std::array<unsigned char, 4> empty_fields{'0', 'i', 'c', 'b'};
std::array<unsigned char, 8> players{'1', '2', '3', '4', '5', '6', '7', '8'};

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

void print_tuple_vector(std::vector<std::tuple<uint16_t, uint16_t>> &vec)
{
    for (std::tuple<uint16_t, uint16_t> &element : vec)
    {
        uint16_t x;
        uint16_t y;
        std::tie(x, y) = element;
        std::cout << "(" << x << ", " << y << ")" << std::endl;
    }
}

void check_coordinate(uint16_t coord, std::vector<std::tuple<uint16_t, uint16_t>> &valid_moves, Map &map, bool paint)
{
    // CHANGE
    char currPlayer = '1';
    bool changedSmth = false;
    bool inversion = false;
    uint16_t position;
    uint16_t currTransitionCoord;
    std::unordered_set<uint16_t> cellsToChange;

    // check whether the cell can be entered without certain stones or whether the required stones are available
    if (check_empty_fields(map.all_map_moves.at(coord).symbol) || (map.ueberschreibsteine > 0))
    {
        for (int i = 0; i < NUM_OF_DIRECTIONS; i++)
        {
            bool playerFound = false;
            bool foundSelf = false;
            uint8_t currentDirection = i;
            position = coord;
            std::unordered_set<uint16_t> tempCellSet;
            tempCellSet.insert(position);

            if (map.all_map_moves.at(position).symbol == 'i')
            {
                inversion = true;
            }
            do
            {
                // get the coordinate of the next cell according to the current cell and direction of movement
                currTransitionCoord = map.all_map_moves.at(position).transitions.at(currentDirection);

                if (currTransitionCoord == 0)
                {
                    break;
                }

                // Vielleicht drehen der Richtungen zum einlesen verschieben
                currentDirection = currTransitionCoord % 10;
                currTransitionCoord /= 10;
                currentDirection += 4;
                currentDirection = currentDirection % 8;

                if (currTransitionCoord == coord)
                {
                    foundSelf = true;
                }

                // check if enemy found
                if (!check_empty_fields(map.all_map_moves.at(currTransitionCoord).symbol) && map.all_map_moves.at(currTransitionCoord).symbol != currPlayer)
                {
                    tempCellSet.insert(currTransitionCoord);
                    playerFound = true;
                    position = currTransitionCoord;
                }
                // check whether other of our stones have been found
                else if (playerFound && map.all_map_moves.at(currTransitionCoord).symbol == currPlayer)
                {
                    if (!foundSelf)
                    {
                        for (auto elem : tempCellSet)
                        {
                            cellsToChange.insert(elem);
                        }
                        changedSmth = true;
                    }
                    playerFound = false;
                    break;
                }

                // check if other symbol is found
                else if (playerFound && check_empty_fields(map.all_map_moves.at(currTransitionCoord).symbol))
                {
                    playerFound = false;
                }

            } while (playerFound);
        }
    }

    if (paint)
    {
        paint_cells(cellsToChange, currPlayer, map);
    }

    if (changedSmth)
    {
        uint16_t x, y;
        coord % map.width == 0 ? x = map.width - 1 : x = coord % map.width - 1;
        y = (coord - (x + 1)) / map.width;
        std::tuple<uint16_t, uint16_t> tuple(x, y);
        valid_moves.push_back(tuple);
        if (!check_empty_fields(map.all_map_moves.at(coord).symbol))
        {
            // ueberschreibstein abziehen
        }
        if (inversion && paint)
        {
            for (int i = 1; i < (map.width * map.height + 1); i++)
            {
                if (check_players(map.all_map_moves[i].symbol))
                {
                    uint16_t number = (int)map.all_map_moves[i].symbol;
                    number = (number + 1) % map.spielerzahl;
                    map.all_map_moves[i].symbol = (char)number;
                }
            }
        }
    }
}

/// @brief This function asks for a coordinate and checks if it's a valid move
/// @param map current map layout
void process_moves(Map &map)
{
    std::vector<std::tuple<uint16_t, uint16_t>> valid_moves;
    uint16_t coord;
    for (int i = 1; i < (map.width * map.height) + 1; i++)
    {
        if (map.all_map_moves[coord].symbol != '-')
        {
            check_coordinate(i, valid_moves, map, false);
        }
    }
    std::cout << "Das sind alle möglichen Züge: " << std::endl;
    print_tuple_vector(valid_moves);
    std::cout << "Wo wollen Sie setzen? Format (x y): " << std::endl;
    int row;
    int column;
    std::cin >> column >> row;
    coord = column + 1 + row * map.width;
    check_coordinate(coord, valid_moves, map, true);
    map.print_map();
}

void paint_cells(std::unordered_set<uint16_t> &set, unsigned char player_number, Map &map)
{
    for (auto elem : set)
    {
        map.all_map_moves.at(elem).symbol = player_number;
    }
}