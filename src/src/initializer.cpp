#include "initializer.hpp"
#include "board.hpp"
#include "move_generator.hpp"
#include "logging.hpp"
#include "timer.hpp"

/**
 * @brief initializer.cpp is responsible for reading in the map information and the correct output as well as calculating the correct neighbourhood relationships
 *
 */

Initializer::Initializer() : wall_sets(),
                             before_wall_sets(),
                             before_before_wall_sets(),
                             corners_and_walls(),
                             next_coords()
{

    m_numbers = std::vector<char>();
    m_transitions = std::vector<uint16_t>();
    m_height = 0;
    m_width = 0;
    m_player_count = 0;
    m_strength = 0;
    m_initial_overwrite_stones = 0;
    m_initial_bombs = 0;
    m_num_of_fields = 0;
}

Initializer::~Initializer() {}

/**
 * @brief Calculation of all possible transitions and their coordinates depending on their direction
 *
 * @param c current coordinate
 */
void Initializer::check_neighbours(uint16_t c)
{
    if (c > m_width && get_symbol(c - m_width) != '-') // checks if there is a field above
        set_transition(c, UP, (c - m_width) * COORD_TO_DIR_OFFSET);

    if (c % m_width != 0 && c > m_width && get_symbol(c - m_width + 1) != '-') // checks if there is a field top right
        set_transition(c, UPPER_RIGHT, (c - m_width + 1) * COORD_TO_DIR_OFFSET + UPPER_RIGHT);

    if (c % m_width != 0 && get_symbol(c + 1) != '-') // checks if there is a field to the right
        set_transition(c, RIGHT, (c + 1) * COORD_TO_DIR_OFFSET + RIGHT);

    if (c % m_width != 0 && c <= m_width * (m_height - 1) && get_symbol(c + m_width + 1) != '-') // checks if there is a field bottom right
        set_transition(c, LOWER_RIGHT, (c + m_width + 1) * COORD_TO_DIR_OFFSET + LOWER_RIGHT);

    if (c <= m_width * (m_height - 1) && get_symbol(c + m_width) != '-') // checks if there is a field below
        set_transition(c, DOWN, (c + m_width) * COORD_TO_DIR_OFFSET + DOWN);

    if (c % m_width != 1 && c <= m_width * (m_height - 1) && get_symbol(c + m_width - 1) != '-') // check if there is a field bottom left
        set_transition(c, LOWER_LEFT, (c + m_width - 1) * COORD_TO_DIR_OFFSET + LOWER_LEFT);

    if (c % m_width != 1 && get_symbol(c - 1) != '-') // checks if there is a field to the left
        set_transition(c, LEFT, (c - 1) * COORD_TO_DIR_OFFSET + LEFT);

    if (c % m_width != 1 && c > m_width && get_symbol(c - m_width - 1) != '-') // checks if there is a field top left
        set_transition(c, UPPER_LEFT, (c - m_width - 1) * COORD_TO_DIR_OFFSET + UPPER_LEFT);
}

/**
 *
 * HERE ARE JUST INITIALIZATIONS AND SETTER AND GETTER
 *
 */

void Initializer::set_symbol(uint16_t c, unsigned char s)
{
    m_numbers[c] = s;
}

void Initializer::set_transition(uint16_t c, uint8_t d, uint16_t t)
{
    m_transitions[(c - 1) * NUM_OF_DIRECTIONS + d] = t;
}

char Initializer::get_symbol(uint16_t c)
{
    return m_numbers[c];
}

uint16_t Initializer::get_transition(uint16_t c, uint8_t d)
{
    return m_transitions[(c - 1) * NUM_OF_DIRECTIONS + d] / COORD_TO_DIR_OFFSET;
}

uint8_t Initializer::get_direction(uint16_t c, uint8_t d)
{
    return m_transitions[(c - 1) * NUM_OF_DIRECTIONS + d] % COORD_TO_DIR_OFFSET;
}

uint16_t Initializer::get_height()
{
    return m_height;
}

uint16_t Initializer::get_width()
{
    return m_width;
}

uint16_t Initializer::get_player_count()
{
    return m_player_count;
}

uint16_t Initializer::get_strength()
{
    return m_strength;
}

uint16_t Initializer::get_num_of_fields()
{
    return m_num_of_fields;
}

