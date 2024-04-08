#include "map.hpp"

Map::Map(){};
Map::~Map(){};

void Map::check_neighbours(uint16_t n)
{
    n > width &&all_map_moves[n - width].symbol != '-' ? all_map_moves[n].transitions[0] = (n - width) * 10 + 4 : all_map_moves[n].transitions[0] = 0;
    n % width != 0 && n > width &&all_map_moves[n - width + 1].symbol != '-' ? all_map_moves[n].transitions[1] = (n - width + 1) * 10 + 5 : all_map_moves[n].transitions[1] = 0;
    n % width != 0 && all_map_moves[n + 1].symbol != '-' ? all_map_moves[n].transitions[2] = (n + 1) * 10 + 6 : all_map_moves[n].transitions[2] = 0;
    n % width != 0 && n <= width *(height - 1) && all_map_moves[n + width + 1].symbol != '-' ? all_map_moves[n].transitions[3] = (n + width + 1) * 10 + 7 : all_map_moves[n].transitions[3] = 0;
    n <= width *(height - 1) && all_map_moves[n + width].symbol != '-' ? all_map_moves[n].transitions[4] = (n + width) * 10 + 0 : all_map_moves[n].transitions[4] = 0;
    n % width != 1 && n <= width *(height - 1) && all_map_moves[n + width - 1].symbol != '-' ? all_map_moves[n].transitions[5] = (n + width - 1) * 10 + 1 : all_map_moves[n].transitions[5] = 0;
    n % width != 1 && all_map_moves[n - 1].symbol != '-' ? all_map_moves[n].transitions[6] = (n - 1) * 10 + 2 : all_map_moves[n].transitions[6] = 0;
    n % width != 1 && n > width &&all_map_moves[n - width - 1].symbol != '-' ? all_map_moves[n].transitions[7] = (n - width - 1) * 10 + 3 : all_map_moves[n].transitions[7] = 0;
}

void Map::read_hash_map(const std::string inputfile)
{
    h_res_clock::time_point start_time = h_res_clock::now();
    std::ifstream inputFile(inputfile);
    std::stringstream mapfile;
    mapfile << inputFile.rdbuf();
    mapfile << 65000;
    inputFile.close();
    mapfile >> spielerzahl >> ueberschreibsteine >> bomben >> staerke >> height >> width;
    hash_map_element elem;
    for (int i = 1; i < (width * height + 1); i++)
    {
        mapfile >> elem.symbol;
        elem.transitions = {0, 0, 0, 0, 0, 0, 0, 0};
        all_map_moves[i] = elem;
    }
    for (int i = 1; i < (width * height + 1); i++)
    {
        if (all_map_moves[i].symbol != '-')
        {
            check_neighbours(i);
        }
    }
    while (mapfile)
    {
        uint16_t x1, y1, r1, x2, y2, r2, pos1, pos2, pos1r, pos2r = 0;
        unsigned char trash;
        mapfile >> x1 >> y1 >> r1 >> trash >> trash >> trash >> x2 >> y2 >> r2;
        if (x1 != 65000)
        {
            x1++;
            x2++;
            pos1 = (x1) + (y1)*width;
            pos2 = (x2) + (y2)*width;
            pos1r = pos1 * 10 + r1;
            pos2r = pos2 * 10 + r2;
            all_map_moves[pos1].transitions[r1] = pos2r;
            all_map_moves[pos2].transitions[r2] = pos1r;
        }
    }
    h_res_clock::time_point end_time = h_res_clock::now();
    std::chrono::duration<double, std::micro> elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    std::cout << "Elapsed time (read_hash_map): " << elapsed_time.count() << " microseconds" << std::endl;
}

