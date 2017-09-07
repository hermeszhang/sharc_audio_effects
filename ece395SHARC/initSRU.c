#include "initSRU.h"

void clearDAIpins(void);

void initSRU() {

	clearDAIpins();
	
	// use pin 12 on the board for SPDIF in
	// this is the pin second from the power,
	// in the not-ground row
	// DAI pin buffer 12 output to SPDIF receiver as input
	SRU(LOW, DAI_PB12_I);
	SRU(LOW, PBEN12_I);
	SRU(DAI_PB12_O, DIR_I);

	//Power off the DAC
	SRU2(HIGH, DPI_PBEN04_I);
	SRU2(LOW, DPI_PB04_I);
	
	delay(10);
	 
	/* ---------- DAC ----------------- */
    //Attach Main Clocks from SPDIF receiver

	//MCLK
	SRU(DIR_TDMCLK_O, DAI_PB05_I); 
	SRU(HIGH,PBEN05_I);
	
	//BICK
	SRU(DIR_CLK_O, DAI_PB06_I);
	SRU(HIGH,PBEN06_I);
	
	//LRCK
	SRU(DIR_FS_O, DAI_PB03_I);
	SRU(HIGH,PBEN03_I);
	
	//CSN
	SRU2(SPI_FLG0_O, DPI_PB07_I);
    SRU2(SPI_FLG0_PBEN_O, DPI_PBEN07_I);

	//Set MOSI/CDT1 to output
	SRU2(SPI_MOSI_O, DPI_PB01_I);
	SRU2(HIGH, DPI_PBEN01_I);
	
	//Send SPI clock to DPI 3
	SRU2(SPI_CLK_O, DPI_PB03_I);
	SRU2(SPI_CLK_PBEN_O, DPI_PBEN03_I);
	
	//Power back on the DAC
	SRU2(HIGH, DPI_PB04_I);

	delay(10);  

	/* ---------- SPORT -------------- */
	SRU(DAI_PB06_O, SPORT0_CLK_I);
	SRU(DAI_PB06_O, SPORT1_CLK_I);
	SRU(DAI_PB03_O, SPORT0_FS_I);
	SRU(DAI_PB03_O, SPORT1_FS_I);

	// SPORT0 receives from SPDIF 
	SRU(DIR_DAT_O, SPORT0_DA_I);

	// SPORT1 outputs to the DAC
	SRU(SPORT1_DA_O, DAI_PB04_I);
	SRU(HIGH, PBEN04_I);

	/* --------- ADC -------------------------------------------- */

	/* -- outputs from SHARC to inputs on ADC board -- */

	// CLKA_O to DAI 15 (sample clock for potentiometer ADC)
	SRU(PCG_CLKA_O, DAI_PB15_I);
	SRU(HIGH, PBEN15_I);

	// chip select (active low) output on DAI 08
	SRU(PCG_FSA_O, DAI_PB08_I);
	SRU(HIGH, DAI_PBEN08_I);

	/* -- inputs to SHARC SPORT 2A from ADC board -- */

	// connect SPORT2A clock and frame sync with PCG clock
	SRU(PCG_CLKA_O, SPORT2_CLK_I); 
	SRU(PCG_FSA_O, SPORT2_FS_I);	

	// sampled potentiometer value physically wired to DAI 14
	// connect DAI 14 to SPORT2A's data input
	SRU(LOW, DAI_PB14_I);
	SRU(LOW, DAI_PBEN14_I);
	SRU(DAI_PB14_O, SPORT2_DA_I);
	
	/* ---------- DEBUG -------------- */
	
	SRU(PCG_CLKA_O, DAI_PB09_I);
	SRU(HIGH, PBEN09_I);

	SRU(PCG_FSA_O, DAI_PB11_I);
	SRU(HIGH, PBEN11_I);
}

void clearDAIpins(void) {

//  Tie the pin buffer inputs LOW for all DAI pins.  Even though
//  these pins are inputs to the SHARC, tying unused pin buffer inputs
//  LOW is "good coding style" to eliminate the possibility of
//  termination artifacts internal to the IC.  Note that signal
//  integrity is degraded only with a few specific SRU combinations.
//  In practice, this occurs VERY rarely, and these connections are
//  typically unnecessary.  This is GROUP D

    SRU(LOW, DAI_PB01_I);
    SRU(LOW, DAI_PB02_I);
    SRU(LOW, DAI_PB03_I);
    SRU(LOW, DAI_PB04_I);
    SRU(LOW, DAI_PB05_I);
    SRU(LOW, DAI_PB06_I);
    SRU(LOW, DAI_PB07_I);
    SRU(LOW, DAI_PB08_I);
    SRU(LOW, DAI_PB09_I);
    SRU(LOW, DAI_PB10_I);
    SRU(LOW, DAI_PB11_I);
    SRU(LOW, DAI_PB12_I);
    SRU(LOW, DAI_PB13_I);
    SRU(LOW, DAI_PB14_I);
    SRU(LOW, DAI_PB15_I);
    SRU(LOW, DAI_PB16_I);
    SRU(LOW, DAI_PB17_I);
    SRU(LOW, DAI_PB18_I);
    SRU(LOW, DAI_PB19_I);
    SRU(LOW, DAI_PB20_I);

//  Tie the pin buffer enable inputs LOW for all DAI pins so
//  that they are always input pins.  This is GROUP F.

    SRU(LOW, PBEN01_I);
    SRU(LOW, PBEN02_I);
    SRU(LOW, PBEN03_I);
    SRU(LOW, PBEN04_I);
    SRU(LOW, PBEN05_I);
    SRU(LOW, PBEN06_I);
    SRU(LOW, PBEN07_I);
    SRU(LOW, PBEN08_I);
    SRU(LOW, PBEN09_I);
    SRU(LOW, PBEN10_I);
    SRU(LOW, PBEN11_I);
    SRU(LOW, PBEN12_I);
    SRU(LOW, PBEN13_I);
    SRU(LOW, PBEN14_I);
    SRU(LOW, PBEN15_I);
    SRU(LOW, PBEN16_I);
    SRU(LOW, PBEN17_I);
    SRU(LOW, PBEN18_I);
    SRU(LOW, PBEN19_I);
    SRU(LOW, PBEN20_I);
}