uint16_t Initializer::get_initial_overwrite_stones()
{
    return m_initial_overwrite_stones;
}

uint16_t Initializer::get_initial_bombs()
{
    return m_initial_bombs;
}

std::vector<uint16_t> Initializer::get_transitions()
{
    return m_transitions;
}

uint8_t Initializer::get_reverse_direction(uint8_t d)
{
    return (d + 4) % NUM_OF_DIRECTIONS;
}

/// @brief expands transitions into direction and saves next coordinates to vector
/// @param c current coordinate
/// @param next_coords_pos position in next_coords vector
void Initializer::expand_coord(uint16_t c, uint8_t d, uint16_t next_coords_pos)
{
    uint16_t next_coord = get_transition(c, d);
    uint8_t next_direction = get_direction(c, d);
    uint16_t prev_coord = c;
    uint8_t prev_direction = d;
    while (next_coord != 0 && next_coord != c)
    {
        if (next_coord == prev_coord && get_reverse_direction(prev_direction) == next_direction)
            break;
        prev_coord = next_coord;
        prev_direction = next_direction;
        next_coords[next_coords_pos].push_back(next_coord);
        next_coord = get_transition(prev_coord, prev_direction);
        next_direction = get_direction(prev_coord, prev_direction);
    }
}

/// @brief initializes all possible fields
void Initializer::init_next_coords()
{
    next_coords.resize((m_num_of_fields - 1) * 8);
    for (uint16_t c = 1; c < m_num_of_fields; c++)
        for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d++)
            expand_coord(c, d, (c - 1) * NUM_OF_DIRECTIONS + d);
}

/**
 * @brief reads the input and sets all the information required for the game map
 *
 * @param inputfile mapfile to read
 */
void Initializer::read_map(std::stringstream mapfile)
{
    char temp;
    mapfile >> m_player_count >> m_initial_overwrite_stones >> m_initial_bombs >> m_strength >> m_height >> m_width;
    m_num_of_fields = m_height * m_width + 1;
    m_transitions.resize((m_num_of_fields - 1) * NUM_OF_DIRECTIONS + 1, 0);
    m_numbers.resize(m_num_of_fields, 0);
    // every coordinate gets a symbol and it's neighbours are being set
    for (int c = 1; c < m_num_of_fields; c++)
    {
        mapfile >> temp;
        set_symbol(c, temp);
    }
    for (int c = 1; c < m_num_of_fields; c++)
        if (get_symbol(c) != '-')
            check_neighbours(c);

    uint16_t x1, y1, r1, x2, y2, r2, pos1, pos2, pos1r, pos2r;
    while (mapfile >> x1)
    {
        // special transitions are being set in same format to appear as neighbours
        unsigned char trash;
        mapfile >> y1 >> r1 >> trash >> trash >> trash >> x2 >> y2 >> r2;
        x1++;
        x2++;
        pos1 = (x1) + (y1)*m_width;
        pos2 = (x2) + (y2)*m_width;
        pos1r = pos1 * 10 + get_reverse_direction(r1);
        pos2r = pos2 * 10 + get_reverse_direction(r2);
        set_transition(pos1, r1, pos2r);
        set_transition(pos2, r2, pos1r);
    }
    init_next_coords();
    init_possible_fields();
}

/// @brief converts 1D coordinate to 2D coordinates
/// @param _1D_coord 1D coordinate
/// @param x x coordinate
/// @param y y coordinate
void Initializer::one_dimension_2_second_dimension(uint16_t _1D_coord, uint8_t &x, uint8_t &y)
{
    _1D_coord % m_width == 0 ? x = m_width - 1 : x = _1D_coord % m_width - 1;
    y = (_1D_coord - (x + 1)) / m_width;
}

/// @brief converts 2D coordinates to 1D coordinate
/// @param x x coordinate
/// @param y y coordinate
/// @return 1D coordinate
uint16_t Initializer::two_dimension_2_one_dimension(uint8_t x, uint8_t y)
{
    return (x + 1 + y * m_width);
}

/// @brief checks if the character is a player
/// @param c character to check
/// @return true if it is a player
bool Initializer::check_players(char c)
{
    std::array<unsigned char, 8> player_symbols{'1', '2', '3', '4', '5', '6', '7', '8'};
    bool var = false;
    for (uint8_t i = 0; i < player_symbols.size(); i++)
        if (c == player_symbols[i])
            var = true;

    return var;
}

