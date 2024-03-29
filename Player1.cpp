/*
    MiniMax
*/

#include <iostream>
#include <bitset>
#include <algorithm>
#include <chrono>
#include "UnitTest.h"
#include "Referee.h"
#include "Player1.h"

#pragma GCC optimize("-O3")
#pragma GCC optimize("inline")
#pragma GCC optimize("omit-frame-pointer")
#pragma GCC optimize("unroll-loops")
#undef __NO_INLINE__
#define __OPTIMIZE__ 1


using namespace std;
using namespace std::chrono;

namespace P1 {
    bool play(State& s, int house_played);
    bool playHim(State& s, int house_played);
    int minimax(State& s, int depth, bool maxPlayer, int alpha, int beta);
    int cpBoardScore(uint32_t board);

    const uint32_t FULL_HOUSE = 34636833;

    uint8_t my_new_score = 0;
    uint8_t prev_seed = 48;
    uint8_t prev_score = 0;
    uint8_t prev_him_score = 0;
    uint8_t potential_score = 0;

    const int MAX = 1000;
    const int MIN = -1000;
    int maxDepth = 12;

    int malus = 0;
    int value = 0;
    int scoreBoard = 0;


    const uint32_t values[] = {0, 1, 33, 1057, 33825, 1082401, 34636833 };
    const uint32_t initValues[] = {34636832, 34636800, 34635776, 34603008, 33554432};

    int nbSim = 0;

    int last_house = 0;
    int turn = 0;

    int inc = 0;

    int main() {

        while(1) {
            int arr[12];
            uint8_t total_seeds = 0;
            for (int i = 0; i < 12; i++) {
                int seeds;
                cin >> seeds; cin.ignore();
                arr[i] = seeds;
            total_seeds += seeds;
            }
            
            nbSim = 0;

            // int arr[] = {4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4};
            // uint8_t total_seeds = 48;

            if (total_seeds > 40)
                maxDepth = 12;
            else if (total_seeds > 20)
                maxDepth = 13;
            else if (total_seeds > 10)
                maxDepth = 16;
            else
                maxDepth = 20;

            State s;
            s.him_score = prev_him_score + prev_seed - total_seeds - (my_new_score - prev_score);
            prev_him_score = s.him_score;
            s.me_score = my_new_score;
            prev_score = s.me_score;
            s.me = 0;
            s.him = 0;
            prev_seed = total_seeds;
            for (int i = 11; i >= 0; i--) {
                if (i < 6) {
                    s.me = (s.me << 5) | arr[i];
                } else {
                    s.him = (s.him << 5) | arr[i];
                }
            }

            cerr << "My score : " << unsigned(s.me_score) << endl;
            cerr << "Opp score : " << unsigned(s.him_score) << endl;

            int sol = 0;
            high_resolution_clock::time_point start = high_resolution_clock::now();
            //testAll();
            if (turn == 0) {
                sol = 5;
            } else {
                sol = minimax(s, 0, true, MIN, MAX);
            }
            high_resolution_clock::time_point end = high_resolution_clock::now();

            cout << sol << endl;
            cerr << "Simulations : " << nbSim << endl;
            cerr << "Time : " << duration_cast<microseconds>( (end - start) ).count()/1000.0 << " ms" << endl;
            turn++;
        }

        return 0;
    }

    inline int eval(State& s) {
        if (s.me_score >= 25)
            return 100;
        if (s.him_score >= 25)
            return -100;
        return s.me_score - s.him_score;
    }

    int cpBoardScore(uint32_t board) {
        scoreBoard = 0;
        for (inc = 0; inc < 6; inc++) {
            scoreBoard += (board >> (inc * 5)) & 0b11111;
        }
        return scoreBoard;
    }

    inline uint8_t cpTotalSeeds(State& s){
        uint8_t total = 0;
        for (inc = 0; inc < 6; inc++) {
            total += (s.me >> (inc * 5)) & 0b11111;
        }
        for (inc = 0; inc < 6; inc++) {
            total += (s.him >> (inc * 5)) & 0b11111;
        }
        return total;
    }

