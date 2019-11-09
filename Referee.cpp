#include "Player1.h"
#include "Player2.h"
#include "Referee.h"
#include "UnitTest.h"
#include <iostream>

using namespace std;

const int TURN_MAX = 200;
const int MAX = 1000;
const int MIN = -1000;

const int NUMBER_OF_GAMES = 20;

bool is_finished(State& s, int turn);
void print_end_game(State& s, int player, int turn);

int main() {
    int game = 0;
    int nbWonByP1 = 0;
    int nbWonByP2 = 0;
    while (game < NUMBER_OF_GAMES) {
        int turn = 0;
        bool current_player = 1;
        State s;
        s.me = 0b001000010000100001000010000100;
        s.him = 0b001000010000100001000010000100;
        s.me_score = 0;
        s.him_score = 0;
        //testAll();
        P2::initMCTS();
        cout << "New game" << endl;
        while (1) {
            int move = 0;
            if (current_player) {
                move = P1::minimax(s, 0, true, MIN, MAX);
            } else {
                move = P2::playLocal(s, turn);
            }
            P1::play(s, move);
            //print_end_game(s, current_player, turn);
            if (is_finished(s, turn)) {
                print_end_game(s, current_player, turn);
                if (current_player) {
                    nbWonByP1++;
                } else {
                    nbWonByP2++;
                }
                break;
            }
            swap(s.me, s.him);
            swap(s.me_score, s.him_score);
            current_player=!current_player;
            turn++;
        }
        cout << "End game" << endl;
        P2::destroy();
        game++;
    }
    cout << "Player 1 final score : " << nbWonByP1 << endl;
    cout << "Player 2 final score : " << nbWonByP2 << endl;
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