/// @brief sets values from vector to bitsets
/// @param board board to set values
/// @param c coordinate
void Initializer::set_values(Board &board, uint16_t c)
{
    if (get_symbol(c) == '0')
        board.board_sets[EMPTY].set(c);

    else if (get_symbol(c) == '-')
    {
        board.board_sets[MINUS].set(c);
        board.decrement_not_minus_fields();
    }

    else if (get_symbol(c) == 'i')
    {
        board.board_sets[EMPTY].set(c);
        board.board_sets[I].set(c);
    }
    else if (get_symbol(c) == 'c')
    {
        board.board_sets[EMPTY].set(c);
        board.board_sets[C].set(c);
    }
    else if (get_symbol(c) == 'b')
    {
        board.board_sets[EMPTY].set(c);
        board.board_sets[B].set(c);
    }
    else if (get_symbol(c) == 'x')
        board.board_sets[X].set(c);

    else if (check_players(get_symbol(c)))
        board.player_sets[get_symbol(c) - '0' - 1].set(c);
}

/// @brief initializes wall values of the board
/// @param board board to set values
/// @param checked checked fields
void Initializer::init_wall_values(Board &board, std::bitset<MAX_NUM_OF_FIELDS> &checked)
{
    for (uint16_t c = 1; c < board.get_num_of_fields(); c++)
        if (checked.test(c))
        {
            uint8_t counter = 0;
            uint8_t most = 0;
            for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d++)
            {
                uint16_t next_coord = get_transition(c, d);
                uint8_t prev_dir = d;
                d = (d + 1) % NUM_OF_DIRECTIONS;
                while (next_coord == 0 && d != prev_dir)
                {
                    counter++;
                    if (counter > most)
                        most = counter;

                    next_coord = get_transition(c, d);
                    d = (d + 1) % NUM_OF_DIRECTIONS;
                }
                counter = 0;
                d = prev_dir;
            }
            if (most == 3)
                wall_sets[THREE_WALLS].set(c);
            else if (most == 4)
                wall_sets[FOUR_WALLS].set(c);
            else if (most == 5)
                wall_sets[FIVE_WALLS].set(c);

            if (most > 3)
                board.fixed_protected_fields.set(c);
        }

    corners_and_walls = wall_sets[THREE_WALLS] | wall_sets[FOUR_WALLS] | wall_sets[FIVE_WALLS];
    init_before_wall_values(board);
}

/// @brief initializes values before wall values
/// @param board board to set values
void Initializer::init_before_wall_values(Board &board)
{
    for (uint16_t c = 1; c < board.get_num_of_fields(); c++)
        if (wall_sets[THREE_WALLS].test(c))
            for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d++)
            {
                uint16_t next_coord = get_transition(c, d);
                if (next_coord != 0)
                    before_wall_sets[THREE_WALLS].set(next_coord);
            }
        else if (wall_sets[FOUR_WALLS].test(c))
            for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d++)
            {
                uint16_t next_coord = get_transition(c, d);
                if (next_coord != 0)
                    before_wall_sets[FOUR_WALLS].set(next_coord);
            }
        else if (wall_sets[FIVE_WALLS].test(c))
            for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d++)
            {
                uint16_t next_coord = get_transition(c, d);
                if (next_coord != 0)
                    before_wall_sets[FIVE_WALLS].set(next_coord);
            }
    init_before_before_wall_values(board);
}

/// @brief initializes values before before wall values
/// @param board board to set values
void Initializer::init_before_before_wall_values(Board &board)
{
    for (uint16_t c = 1; c < board.get_num_of_fields(); c++)
    {
        if (before_wall_sets[THREE_WALLS].test(c))
            for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d++)
            {
                uint16_t next_coord = get_transition(c, d);
                if (next_coord != 0)
                    before_before_wall_sets[THREE_WALLS].set(next_coord);
            }
        if (before_wall_sets[FOUR_WALLS].test(c))
            for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d++)
            {
                uint16_t next_coord = get_transition(c, d);
                if (next_coord != 0)
                    before_before_wall_sets[FOUR_WALLS].set(next_coord);
            }
        if (before_wall_sets[FIVE_WALLS].test(c))
            for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d++)
            {
                uint16_t next_coord = get_transition(c, d);
                if (next_coord != 0)
                    before_before_wall_sets[FIVE_WALLS].set(next_coord);
            }
    }
}

