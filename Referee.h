 #include <stdint.h>

#ifndef REFEREE_H
#define REFEREE_H
struct State {
    uint32_t me;
    uint32_t him;
    uint8_t me_score;
    uint8_t him_score;
};
#endif