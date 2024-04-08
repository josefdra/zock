#include "check_valid_moves.hpp"

std::array<unsigned char, 4> empty_fields{'0', 'i', 'c', 'b'};
std::array<unsigned char, 8> players{'1', '2', '3', '4', '5', '6', '7', '8'};

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

void process_moves(Map& map)
{
    // int because cin can't convert to uint8_t
    int row;
    int column;
    char throwaway;
    std::cout << "enter coordinates to check move in this format: column row" << std::endl;
    std::cin >> column >> row;
    uint16_t coord;
    coord = column + 1 + row * map.width;
    char currPlayer = '1';
    uint16_t currTransitionCoord;
    bool changedSmth = false;
    bool inversion = false;
    uint16_t position;
    std::unordered_set<uint16_t> cellsToChange;
    // ueberpruefe ob betretbares Feld (0) gewählt wurde
    if (check_empty_fields(map.all_map_moves.at(coord).symbol) || (map.ueberschreibsteine > 0))
    {
        // suche nach umliegenden Gegnerischen Steinen
        for (int i = 0; i < NUM_OF_DIRECTIONS; i++)
        {
            bool playerFound = false;
            bool foundSelf = false;            
            uint8_t currentDirection = i;
            position = coord;
            std::unordered_set<uint16_t> tempCellSet;
            tempCellSet.insert(position);
            // check if current Coordinate has transitions
            if(map.all_map_moves.at(position).symbol == 'i'){
                inversion = true;
            }
            do
            {

                currTransitionCoord = map.all_map_moves.at(position).transitions.at(currentDirection);

                if (currTransitionCoord == 0)
                {
                    break;
                }

                currentDirection = currTransitionCoord % 10;
                currTransitionCoord /= 10;
                currentDirection += 4;
                currentDirection = currentDirection % 8;

                // Vielleicht drehen der Richtungen zum einlesen verschieben

                if(currTransitionCoord == coord){
                    foundSelf = true;
                }

                if (!check_empty_fields(map.all_map_moves.at(currTransitionCoord).symbol) && map.all_map_moves.at(currTransitionCoord).symbol != currPlayer)
                {
                    tempCellSet.insert(currTransitionCoord);
                    playerFound = true;
                    position = currTransitionCoord;
                }
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
                else if (playerFound && check_empty_fields(map.all_map_moves.at(currTransitionCoord).symbol))
                {
                    playerFound = false;
                }

            } while (playerFound);
        }
    }
    paint_cells(cellsToChange, currPlayer, map);
    if (changedSmth)
    {
        if (!check_empty_fields(map.all_map_moves.at(coord).symbol))
        {
            // ueberschreibstein abziehen
        }
        if(inversion){
            for (int i = 1; i < (map.width * map.height + 1); i++){
                if(check_players(map.all_map_moves[i].symbol)){
                    uint16_t number = (int)map.all_map_moves[i].symbol;
                    number = (number + 1) % map.spielerzahl;
                    map.all_map_moves[i].symbol = (char)number;
                }
            }
        }
        map.print_map();
    }
}

void paint_cells(std::unordered_set<uint16_t>& set, unsigned char player_number, Map& map)
{
    for (auto elem : set)
    {
        map.all_map_moves.at(elem).symbol = player_number;
    }
}