/// @brief initializes wall values of the board
/// @param board board to set values
/// @param checked checked fields
bool Initializer::get_walls(Board &board, std::bitset<MAX_NUM_OF_FIELDS> &checked)
{
    for (uint16_t c = 1; c < m_num_of_fields; c++)
        for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d++)
            if (get_transition(c, d) == 0 && !board.board_sets[MINUS].test(c))
            {
                checked.set(c);
                break;
            }

    if (checked.count() == 0)
        return false;
    else
    {
        init_wall_values(board, checked);
        return true;
    }
}

/// @brief checkes if field is protected and expands it
/// @param board board to set values
/// @param player player to check
/// @param coord coordinate to check
void Initializer::check_if_protected_field(Board &board, uint8_t player, uint16_t coord)
{
    uint8_t counter = 0;
    uint8_t most = 0;
    for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d++)
    {
        uint16_t next_coord = get_transition(coord, d);
        uint8_t prev_dir = d;
        d = (d + 1) % NUM_OF_DIRECTIONS;
        while ((next_coord == 0 || board.player_sets[player].test(next_coord)) && d != prev_dir)
        {
            counter++;
            if (counter > most)
                most = counter;
            next_coord = get_transition(coord, d);
            d = (d + 1) % NUM_OF_DIRECTIONS;
        }
        counter = 0;
        d = prev_dir;
    }

    if (most > 3)
    {
        board.protected_fields[player].set(coord);
        for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d++)
        {
            uint16_t next_coord = get_transition(coord, d);
            if (next_coord != 0 && board.player_sets[player].test(next_coord) && !board.protected_fields[player].test(next_coord))
                check_if_protected_field(board, player, next_coord);
        }
    }
}

/// @brief expands protected fields if possible
/// @param board board to set values
/// @param player player to check
void Initializer::expand_protected_fields(Board &board, uint8_t player)
{
    board.protected_fields[player] = board.fixed_protected_fields & board.player_sets[player];
    for (uint16_t c = 1; c < board.get_num_of_fields(); c++)
        if (board.fixed_protected_fields.test(c))
            for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d++)
            {
                uint16_t next_coord = get_transition(c, d);
                if (next_coord != 0 && board.player_sets[player].test(next_coord) && !board.protected_fields[player].test(next_coord))
                    check_if_protected_field(board, player, next_coord);
            }
}

/// @brief expands community if possible
/// @param board board to set values
/// @param community community to expand
/// @param c coordinate to check
/// @param checked_fields checked fields
void Initializer::expand_community(Board &board, std::bitset<MAX_NUM_OF_FIELDS> &community, uint16_t c, std::bitset<MAX_NUM_OF_FIELDS> &checked_fields)
{
    for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d++)
    {
        uint16_t next_coord = get_transition(c, d);
        if (next_coord != 0 && !board.board_sets[EMPTY].test(next_coord) && !community.test(next_coord))
        {
            if (std::get<0>(board.start_end_communities.back()) > next_coord)
                std::get<0>(board.start_end_communities.back()) = next_coord;
            if (std::get<1>(board.start_end_communities.back()) < next_coord)
                std::get<1>(board.start_end_communities.back()) = next_coord;
            community.set(next_coord);
            if (checked_fields.test(next_coord))
                continue;
            else
            {
                checked_fields.set(next_coord);
                expand_community(board, community, next_coord, checked_fields);
            }
        }
    }
}

/// @brief initializes frames of the board
/// @param board board to set values
void Initializer::init_frames(Board &board)
{
    for (uint16_t c = 1; c < get_num_of_fields(); c++)
        for (uint8_t i = 0; i < board.get_num_of_communities(); i++)
            if (board.communities[i].test(c))
                for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d++)
                {
                    uint16_t next_coord = get_transition(c, d);
                    if (next_coord != 0 && board.board_sets[EMPTY].test(next_coord))
                    {
                        if (std::get<0>(board.start_end_frames[i]) > next_coord)
                            std::get<0>(board.start_end_frames[i]) = next_coord;
                        if (std::get<1>(board.start_end_frames[i]) < next_coord)
                            std::get<1>(board.start_end_frames[i]) = next_coord;
                        board.frames[i].set(next_coord);
                    }
                }

    for (uint8_t i = 0; i < board.get_num_of_communities(); i++)
        if (board.frames[i].count() == 0)
            board.communities[i].reset();
}