    int minimax(State& s, int depth, bool maxPlayer, int alpha, int beta) {

        nbSim++;

        if (depth == maxDepth) {
            return eval(s);
        }

        if (maxPlayer) {
            int best = MIN;
            int solution = 0;
            for (int i = 0; i < 6; i++) {
                if ((s.me & (0b11111 << 5*i)) && (s.him || ((s.me >> (i * 5)) & 0b11111) > 5 - i)) {
                    State ns = s;
                    if (play(ns, i)) {
                        int res = minimax(ns, depth+1, 0, alpha, beta);
                        if (res > best) {
                            best = res;
                            solution = i;
                            if (depth == 0) {
                                my_new_score = ns.me_score;
                                //cerr << "Bowl " << solution << " : " << best << endl;
                            }
                        }
                        alpha = max(alpha, best);
                        if (beta <= alpha)
                            break;
                    }
                }
            }
            if (depth == 0) {
                return solution;
            }
            if (best == MIN) {
                s.me_score += cpBoardScore(s.me);
                return eval(s);
            }
            return best;
        } else {
            int best = MAX;
            for (int i = 0; i < 6; i++) {
                if ((s.him & (0b11111 << 5*i))) {
                    State ns = s;
                    if (playHim(ns, i)) {
                        best = min(best, minimax(ns, depth+1, 1, alpha, beta));
                        beta = min(beta, best);
                        if (beta <= alpha)
                            break;
                    }
                }
            }
            if (best == MAX) {
                s.him_score += cpBoardScore(s.him);
                return eval(s);
            }
            return best;
        }
        return 0;
    }

    bool play(State& s, int house_played) {

        value =  (s.me >> (house_played * 5)) & 0b11111;

        potential_score = 0;
        if (house_played != 5) {
            s.me += initValues[house_played] & (FULL_HOUSE >> (max(5-house_played-value,0)*5));
            value -= 5 - house_played;
        }
        s.me &= ~(0b11111 << 5*house_played);
        while (value > 0) {
            malus = min(value, 6);
            s.him += values[malus];
            value -= malus;
            if (value <= 0) {
                last_house = malus-1;
                do {
                    if (!((s.him & (0b11111 << 5*last_house))  ^ (0b00010 << 5*last_house))) {
                        potential_score += 2;
                        s.him &= ~(0b11111 << 5*last_house);
                    } else if (!((s.him & (0b11111 << 5*last_house))  ^ (0b00011 << 5*last_house))) {
                        potential_score += 3;
                        s.him &= ~(0b11111 << 5*last_house);
                    } else {
                        break;
                    }
                    last_house--;
                } while (last_house >= 0);
                break;
            }
            if (value <= house_played) {
                s.me += values[value];
                break;
            } else {
                malus = min(value, 5);
                s.me += values[malus+1];
                value -= malus;
                s.me &= ~(0b11111 << 5*house_played);
            }
        }
        if (s.him) {
            s.me_score += potential_score;
        }
        return s.him;
    }

    bool playHim(State& s, int house_played) {

        value =  (s.him >> (house_played * 5)) & 0b11111;

        potential_score = 0;
        if (house_played != 5) {
            s.him += initValues[house_played] & (FULL_HOUSE >> (max(5-house_played-value,0)*5));
            value -= 5 - house_played;
        }
        s.him &= ~(0b11111 << 5*house_played);
        while (value > 0) {
            malus = min(value, 6);
            s.me += values[malus];
            value -= malus;
            if (value <= 0) {
                last_house = malus-1;
                do {
                    if (!((s.me & (0b11111 << 5*last_house))  ^ (0b00010 << 5*last_house))) {
                        potential_score += 2;
                        s.me &= ~(0b11111 << 5*last_house);
                    } else if (!((s.me & (0b11111 << 5*last_house))  ^ (0b00011 << 5*last_house))) {
                        potential_score += 3;
                        s.me &= ~(0b11111 << 5*last_house);
                    } else {
                        break;
                    }
                    last_house--;
                } while (last_house >= 0);
                break;
            }
            if (value <= house_played) {
                s.him += values[value];
                break;
            } else {
                malus = min(value, 5);
                s.him += values[malus+1];
                value -= malus;
                s.him &= ~(0b11111 << 5*house_played);
            }
        }
        if (s.me) {
            s.him_score += potential_score;
        }
        return s.me;
    }
}