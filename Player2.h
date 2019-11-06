#ifndef PLAYER2_H
#define PLAYER2_H
#include "Referee.h"
namespace P2 {
    bool play(State& s, int house_played);
    bool playHim(State& s, int house_played);
    int cpBoardScore(uint32_t board);
    int minimax(State& s, int depth, bool maxPlayer, int alpha, int beta);
}
#endif