/// @brief removes double communities
/// @param board board to set values
void Initializer::remove_double_communities(Board &board)
{
    std::vector<std::bitset<MAX_NUM_OF_FIELDS>> temp_communities;
    for (uint8_t i = 0; i < board.get_num_of_communities(); i++)
    {
        bool not_empty = false;
        if ((board.communities[i] & board.board_sets[X]).count() != 0)
            not_empty = true;

        for (auto &p : board.player_sets)
            if ((board.communities[i] & p).count() != 0)
                not_empty = true;

        if (!not_empty)
            board.communities[i].reset();
    }
    for (uint8_t i = 0; i < board.get_num_of_communities(); i++)
        for (uint8_t j = 0; j < board.get_num_of_communities(); j++)
            if (i != j && (board.communities[i] & board.communities[j]).count() != 0)
            {
                board.communities[i] |= board.communities[j];
                board.communities[j].reset();
            }

    for (auto &community : board.communities)
        if (community.count() != 0)
            temp_communities.push_back(community);

    board.communities = temp_communities;
}

/// @brief initializes number of players in communities
/// @param board board to set values
void Initializer::init_players_in_communities_count(Board &board)
{
    for (auto &community : board.communities)
    {
        uint8_t count = 0;
        for (uint8_t i = 0; i < m_player_count; i++)
            if ((community & board.player_sets[i]).count() != 0)
                count++;

        board.num_of_players_in_community.push_back(count);
    }
}

/// @brief initializes communities of the board
/// @param board board to set values
void Initializer::init_communities(Board &board)
{
    std::bitset<MAX_NUM_OF_FIELDS> all_players;
    std::bitset<MAX_NUM_OF_FIELDS> checked_fields;
    for (auto &player : board.player_sets)
        all_players |= player;

    all_players |= board.board_sets[X];

    for (uint16_t c = 1; c < m_num_of_fields; c++)
        if (all_players.test(c) && !checked_fields.test(c))
        {
            board.communities.push_back(std::bitset<MAX_NUM_OF_FIELDS>(0));
            board.communities.back().set(c);
            board.start_end_communities.push_back(std::make_tuple(c, c));
            expand_community(board, board.communities.back(), c, checked_fields);
        }

    remove_double_communities(board);
    board.frames.resize(board.get_num_of_communities());
    board.start_end_frames.resize(board.get_num_of_communities(), std::make_tuple(65000, 0));
    init_frames(board);
    init_players_in_communities_count(board);
}

/// @brief initializes static evaluation of the board
/// @param board board to set values
void Initializer::init_static_evaluation(Board &board)
{
    std::bitset<MAX_NUM_OF_FIELDS> all_stones;
    for (auto &player : board.player_sets)
        all_stones |= player;
    all_stones |= board.board_sets[X];
    for (uint16_t c = 1; c < board.get_num_of_fields(); c++)
    {
        if (board.board_sets[MINUS].test(c) || all_stones.test(c))
            continue;
        if (wall_sets[THREE_WALLS].test(c))
            board.static_evaluation[c] += THREE_WALLS_VALUE;
        if (wall_sets[FOUR_WALLS].test(c))
            board.static_evaluation[c] += FOUR_WALLS_VALUE;
        if (wall_sets[FIVE_WALLS].test(c))
            board.static_evaluation[c] += FIVE_WALLS_VALUE;
        if (before_wall_sets[THREE_WALLS].test(c))
            board.static_evaluation[c] += BEFORE_THREE_WALLS_VALUE;
        if (before_wall_sets[FOUR_WALLS].test(c))
            board.static_evaluation[c] += BEFORE_FOUR_WALLS_VALUE;
        if (before_wall_sets[FIVE_WALLS].test(c))
            board.static_evaluation[c] += BEFORE_FIVE_WALLS_VALUE;
        if (before_before_wall_sets[THREE_WALLS].test(c))
            board.static_evaluation[c] += BEFORE_BEFORE_THREE_WALLS_VALUE;
        if (before_before_wall_sets[FOUR_WALLS].test(c))
            board.static_evaluation[c] += BEFORE_BEFORE_FOUR_WALLS_VALUE;
        if (before_before_wall_sets[FIVE_WALLS].test(c))
            board.static_evaluation[c] += BEFORE_BEFORE_FIVE_WALLS_VALUE;
    }
}

