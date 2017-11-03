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

	volatile clock_t clock_old;
	volatile clock_t clock_new;
	double secs;

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
				
				// 3 is the mux select line for the button
				if (selectCounter == 3)
					checkButton();

				// MAX_POT_VAL/4095.0  = 2, this factor blows up the range
				dSlope[selectCounter] = (double)((potArray[selectCounter]*(MAX_POT_VAL/4095.0) - (int)d[selectCounter])/(TOGGLE_TIME * NUM_POTS));

				selectCounter = (selectCounter + 1) % NUM_POTS;

				if (selectCounter == 0)
					clearCounter();
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
			f = (2*PI/Fs) * (MAX_LFO_SPEED / MAX_POT_VAL) * d[2];
			// f = 0.0;

			// delayLagrangeWithFeedback();
			// potato /= 9388607;
			// printf("potatoNorm = %lf\t potato = %1f\n", potato / 9388607, potato);
			// delayFromIEEE(d[1], d[0], &delayLimiter);
			delayLFO(d[1], d[0], &delayLimiter,  MAX_LFO_AMP * sin(f * n) );

			// chorus(d[1], d[0], &chorusLimiter);

			n = (n + 1) % (int)(Fs / ((MAX_LFO_SPEED / MAX_POT_VAL) * d[2]));

			// potato = iirFilter(potato);
			// firFilter();
			// potato = MAX_LFO_AMP * sin(f * n);

			formatOutput();
		}

		// printf("Time taken is %lf seconds\n",secs); 

	}
}
