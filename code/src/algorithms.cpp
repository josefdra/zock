#include "algorithms.hpp"

int minimaxWithPruning(unsigned short &position, unsigned char depth, unsigned char &alpha, unsigned char &beta, bool maximizingPlayer, Map &map, Player &p)
{
    unsigned char eval;
    unsigned char maxEval;
    unsigned char minEval;
    unsigned char staticEval;
    unsigned short currChildPos;
    if (depth == 0)
    {
        return 1 /*staticEval at position*/

            ;
    } // @todo bewertung an position einbauen

    if (maximizingPlayer)
    {
        for (unsigned char child = 0; child < 8; child++)
        {
            currChildPos = p.m_valid_moves.eval = minimaxWithPruning(currChildPos, depth - 1, alpha, beta, false, map, p);
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
        for (unsigned char child = 0; child < 8; child++)
        {
            currChildPos = map.m_symbol_and_transitions.at(position).transitions.at(child);
            eval = minimaxWithPruning(currChildPos, depth - 1, alpha, beta, true, map);
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