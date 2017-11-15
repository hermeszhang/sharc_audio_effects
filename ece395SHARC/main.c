#include <Cdef21489.h>
#include <signal.h>

#include "globals.h"
#include "delay.h"
#include "chorus.h"
#include "initSRU.h"
#include "initSPDIF.h"
#include "initSPI.h"
#include "configAK4396.h"
#include "initDMA.h"
#include "initPCGA.h"
#include "firFilter.h"
#include "iirFilter.h"
#include "format.h"
#include "muxSelect.h"
#include "readPotValues.h"
#include "pingCounter.h"

// Addresses
#define AK4396_ADDR    (0x00)
#define AK4396_CTRL1   (0x00)
#define AK4396_CTRL2   (0x01)
#define AK4396_CTRL3   (0x02)
#define AK4396_LCH_ATT (0x03)
#define AK4396_RCH_ATT (0x04)
// Reset CTRL1 setting
#define AK4396_CTRL1_RST   (0x06)	
// DAC register settings
#define AK4396_CTRL1_DEF   (0x07) 
#define AK4396_CTRL2_DEF   (0x02)
#define AK4396_CTRL3_DEF   (0x00)
#define AK4396_LCH_ATT_DEF (0xFF)
#define AK4396_RCH_ATT_DEF (0xFF)
// for masking out possible address offset when only interested in pointer relative positions			
#define BUFFER_MASK 0x000000FF    

// Configure the PLL for a core-clock of 266MHz and SDCLK of 133MHz
extern void initPLL_SDRAM(void);

void main(void) {


		initPLL_SDRAM();
	initSPI(DS0EN);
	initSRU();

	configAK4396(AK4396_CTRL2, AK4396_CTRL2_DEF);
	delay(10);
	//Set the reset so that the device is ready to initialize registers.
	configAK4396(AK4396_CTRL1, AK4396_CTRL1_RST);
	delay(10); 	
    configAK4396(AK4396_CTRL1, AK4396_CTRL1_DEF);
	delay(10);
	configAK4396(AK4396_CTRL3, AK4396_CTRL3_DEF);
	delay(10);
	configAK4396(AK4396_LCH_ATT, AK4396_LCH_ATT_DEF);
	delay(10);
	configAK4396(AK4396_RCH_ATT, AK4396_RCH_ATT_DEF);
	delay(10);

	initDMA();
	initSPDIF();
	initPCGA();

	initADCSPI(DS1EN);

	clearCounter();

	int i = 0;
	int selectCounter = 0;
	int toggle = 0;
	double d[NUM_POTS] = {0.0};
	double dSlope[NUM_POTS] = {0.0};
	double f;
	double amp = 500;
	int n = 0;
	double debug;
	double movie[5000] = {0.0};
	int movie_idx = 0;
	int skipCounter = 0;

	double potArrayPrev[NUM_POTS] = {0.0};
	double threshold = 10;
	// target value for slewing (dslope)
	double target;

	// potentiometer histories for filtering
	double pot_history[NUM_POTS*FILTER_LENGTH] = {0.0};
	int history_idx = 0;

	volatile clock_t clock_old;
	volatile clock_t clock_new;
	double secs;

	// 1 is for knob, 0 for button
	int controlState = 0;
	// boolean type to check if knob has changed
	int knobChange = 0;
	// boolean type to check if new button session has begun
	int newButtonSession = 0;

	// DEBUG
	// int potValueArray[5][NUM_POTS];
	// int potValPtr = 0;
	// DEBUG

	limiter_state delayLimiter = init_limiter(0.9, 0.5, DELAY_LINE_LENGTH);		// attack, release, delay length
	limiter_state chorusLimiter = init_limiter(0.9, 0.5, DELAY_LINE_LENGTH);	// attack, release, delay length

	initChorus();
	initDelayButton();
	initDelayStruct();

	while(1){


 		while( ( ((int)rx0a_buf + dsp) & BUFFER_MASK ) != ( *pIISP0A & BUFFER_MASK ) ) 
		{
			formatInput();

			if (toggle == TOGGLE_TIME) {

				potArray[selectCounter] = readPotValues();

				// don't apply the filter to the button!!
				if (selectCounter != 3)
					potArray[selectCounter] = fir_filter(potArray[selectCounter], pot_history + selectCounter*FILTER_LENGTH, history_idx);

				// if (selectCounter == 1){
				// 	movie[movie_idx] = potArray[selectCounter];
				// 	movie_idx = (movie_idx + 1) % 3000;
				// }

				
				// 1 is select line for delay knob
				if (selectCounter == 1) {
					knobChange = (potArray[1] > potArrayPrev[1] + threshold) || (potArray[1] < potArrayPrev[1] - threshold);
					
					if (knobChange){
						controlState = KNOB;
						dSlope[1] = (double)((potArray[1] * (MAX_POT_VAL/4095.0) - (int)d[1])/(TOGGLE_TIME * NUM_POTS));
						initDelayButton();
					}
				}
				// 3 is select line for button
				else if (selectCounter == 3) {
					newButtonSession = checkButton();

					if (newButtonSession){
						controlState = BUTTON;
						dSlope[1] = (double)((getButtonDelayReach() - (int)d[1])/(TOGGLE_TIME * NUM_POTS));
					}
				}
				// make sure slope is properly computed for all other pots
				else {
					dSlope[selectCounter] = (double)((potArray[selectCounter] * (MAX_POT_VAL/4095.0) - (int)d[selectCounter])/(TOGGLE_TIME * NUM_POTS));
				}


				potArrayPrev[selectCounter] = potArray[selectCounter];

				selectCounter = (selectCounter + 1) % NUM_POTS;

				if (selectCounter == 0){
					clearCounter();
					history_idx = (history_idx + 1) % FILTER_LENGTH;
				}
				else
					pingCounter();

				// DEBUG

				// for (i = 0 ; i < NUM_POTS ; i++)
				// 	potValueArray[potValPtr][i] = potArray[i];

				// potValPtr = (potValPtr + 1) % 5;

			    // END DEBUG 

				toggle = 0;
			}

			toggle++;

			for (i = 0 ; i < NUM_POTS ; i++)
				d[i] = d[i] + dSlope[i];

			// remap LFO frequency range to range from 0.25 Hz -> 5 Hz
			// f = (2*PI/Fs) * (MAX_LFO_SPEED / MAX_POT_VAL) * d[2];
			// f = 0.0;

			// delayLagrangeWithFeedback();
			// potato /= 9388607;
			// printf("potatoNorm = %lf\t potato = %1f\n", potato / 9388607, potato);
			// delayFromIEEE(d[1], d[0], &delayLimiter);
			debug = delayLFO(d[1], d[0], &delayLimiter, d[2]);

			if (skipCounter++ == 480) {
				movie[movie_idx] = debug;
				movie_idx = (movie_idx + 1) % 5000;
				skipCounter = 0;
			}
	
			// chorus(d[1], d[0], &chorusLimiter);

			// n = (n + 1) % (int)(Fs / ((MAX_LFO_SPEED / MAX_POT_VAL) * d[2]));

			// potato = iirFilter(potato);
			// firFilter();
			// potato = MAX_LFO_AMP * sin(f * n);

			formatOutput();
			
		}
	}
}
