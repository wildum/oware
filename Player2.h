#include "Referee.h"
#ifndef PLAYER2_H
#define PLAYER2_H
namespace P2 {
    const int MAX = 1000;
    const int MIN = -1000;
    int maxDepth = 13;
    bool play(State& s, int house_played);
    bool playHim(State& s, int house_played);
    int cpBoardScore(uint32_t board);
    int minimax(State& s, int depth, bool maxPlayer, int alpha, int beta);
}
#endif