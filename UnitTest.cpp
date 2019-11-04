#include <iostream>
#include <bitset>
#include "Player1.h"
#include <cassert>

using namespace std;

void utestCapture();
void utestNotPossible();
void utestPossible();
void utestBigTest();
void utestBigTestHim();
void utestCannotPlayHouse2();
void testCpBoardScore ();
void testParseInput();

void testAll() {
    utestCapture();
    utestNotPossible();
    utestPossible();
    utestBigTest();
    utestBigTestHim();
    utestCannotPlayHouse2();
    testCpBoardScore();
    testParseInput();
    cout << "All tests passed" << endl;
}

void testCpBoardScore () {
    assert(cpBoardScore(0b00000000011001010110001001101000) == 67);
}

void testParseInput() {

    State s;
    s.me_score = 0;
    s.him_score = 0;
    s.me = 0;
    s.him = 0;
    int arr[] = {1, 6, 6, 5, 5, 4, 4, 4, 4, 4, 0, 5};
    for (int i = 11; i >= 0; i--) {
        if (i < 6) {
            s.me = (s.me << 5) | arr[i];
        } else {
            s.him = (s.him << 5) | arr[i];
        }
    }
    play(s, 0);
    playHim(s, 5);
    assert(s.me  == 0b00001000011000110001110100000001);
    assert(s.him == 0b00000000000000100001000010000100);
}

void utestCannotPlayHouse2() {
    State s;
    s.me  = 0b00000001100101011000000001101000;
    s.him = 0b00000000000100110010000100110001;
    s.me_score = 0;
    s.him_score = 0;

    int house_played = 2;

    assert(!(s.me & (0b1111 << 4*house_played)));
}

void utestCapture() {

    State s;
    s.me  = 0b00001100010100110001100011000110;
    s.him = 0b00000010001100010000010001100001;
    s.me_score = 0;
    s.him_score = 0;

    int house_played = 4;

    play(s, house_played);

    assert(235083974 == s.me);
    assert(36700290 == s.him);
    assert(5 == s.me_score);
    assert(0 == s.him_score);
}

void utestNotPossible() {
    State s;
    s.me  = 0b00000000000000000000000000000001;
    s.him = 0b00000000000000000000000000000000;
    s.me_score = 0;
    s.him_score = 0;

    int house_played = 0;

    assert(!play(s, house_played));
}

void utestPossible() {
    State s;
    s.me  = 0b00000010000000000000000000000000;
    s.him = 0b00000000000000000000000000000000;
    s.me_score = 0;
    s.him_score = 0;

    int house_played = 5;

    assert(play(s, house_played));
}

void utestBigTest() {

    State s;
    s.me  = 0b00001101110000110011000110000110;
    s.him = 0b00000000110000100001000000001011;
    s.me_score = 0;
    s.him_score = 0;

    int house_played = 4;

    play(s, house_played);
    assert(302266824 == s.me);
    assert(83074158 == s.him);
    assert(0 == s.me_score);
    assert(0 == s.him_score);
}

void utestBigTestHim() {

    State s;
    s.him = 0b00001101110000110011000110000110;
    s.me  = 0b00000000110000100001000000001011;
    s.me_score = 0;
    s.him_score = 0;

    int house_played = 4;

    playHim(s, house_played);

    assert(302266824 == s.him);
    assert(83074158 == s.me);
    assert(0 == s.him_score);
    assert(0 == s.me_score);
}