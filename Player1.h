#ifndef PLAYER1_H
#define PLAYER1_H
struct State {
    uint32_t me;
    uint32_t him;
    uint8_t me_score;
    uint8_t him_score;
};
bool play(State& s, int house_played);
bool playHim(State& s, int house_played);
int cpBoardScore(uint32_t board);
#endif