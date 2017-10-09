#include <Cdef21489.h>
#include <signal.h>
#include <stdio.h>
#include <math.h>

#include "globals.h"
#include "delay.h"
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

	initCounter();

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

	int selectCounter = 0;
	int potArray_0, potArray_1;
	int toggle = 0;
	// int potValueArray[120][2];
	// int potValPtr = 0;

	while(1){

		while( ( ((int)rx0a_buf + dsp) & BUFFER_MASK ) != ( *pIISP0A & BUFFER_MASK ) ) 
		{
			formatInput();

			if (toggle == 1500) {
				potArray[selectCounter] = readPotValues();
				pingCounter();
				selectCounter = (selectCounter + 1) % NUM_POTS;

				// potArray_0 = potArray[0];
				// potArray_1 = potArray[1];

				// potValueArray[potValPtr][0] = potArray[0];
				// potValueArray[potValPtr][1] = potArray[1];

				// potValPtr = (potValPtr + 1) % 120;
				toggle = 0;
			}

			toggle++;


			// printf("select: %d\tch.0: %d\tch.1: %d\n", selectCounter, potArray[0], potArray[1]);

			delayLagrangeWithFeedback();
			// delayFromIEEE();

			// potTesting();
			//iirFilter();
			//firFilter();

			formatOutput();			
		}
		
	}  
}
