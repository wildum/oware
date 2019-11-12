/*
    MCTS + MINMAX
*/


#include <iostream>
#include <bitset>
#include <algorithm>
#include <chrono>
#include <vector>
#include "UnitTest.h"
#include "Player3.h"
#include "math.h"

#pragma GCC optimize("-O3")
#pragma GCC optimize("inline")
#pragma GCC optimize("omit-frame-pointer")
#pragma GCC optimize("unroll-loops")
#undef __NO_INLINE__
#define __OPTIMIZE__ 1


using namespace std;
using namespace std::chrono;

namespace P3 {

    long long g_seed = 2345678765;

    inline int fastrand() { 
        g_seed = (214013*g_seed+2531011); 
        return (g_seed>>16)&0x7FFF; 
    }

    struct Node {
        vector<Node*> childs;
        State state;
        Node* parent;
        double winscore = 0;
        int visitCount = 0;
        int turn = 0;
        int house = 0;
        bool myTurn = true;
        bool leaf = false;
    };

    struct Tree {
        Node* root;
    };
    
    bool play(State& s, int house_played);
    bool playHim(State& s, int house_played);
    inline double uctValue(int totalVisit, double nodeWinScore, int nodeVisit);
    inline bool simulateRandomPlayout(Node* n);
    inline bool is_finished(State& s, int turn, bool iPlayNext);
    inline void backPropogation(Node* nodeToExplore, bool meWon);
    void expand(Node* n);
    inline Node* selectPromisingNode();
    inline Node* findBestNodeWithBestWinScore(Node* node);
    inline Node* findBestNodeWithUTC(Node* node);
    inline int eval(State& s);
    int MCTS();
    void reuseTree(State& s);

    const uint32_t FULL_HOUSE = 34636833;

    const int NB_CHECK_TIME = 100;

    uint8_t my_new_score = 0;
    uint8_t prev_seed = 48;
    uint8_t prev_score = 0;
    uint8_t prev_him_score = 0;
    uint8_t potential_score = 0;

    const int MAX_HOUSE = 6;

    const int MAX = 1000;
    const int MIN = -1000;
    int maxDepth = 12;

    int malus = 0;
    int value = 0;
    int scoreBoard = 0;

    const int MCTSMINMAXRATIO = 2;
    int MCTSMinMaxRatioCurr = 0;


    const uint32_t values[] = {0, 1, 33, 1057, 33825, 1082401, 34636833 };
    const uint32_t initValues[] = {34636832, 34636800, 34635776, 34603008, 33554432};

    int last_house = 0;
    int turnPlay = 0;
    int lastBestNodeIndex = 0;

    int inc = 0;

    Tree tree;

    vector<Node*> nodes_pool;
    int nodes_pool_index = 0;

    int main() {
        for (int i = 0; i < 4000000; ++i){nodes_pool.push_back(new Node());}
        while(1) {
            int arr[12];
            uint8_t total_seeds = 0;
            for (int i = 0; i < 12; i++) {
                int seeds;
                cin >> seeds; cin.ignore();
                arr[i] = seeds;
                total_seeds += seeds;
            }

            // int arr[] = {4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4};
            // uint8_t total_seeds = 48;

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


            tree.root = new Node();
            tree.root -> parent = NULL;
            tree.root -> state = s;
            tree.root -> turn = turnPlay;
            tree.root -> myTurn = true;

            int sol = 0;
            //testAll();
            if (turnPlay == 0) {
                sol = 5;
            } else {
                sol = MCTS();
            }
            cout << sol << endl;
            turnPlay++;
        }

        return 0;
    }

    void destroy() {
        for (auto p : nodes_pool)
            delete p;
        nodes_pool.clear();
        //delete tree.root;
    }

    void initMCTS() {
        turnPlay = 0;
        nodes_pool_index = 0;
        for (int i = 0; i < 4000000; ++i){nodes_pool.push_back(new Node());}
    }

