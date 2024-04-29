#include "algorithms.hpp"

// this algorithm could be optimized by adding functionality to check if enemy has direct influence in our player and if he can "attack" us, else his
// possible turns could be ignored
int minimaxOrParanoiaWithPruning(uint16_t position, uint8_t depth, int alpha, int beta, bool maximizingPlayer, Map &map, Game &game, uint8_t &playersTurn, uint8_t &game_phase, Player &myPlayer, uint16_t &turns)
{
    int maxEval = std::numeric_limits<int>::min();
    int minEval = std::numeric_limits<int>::max();
    turns++;

    if (depth == 0)
    {
        return game.evaluate_board(game_phase, myPlayer, map);
    }

    // map.print_map();
    Player currPlayer = game.m_players[playersTurn];
    check_moves(map, currPlayer);
    map.m_symbols = map.m_symbols;
    uint8_t nextPlayer = ((playersTurn + 1) % map.m_player_count);

    // check if current player has any influence on our player else skip his moves
    if (map.m_player_count > 2)
    {
        if (currPlayer.m_symbol != myPlayer.m_symbol && !affectsMyPlayer(currPlayer, myPlayer, map))
        {
            minimaxOrParanoiaWithPruning(position, depth, alpha, beta, maximizingPlayer, map, game, nextPlayer, game_phase, myPlayer, turns);
        }
    }
    // if there are no moves left return the current value for it's position
    if (currPlayer.m_valid_moves.size() < 1)
    {
        return game.evaluate_board(game_phase, myPlayer, map);
    }

    if (maximizingPlayer)
    {

        for (auto &child : currPlayer.m_valid_moves)
        {

            Map mapcopy = map;
            char curr_symbol = mapcopy.get_symbol(child.first);
            uint8_t special = checkForSpecial(curr_symbol);
            execute_move(child.first, special, currPlayer, mapcopy);
            int eval = minimaxOrParanoiaWithPruning(child.first, depth - 1, alpha, beta, false, mapcopy, game, nextPlayer, game_phase, myPlayer, turns);
            maxEval = std::max(maxEval, eval);
            alpha = std::max(alpha, eval);
            if (beta <= alpha)
            {
                break;
            }
        }
        return maxEval;
    }

    else
    {

        for (auto child : currPlayer.m_valid_moves)
        {

            Map mapcopy = map;
            int eval;
            char curr_symbol = mapcopy.get_symbol(child.first);
            uint8_t special = checkForSpecial(curr_symbol);
            execute_move(child.first, special, currPlayer, mapcopy);
            // initializes paranoia
            if (game.m_players[playersTurn].m_symbol == myPlayer.m_symbol)
            {
                eval = minimaxOrParanoiaWithPruning(child.first, depth - 1, alpha, beta, true, mapcopy, game, nextPlayer, game_phase, myPlayer, turns);
            }
            else
            {
                eval = minimaxOrParanoiaWithPruning(child.first, depth - 1, alpha, beta, false, mapcopy, game, nextPlayer, game_phase, myPlayer, turns);
            }

            minEval = std::min(minEval, eval);
            beta = std::min(beta, eval);
            if (beta <= alpha)
            {
                break;
            }
        }
        return minEval;
    }
}

int getBestPosition(uint8_t &playernum, Map &map, uint8_t &depth, uint8_t &game_phase, Game &game)
{
    auto start = std::chrono::high_resolution_clock::now();
    uint16_t bestEval = -INFINITY;
    uint16_t bestCoord = 0;
    Player p = game.m_players[playernum];
    check_moves(map, p);
    uint8_t nextPlayer = ((playernum + 1) % map.m_player_count);
    uint16_t tried_turns = 0;
    for (auto possibleMove : p.m_valid_moves)
    {
        Map mapcopy = map;
        char curr_symbol = mapcopy.get_symbol(possibleMove.first);
        uint8_t special = checkForSpecial(curr_symbol);
        execute_move(possibleMove.first, special, p, mapcopy);
        uint16_t currEval = minimaxOrParanoiaWithPruning(possibleMove.first, depth - 1, -INT32_MAX, INT32_MAX, true, mapcopy, game, nextPlayer, game_phase, p, tried_turns);
        if (currEval > bestEval)
        {
            bestEval = currEval;
            bestCoord = possibleMove.first;
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "tried " << tried_turns << " turns"
              << " and this took " << duration.count() << " seconds" << std::endl;
    return bestCoord;
}
