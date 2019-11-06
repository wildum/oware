#ifndef PLAYER1_H
#define PLAYER1_H
#include "Referee.h"
namespace P1 {
    bool play(State& s, int house_played);
    bool playHim(State& s, int house_played);
    int cpBoardScore(uint32_t board);
    int minimax(State& s, int depth, bool maxPlayer, int alpha, int beta);
}
#endif