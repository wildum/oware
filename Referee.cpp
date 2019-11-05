#include "Player1.h"
#include "Player2.h"
#include "Referee.h"
#include <iostream>

using namespace std;

const int TURN_MAX = 200;

bool is_finished(State& s, int turn);
void print_end_game(State& s, int player);

int main() {
    int turn = 0;
    int current_player = 1;
    State s;
    s.me = 0b001000010000100001000010000100;
    s.him = 0b001000010000100001000010000100;
    s.me_score = 0;
    s.him_score = 0;
    while (1) {
        int move = 0;
        if (current_player) {
            move = P1::minimax(s, P1::maxDepth, true, P1::MIN, P1::MAX);
        } else {
            move = P2::minimax(s, P2::maxDepth, true, P2::MIN, P2::MAX);
        }
        P1::play(s, move);
        if (is_finished(s, turn)) {
            print_end_game(s, current_player);
            break;
        }
        swap(s.me, s.him);
        current_player*=-1;
        turn++;
    }
}

inline bool is_finished(State& s, int turn) {
    return !(turn != 200 && s.him && s.me_score >= 25 && s.him_score >= 25);
}

void print_end_game(State& s, int player) {
    if (player == 1) {
        cout << "Player 1 score : " << s.me_score << endl;
        cout << "Player 2 score : " << s.him_score << endl;
    } else {
        cout << "Player 1 score : " << s.him_score << endl;
        cout << "Player 2 score : " << s.me_score << endl;
    }
}