#ifndef _LIMITER_H
#define _LIMITER_H

#include "globals.h"
#include "iirFilter.h"

// this corresponds to the Python Limiter class.
typedef struct limiter_state_t {
    int delay_index;
    int delay_length;
    float envelope;
    float current_gain;
    float attack_coeff;
    float release_coeff;
} limiter_state;

limiter_state init_limiter(float attack_coeff, float release_coeff, int delay_len);
void limit(float *signal, int block_length, float threshold,
           float *delay_line, limiter_state *state);

#endif
