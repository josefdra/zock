#include "game.hpp"

// initialize Game
Game::Game(const std::string map_name) : m_map_name(map_name),
                                         m_map(map_name),
                                         m_player1('1'),
                                         m_player2('2'),
                                         m_player3('3'),
                                         m_player4('4'),
                                         m_player5('5'),
                                         m_player6('6'),
                                         m_player7('7'),
                                         m_player8('8')
{
    // initialize players
    m_player1.set_bombs_and_stones(m_map.m_initial_overwrite_stones, m_map.m_initial_bombs);
    m_player2.set_bombs_and_stones(m_map.m_initial_overwrite_stones, m_map.m_initial_bombs);
    m_player3.set_bombs_and_stones(m_map.m_initial_overwrite_stones, m_map.m_initial_bombs);
    m_player4.set_bombs_and_stones(m_map.m_initial_overwrite_stones, m_map.m_initial_bombs);
    m_player5.set_bombs_and_stones(m_map.m_initial_overwrite_stones, m_map.m_initial_bombs);
    m_player6.set_bombs_and_stones(m_map.m_initial_overwrite_stones, m_map.m_initial_bombs);
    m_player7.set_bombs_and_stones(m_map.m_initial_overwrite_stones, m_map.m_initial_bombs);
    m_player8.set_bombs_and_stones(m_map.m_initial_overwrite_stones, m_map.m_initial_bombs);
    run();
}
Game::~Game() {}

void Game::run()
{
    while (1)
    {
        m_map.print_map();
        check_moves(m_map, m_player1);
        std::cout << std::endl
                  << "------------------------ Reset ------------------------" << std::endl
                  << std::endl;
        m_map.read_hash_map(m_map_name);
    }
}