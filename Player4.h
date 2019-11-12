#ifndef PLAYER4_H
#define PLAYER4_H
#include "Referee.h"
namespace P4 {
    bool play(State& s, int house_played);
    bool playHim(State& s, int house_played);
    int cpBoardScore(uint32_t board);
    int minimax(State& s, int depth, bool maxPlayer, int alpha, int beta);
    int playLocal(State& s, int turn);
    void destroy();
    void initMCTS();
}
#endif