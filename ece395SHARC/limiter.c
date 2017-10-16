// code adapted from http://bastibe.de/2012-11-02-real-time-signal-processing-in-python.html
#include "limiter.h"

// this corresponds to the Python __init__ function.
limiter_state init_limiter(double attack_coeff, double release_coeff, int delay_len) {
    limiter_state state;
    state.attack_coeff = attack_coeff;
    state.release_coeff = release_coeff;
    state.delay_index = 0;
    state.envelope = 0;
    state.current_gain = 1;
    state.delay_length = delay_len;
    return state;
}

double limit(double signal, double threshold, double *delay_line, limiter_state *state) {

    signal = signal / MAX_AMPLITUDE;

    delay_line[state->delay_index] = signal;
    state->delay_index = (state->delay_index + 1) % state->delay_length;

    // calculate an envelope of the signal
    // might be worth tweaking? should potentially be -= instead
    state->envelope *= state->release_coeff;
    state->envelope = MAX(fabs(signal), state->envelope);

    // have current_gain go towards a desired limiter target_gain
    double target_gain;
    if (state->envelope > threshold)
        target_gain = (1 + threshold - state->envelope);
    else
        target_gain = 1.0;
    state->current_gain = state->current_gain * state->attack_coeff + target_gain * (1 - state->attack_coeff);

    // limit the delayed signal
    return delay_line[state->delay_index] * state->current_gain * MAX_AMPLITUDE;
}
