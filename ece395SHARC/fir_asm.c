#include "fir_asm.h"

// these values need to be determined
#define HLTH 128
#define IR_LENGTH 128

int fir_asm()
{
    float sum;
    float history[HLTH];
    float imp[IR_LENGTH];
    int histIdx = 0;

    asm(
        "R0 = R0 - R0, MODIFY(I0, M3);                      \n"
        "LCNTR = L0, DO(PC, FIR_LOOP_END) UNTIL LCE;        \n"
        "   F1 = DM(I0, 1);                                 \n"
        "   F2 = DM(I1, 1);                                 \n"
        "   F1 = F1 + F2;                                   \n"
        "FIR_LOOP_END: F0 = F0 + F1;                        \n"      
        // output registers
        : "R0" (sum)
        // input registers
        : "B0" (&history[0]),
          "L0" (HLTH),
          "M3" (histIdx),
          "I1" (&imp[0])
        // clobbered registers
        : "R0", "R1", "F2", "I0", "B0", "M0", "I1", "M3"
    );
}
