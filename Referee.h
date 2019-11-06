 #include <stdint.h>

#ifndef REFEREE_H
#define REFEREE_H
struct State {
    uint32_t me = 0;
    uint32_t him = 0;
    uint8_t me_score = 0;
    uint8_t him_score = 0;
};
#endif