/// @brief calculates which fields are before protected fields
/// @param board board to set values
void Initializer::calculate_before_protected_fields(Board &board)
{
    for (uint8_t player = 0; player < get_player_count(); player++)
    {
        board.before_protected_fields[player].reset();
        for (uint16_t c = 1; c < m_num_of_fields; c++)
            if (board.protected_fields[player].test(c))
                for (uint8_t d = 0; d < NUM_OF_DIRECTIONS; d++)
                {
                    uint16_t next_coord = get_transition(c, d);
                    if (next_coord != 0 && board.board_sets[EMPTY].test(next_coord))
                        board.before_protected_fields[player].set(next_coord);
                }
    }
}

void Initializer::check_if_special_map(Board &board)
{
    if (m_strength == 255)
        for (uint16_t c = 1; c < m_num_of_fields; c++)
            if (board.board_sets[X].test(c))
            {
                uint16_t next_coord = get_transition(c, RIGHT);
                if (next_coord != 0 && board.board_sets[X].test(next_coord))
                {
                    uint16_t next_next_coord = get_transition(next_coord, RIGHT);
                    if (next_next_coord != 0 && board.board_sets[B].test(next_next_coord))
                    {
                        uint8_t x, y;
                        board.special_moves.push_back((uint32_t)16 << TWO_BYTES | (uint32_t)14 << BYTE | 0);
                        one_dimension_2_second_dimension(next_next_coord, x, y);
                        board.special_moves.push_back((uint32_t)x << TWO_BYTES | (uint32_t)y << BYTE | 20);
                        one_dimension_2_second_dimension(c, x, y);
                        board.special_moves.push_back((uint32_t)x << TWO_BYTES | (uint32_t)y << BYTE | 0);
                        board.special_coord = c;
                        return;
                    }
                }
            }
}

/// @brief initializes boards and players
/// @return board
Board Initializer::init_boards_and_players()
{
    Board ret_board(*this);

    for (uint16_t c = 1; c < m_num_of_fields; c++)
        set_values(ret_board, c);

    for (uint8_t i = 0; i < get_player_count(); i++)
    {
        ret_board.set_overwrite_stones(i, m_initial_overwrite_stones);
        ret_board.set_bombs(i, m_initial_bombs);
    }

    std::bitset<MAX_NUM_OF_FIELDS> checked;
    get_walls(ret_board, checked);
    for (uint8_t p = 0; p < get_player_count(); p++)
        expand_protected_fields(ret_board, p);

    calculate_before_protected_fields(ret_board);

    init_communities(ret_board);
    for (uint8_t p = 0; p < get_player_count(); p++)
        ret_board.valid_moves[p].resize(ret_board.get_num_of_communities(), std::bitset<MAX_NUM_OF_FIELDS>(0));

    init_static_evaluation(ret_board);
    check_if_special_map(ret_board);
    return ret_board;
}

/// @brief generates transitions
void Initializer::generate_transitions()
{
    std::vector<std::array<uint16_t, 2>> tr;
    std::vector<std::array<uint16_t, 6>> output;
    std::array<uint16_t, 2> temp = {0, 0};
    for (int i = 1; i < m_num_of_fields; i++)
    {
        if (m_numbers[i] != '-')
        {
            for (int j = 0; j < 8; j++)
            {
                if (m_transitions[(i - 1) * 8 + j] == 0)
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
        tr.back()[0] % m_width == 0 ? temp_bigger[0] = m_width : temp_bigger[0] = tr.back()[0] % m_width;
        temp_bigger[1] = (tr.back()[0] - 1) / m_width + 1;
        temp_bigger[2] = tr.back()[1];
        tr.pop_back();
        tr.back()[0] % m_width == 0 ? temp_bigger[3] = m_width : temp_bigger[3] = tr.back()[0] % m_width;
        temp_bigger[4] = (tr.back()[0] - 1) / m_width + 1;
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

/// @brief initializes possible fields
void Initializer::init_possible_fields()
{
    for (uint16_t c = 1; c < m_num_of_fields; c++)
        if (get_symbol(c) != '-')
            m_possible_fields++;
}

/// @brief returns possible fields
uint16_t Initializer::get_sum_possible_fields()
{
    return m_possible_fields;
}