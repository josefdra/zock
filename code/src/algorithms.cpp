#include "algorithms.hpp"

int minimaxWithPruning(uint16_t position, uint8_t depth, uint8_t alpha, uint8_t beta, bool maximizingPlayer, Map &map, Player &p)
{
    uint8_t eval;
    uint8_t maxEval;
    uint8_t minEval;
    uint16_t nextChildPos;
    if (depth == 0)
    {
        return position;
    } // @todo bewertung an position einbauen

    if (maximizingPlayer)
    {
        for (auto child : p.m_valid_moves)
        {
            check_coordinate(child.first, map, p, false);
            eval = minimaxWithPruning(child.first, depth - 1, alpha, beta, false, map, p);
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
        for (auto child : p.m_valid_moves)
        {
            check_coordinate(child.first, map, p, false);
            eval = minimaxWithPruning(child.first, depth - 1, alpha, beta, true, map, p);
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