    int playLocal(State& s, int turn) {


        turnPlay = turn;

        int sol = 0;
        // if (turn == 0) {
        //     sol = 5;
        // } else {
        if (MCTSMinMaxRatioCurr == MCTSMINMAXRATIO) {
            sol = minimax(s, 0, true, MIN, MAX);
            MCTSMinMaxRatioCurr = 0;
        } else {
            tree.root = new Node();
            tree.root -> parent = NULL;
            tree.root -> state = s;
            tree.root -> turn = turn;
            tree.root -> myTurn = true;
            sol = MCTS();
            MCTSMinMaxRatioCurr++;
        }
        // }
        //cout << "node pool index " << nodes_pool_index << endl;
        return sol;
    }

    int minimax(State& s, int depth, bool maxPlayer, int alpha, int beta) {

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


    int MCTS() {
        int sol = 0;
        int nbSim = 0;
        high_resolution_clock::time_point start = high_resolution_clock::now();
        int timeChecked = 0;
        while(timeChecked || duration_cast<microseconds>( high_resolution_clock::now() - start).count()/1000.0 < 45) {
            Node* promisingNode = selectPromisingNode();
            if (!promisingNode -> leaf) {
                expand(promisingNode);
            }
            Node* nodeToExplore = promisingNode;
            if (promisingNode -> childs.size() > 0) {
                nodeToExplore = promisingNode -> childs[fastrand() % promisingNode -> childs.size()];
            }
            backPropogation(nodeToExplore, simulateRandomPlayout(nodeToExplore));
            nbSim++;
            timeChecked = timeChecked == NB_CHECK_TIME ? 0 : timeChecked + 1;
        }
        if (turnPlay <=1)
            cerr << "Simulations : " << nbSim << endl;
        //cerr << "Time : " << duration_cast<microseconds>( (high_resolution_clock::now() - start) ).count()/1000.0 << " ms" << endl;
        Node* winnerNode = findBestNodeWithBestWinScore(tree.root);
        if (winnerNode == NULL) {
            for (int i = 0; i < 6; i++) {
                if (tree.root -> state.me & (0b11111 << 5*i)) {
                    return i;
                }
            }
        }
        return winnerNode -> house;
    }

    void reuseTree(State& s) {
        Node* n = tree.root -> childs[lastBestNodeIndex];
        for (int i = 0; i < n -> childs.size(); i++) {
            if (s.me == n -> childs[i] -> state.me && s.him == n -> childs[i] -> state.him) {
                tree.root =  n -> childs[i];
                tree.root -> parent = NULL;
                tree.root -> state = s;
                tree.root -> turn = turnPlay;
                tree.root -> myTurn = true;
                return;
            }
        }
        cerr << "error finding child" << endl;
    }

    inline int eval(State& s) {
        if (s.me_score >= 25)
            return 100;
        if (s.him_score >= 25)
            return -100;
        return s.me_score - s.him_score;
    }

    inline int cpBoardScore(uint32_t board) {
        scoreBoard = 0;
        for (inc = 0; inc < 6; inc++) {
            scoreBoard += (board >> (inc * 5)) & 0b11111;
        }
        return scoreBoard;
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

    inline double uctValue(int totalVisit, double nodeWinScore, int nodeVisit) {
        if (nodeVisit == 0) {
            return numeric_limits<int>::max();
        }
        return (nodeWinScore / nodeVisit) + 1.41 * sqrt(log(totalVisit) / nodeVisit);
    }

    inline Node* findBestNodeWithUTC(Node* node) {
        int parentVisit = node -> visitCount;
        Node* best = NULL;
        double score_b = numeric_limits<int>::min();
        // TODO optimise the for loop
        for (int i = 0; i < node -> childs.size(); i++) {
            Node* n = node -> childs[i];
            double score_n = uctValue(parentVisit, n -> winscore,  n -> visitCount);
            if (score_n > score_b) {
                score_b = score_n;
                best = n;
            }
        }
        if (best == NULL) {
            cerr << "ERROR NODE NULL 1" << endl;
        }
        return best;
    }

    // optimise this part too ?
    inline Node* findBestNodeWithBestWinScore(Node* node) {
        Node* best = NULL;
        double score_b = numeric_limits<int>::min();
        for (int i = 0; i < node -> childs.size(); i++) {
            Node* n = node -> childs[i];
            double score_n = n -> winscore;
            if (score_n > score_b) {
                score_b = score_n;
                best = n;
                lastBestNodeIndex = i;
            }
        }
        return best;
    }

    inline Node* selectPromisingNode() {
        Node* node = tree.root;
        while (node -> childs.size() != 0) {
            node = findBestNodeWithUTC(node);
        }
        return node;
    }

    //could expand only one if needed
    void expand(Node* n) {
        bool playAvailable = false;
        if (n -> myTurn) {
            for (int i = 0; i < 6; i++) {
                if ((n -> state.me & (0b11111 << 5*i))) {
                    State ns = n -> state;
                    if (play(ns, i)) {
                        Node* nn = nodes_pool[nodes_pool_index++];
                        nn -> state = ns;
                        nn -> parent = n;
                        nn -> myTurn = false;
                        nn -> turn = n -> turn + 1;
                        nn -> house = i;
                        n -> childs.push_back(nn);
                        playAvailable = true;
                    }
                }
            }
            if (!playAvailable) {
                n -> state.me_score += cpBoardScore(n -> state.me);
                n -> leaf = true;
            }
        } else {
            for (int i = 0; i < 6; i++) {
                if ((n -> state.him & (0b11111 << 5*i))) {
                    State ns = n -> state;
                    if (playHim(ns, i)) {
                        Node* nn = nodes_pool[nodes_pool_index++];
                        nn -> state = ns;
                        nn -> parent = n;
                        nn -> myTurn = true;
                        nn -> turn = n -> turn + 1;
                        nn -> house = i;
                        n -> childs.push_back(nn);
                        playAvailable = true;
                    }
                }
            }
            if (!playAvailable) {
                n -> state.him_score += cpBoardScore(n -> state.him);
                n -> leaf = true;
            }
        }
    }

    inline void backPropogation(Node* nodeToExplore, bool meWon) {
        Node* tempNode = nodeToExplore;
        while (tempNode != NULL) {
            tempNode -> visitCount++;
            if (meWon ^ tempNode  -> myTurn)
                tempNode  -> winscore += 1.0;
            tempNode = tempNode -> parent;
        }
    }

    inline bool is_finished(State& s, int turn, bool iPlayNext, const int MAXTURN) {
        return s.me_score >= 25
            || s.him_score >= 25
            || turn >= MAXTURN;
    }


    // stop when you know you cannot win
    bool simulateRandomPlayout(Node* n) {
        // check if the current State is finished ?
        State tmpState = n -> state;
        int turn = n -> turn;
        const int MAXTURN = turn + 70;
        bool myTurn = n -> myTurn;
        int house = 0;
        int house_tested = 0;
        while (!is_finished(tmpState, turn, myTurn, MAXTURN)) {
            house = fastrand() % 6;
            house_tested = 0;
            if (myTurn) {
                while ((tmpState.me & (0b11111 << 5*house)) && house_tested < MAX_HOUSE && !play(tmpState, house)) {
                    house = house == 5 ? 0 : house + 1;
                    house_tested++;
                }
                if (house_tested == MAX_HOUSE) {
                    tmpState.me_score += cpBoardScore(n -> state.me);
                    break;
                }
            } else {
                while ((tmpState.him & (0b11111 << 5*house)) && house_tested < MAX_HOUSE && !playHim(tmpState, house)) {
                    house = house == 5 ? 0 : house + 1;
                    house_tested++;
                }
                if (house_tested == MAX_HOUSE) {
                    tmpState.him_score += cpBoardScore(n -> state.him);
                    break;
                }
            }
            myTurn = !myTurn;
            turn++;
        }
        // handle draw ?
        return tmpState.me_score > tmpState.him_score;
    }
}