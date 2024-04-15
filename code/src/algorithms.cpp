#include "algorithms.hpp"

int minimaxWithPruning(uint16_t position, uint8_t depth, uint8_t alpha, uint8_t beta, bool maximizingPlayer, Map &map, Player &p)
{
    uint8_t eval;
    uint8_t maxEval;
    uint8_t minEval;
    uint16_t nextChildPos;

    Map mapcopy = map;
    Player pcopy = p;
    pcopy.staticMapEval = p.staticMapEval;
    pcopy.m_valid_moves = p.m_valid_moves;
    mapcopy.m_symbol_and_transitions = map.m_symbol_and_transitions;

    if (depth == 0)
    {
        std::cout << "bestpos: " << position << std::endl;
        return pcopy.staticMapEval.at(position);
    } // @todo bewertung an position einbauen

    if (maximizingPlayer)
    {
        for (auto child : p.m_valid_moves)
        {
            execute_move(child.first, pcopy, mapcopy);
            check_moves(mapcopy, pcopy);
            eval = minimaxWithPruning(child.first, depth - 1, alpha, beta, false, mapcopy, pcopy);
            maxEval = std::max(maxEval, eval);
            alpha = std::max(alpha, eval);
            if (beta <= alpha)
            {
                break;
            }
            return maxEval;
        }
    }
    else
    {
        for (auto child : pcopy.m_valid_moves)
        {
            execute_move(child.first, pcopy, mapcopy);
            check_moves(mapcopy, pcopy);
            eval = minimaxWithPruning(child.first, depth - 1, alpha, beta, true, mapcopy, pcopy);
            minEval = std::max(minEval, eval);
            beta = std::max(beta, eval);
            if (beta <= alpha)
            {
                break;
            }
            return minEval;
        }
    }
}