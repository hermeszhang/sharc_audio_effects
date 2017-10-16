#ifndef _LIMITER_H
#define _LIMITER_H

#include "globals.h"
#include "iirFilter.h"

// this corresponds to the Python Limiter class.
typedef struct limiter_state_t {
    int delay_index;
    int delay_length;
    double envelope;
    double current_gain;
    double attack_coeff;
    double release_coeff;
} limiter_state;

limiter_state init_limiter(double attack_coeff, double release_coeff, int delay_len);
double limit(double signal, double threshold, double *delay_line, limiter_state *state);

#endif
