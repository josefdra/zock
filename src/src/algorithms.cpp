#include "algorithms.hpp"

char get_symbol(std::vector<char> &m, uint16_t c)
{
    return m[c];
}

// this algorithm could be optimized by adding functionality to check if enemy has direct influence in our player and if he can "attack" us, else his
// possible turns could be ignored
int minimaxOrParanoidWithPruning(uint8_t depth, int alpha, int beta, bool maximizingPlayer, std::vector<char> &currMap, Map &map, uint8_t &playersTurn, uint8_t &game_phase, Player &myPlayer, uint16_t &turns)
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
    // map.m_symbols = map.m_symbols;
    uint8_t nextPlayer = ((playersTurn + 1) % map.m_player_count);

    // check if current player has any influence on our player else skip his moves
    if (map.m_player_count > 2)
    {
        if (currPlayer.m_symbol != myPlayer.m_symbol && !affectsMyPlayer(currPlayer, myPlayer, map))
        {
            minimaxOrParanoidWithPruning(position, depth, alpha, beta, maximizingPlayer, map, game, nextPlayer, game_phase, myPlayer, turns);
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
            std::vector<char> mapcopy = child.second;
            int eval = minimaxOrParanoidWithPruning(child.first, depth - 1, alpha, beta, false, mapcopy, game, nextPlayer, game_phase, myPlayer, turns);
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

        for (auto &child : currPlayer.m_valid_moves)
        {

            Map mapcopy = map;
            int eval;
            char curr_symbol = mapcopy.get_symbol(child.first);
            uint8_t special = checkForSpecial(curr_symbol);
            execute_move(child.first, special, currPlayer, mapcopy);
            // initializes paranoia
            if (game.m_players[playersTurn].m_symbol == myPlayer.m_symbol)
            {
                eval = minimaxOrParanoidWithPruning(child.first, depth - 1, alpha, beta, true, mapcopy, game, nextPlayer, game_phase, myPlayer, turns);
            }
            else
            {
                eval = minimaxOrParanoidWithPruning(child.first, depth - 1, alpha, beta, false, mapcopy, game, nextPlayer, game_phase, myPlayer, turns);
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
