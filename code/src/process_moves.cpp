#include "process_moves.hpp"

/**
 * @brief check_valid_moves.cpp is responsible for all calculations to verify valid moves
 *
 */

void check_coordinate(uint16_t c, Map &m, Player &p)
{
    uint16_t pos;
    uint8_t dir;
    bool valid_once = false;
    std::unordered_set<uint16_t> moves;
    for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d++)
    {
        std::unordered_set<uint16_t> temp1;
        bool valid_move = false, wall = false;
        temp1.insert(c);
        dir = m.get_direction(c, d);
        pos = m.get_transition(c, d);
        while (!check_empty_fields(m.get_symbol(pos)) && m.get_symbol(pos) != p.m_symbol && !wall)
        {
            std::unordered_set<uint16_t> temp2;
            temp2.insert(pos);
            dir = m.get_direction(pos, dir);
            pos = m.get_transition(pos, d);            
            if (pos == 0)
            {
                valid_move = false;
                wall = true;
            }
            if (check_empty_fields(m.get_symbol(pos)))
            {
                valid_move = true;
                valid_once = true;
                temp2.insert(pos);
            }
            else if (!check_empty_fields(m.get_symbol(pos)) && m.get_symbol(pos) != p.m_symbol && p.has_overwrite_stones())
            {
                std::unordered_set<uint16_t> temp3;
                temp2.insert(pos);
                uint16_t curr_pos = pos;
                bool found_self = false;
                for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d++)
                {
                    pos = m.get_transition(curr_pos, d);
                    dir = d;
                    while (!check_empty_fields(m.get_symbol(pos)))
                    {
                        temp3.insert(pos);
                        if(m.get_symbol(pos) == p.m_symbol){
                            found_self = true;
                            break;
                        }
                        dir = m.get_direction(pos, d);
                        pos = m.get_transition(pos, d);                        
                    }
                }
            }
        }
        if ()
    }
    if (valid_once)
    {
        if (m.get_symbol(pos) == 'i')
        {
            p.m_valid_moves[pos] = std::tuple(1, temp);
        }
        else if (m.get_symbol(pos) == 'c')
        {
            p.m_valid_moves[pos] = std::tuple(2, temp);
        }
        else if (m.get_symbol(pos) == 'b')
        {
            p.m_valid_moves[pos] = std::tuple(3, temp);
        }
        else
        {
            p.m_valid_moves[pos] = std::tuple(0, temp);
        }
    }
}

void change_players(Map &m, unsigned char p1, unsigned char p2)
{
    for (uint16_t i = 1; i < m.m_num_of_fields + 1; i++)
    {
        if (m.get_symbol(i) == p1)
        {
            m.set_symbol(i, p2);
        }
        else if (m.get_symbol(i) == p2)
        {
            m.set_symbol(i, p1);
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
        uint16_t helper = p.m_symbol - 1 - '0';
        helper = helper % m.m_player_count;
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
            std::cout << "Mit welchem Spieler wollen Sie tauschen?: ";
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
void check_moves(Map &map, Player &player)
{
    for (uint16_t c = 1; c < map.m_num_of_fields + 1; c++)
    {
        if (map.get_symbol(c) == player.m_symbol)
        {
            check_coordinate(c, map, player);
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