void Map::print_transitions()
{
    std::cout << std::endl;
    for (int y = 0; y < height; y++)
    {
        for (int n = 1; n < width + 1; n++)
        {
            int x = width * y + n;
            std::cout << std::setw(3) << all_map_moves[x].transitions[7] << " ";
            std::cout << std::setw(3) << all_map_moves[x].transitions[0] << " ";
            std::cout << std::setw(3) << all_map_moves[x].transitions[1] << " ";
            std::cout << "  ";
        }
        std::cout << std::endl;
        for (int n = 1; n < width + 1; n++)
        {
            int x = width * y + n;
            std::cout << std::setw(3) << all_map_moves[x].transitions[6] << " ";
            std::cout << std::setw(3) << all_map_moves[x].symbol << " ";
            std::cout << std::setw(3) << all_map_moves[x].transitions[2] << " ";
            std::cout << "  ";
        }
        std::cout << std::endl;
        for (int n = 1; n < width + 1; n++)
        {
            int x = width * y + n;
            std::cout << std::setw(3) << all_map_moves[x].transitions[5] << " ";
            std::cout << std::setw(3) << all_map_moves[x].transitions[4] << " ";
            std::cout << std::setw(3) << all_map_moves[x].transitions[3] << " ";
            std::cout << "  ";
        }
        std::cout << std::endl;
        std::cout << std::endl;
    }
}

void Map::print_map()
{
    std::cout << spielerzahl << std::endl;
    std::cout << ueberschreibsteine << std::endl;
    std::cout << bomben << " " << staerke << std::endl;
    std::cout << height << " " << width << std::endl;
    for (int i = 1; i < (width * height + 1); i++)
    {
        std::cout << all_map_moves[i].symbol << " ";
        if (i % width == 0)
        {
            std::cout << std::endl;
        }
    }
    print_transitions();
}

bool Map::check_empty_fields(unsigned char c)
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

bool Map::check_players(unsigned char c)
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

void Map::process_moves()
{
    // int because cin can't convert to uint8_t
    int row;
    int column;
    char throwaway;
    std::cout << "enter coordinates to check move in this format: column row" << std::endl;
    std::cin >> column >> row;
    uint16_t coord;
    coord = column + 1 + row * width;
    char currPlayer = '1';
    uint16_t currTransitionCoord;
    bool changedSmth = false;
    bool inversion = false;
    uint16_t position;
    std::unordered_set<uint16_t> cellsToChange;
    // ueberpruefe ob betretbares Feld (0) gewählt wurde
    if (check_empty_fields(all_map_moves.at(coord).symbol) || (ueberschreibsteine > 0))
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
            if(all_map_moves.at(position).symbol == 'i'){
                inversion = true;
            }
            do
            {

                currTransitionCoord = all_map_moves.at(position).transitions.at(currentDirection);

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

                if (!check_empty_fields(all_map_moves.at(currTransitionCoord).symbol) && all_map_moves.at(currTransitionCoord).symbol != currPlayer)
                {
                    tempCellSet.insert(currTransitionCoord);
                    playerFound = true;
                    position = currTransitionCoord;
                }
                else if (playerFound && all_map_moves.at(currTransitionCoord).symbol == currPlayer)
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
                else if (playerFound && check_empty_fields(all_map_moves.at(currTransitionCoord).symbol))
                {
                    playerFound = false;
                }

            } while (playerFound);
        }
    }
    paint_cells(cellsToChange, currPlayer);
    if (changedSmth)
    {
        if (!check_empty_fields(all_map_moves.at(coord).symbol))
        {
            // ueberschreibstein abziehen
        }
        if(inversion){
            for (int i = 1; i < (width * height + 1); i++){
                if(check_players(all_map_moves[i].symbol)){
                    uint16_t number = (int)all_map_moves[i].symbol;
                    number = (number + 1) % spielerzahl;
                    all_map_moves[i].symbol = (char)number;
                }
            }
        }
        print_map();
    }
}

void Map::paint_cells(std::unordered_set<uint16_t> &set, unsigned char player_number)
{
    for (auto elem : set)
    {
        all_map_moves.at(elem).symbol = player_number;
    }
}