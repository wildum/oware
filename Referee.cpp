#include "Player1.h"
#include "Player2.h"
#include "Referee.h"
#include "UnitTest.h"
#include <iostream>

using namespace std;

const int TURN_MAX = 200;
const int MAX = 1000;
const int MIN = -1000;

bool is_finished(State& s, int turn, int current_player);
void print_end_game(State& s, int player, int turn);

int main() {
    int turn = 0;
    bool current_player = 1;
    State s;
    s.me = 0b001000010000100001000010000100;
    s.him = 0b001000010000100001000010000100;
    s.me_score = 0;
    s.him_score = 0;
    testAll();
    while (1) {
        int move = 0;
        if (current_player) {
            move = P1::minimax(s, 0, true, MIN, MAX);
        } else {
            move = P2::minimax(s, 0, true, MIN, MAX);
        }
        P1::play(s, move);
        if (is_finished(s, turn, current_player)) {
            print_end_game(s, current_player, turn);
            break;
        }
        swap(s.me, s.him);
        swap(s.me_score, s.him_score);
        current_player=!current_player;
        turn++;
    }
}

inline bool is_finished(State& s, int turn) {
    return !(turn != 200 && s.him && s.me_score < 25 && s.him_score < 25);
}

void print_end_game(State& s, int player, int turn) {
    if (player) {
        cout << "Player 1 score : " << unsigned(s.me_score) << endl;
        cout << "Player 2 score : " << unsigned(s.him_score) << endl;
        cout << "Turn : " << turn << endl;
    } else {
        cout << "Player 1 score : " << unsigned(s.him_score) << endl;
        cout << "Player 2 score : " << unsigned(s.me_score) << endl;
        cout << "Turn : " << turn << endl;
    }
}