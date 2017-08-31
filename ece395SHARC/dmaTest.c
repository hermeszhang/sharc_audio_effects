/*****************************************************************************
 * dmaTest.c
 *****************************************************************************/

#include <Cdef21489.h>
#include <signal.h>
#include <stdio.h>
#include "coeffs.h"
#include "coeffsIIR.h"
#include "window.h"
#include <math.h>


 
// Check SRU Routings for errors.
#define SRUDEBUG
#include <SRU.h>

#define SELECT_SPI_SLAVE(select) (*pSPIFLG &= ~(DS0EN<<8))
#define DESELECT_SPI_SLAVE(select) (*pSPIFLG |= (DS0EN<<8))

// Addresses
#define AK4396_ADDR    (0x00)

#define AK4396_CTRL1   (0x00)
#define AK4396_CTRL2   (0x01)
#define AK4396_CTRL3   (0x02)
#define AK4396_LCH_ATT (0x03)
#define AK4396_RCH_ATT (0x04)

// Reset CTRL1 setting
#define AK4396_CTRL1_RST   (0x06)

// ADC PCG register settings
// (24.576 MHz)/(desired freq) = clock divider
// clk and fs are totally indpendent of one another
// frame sync should be every 16 clocks of sample clock as per data sheet
#define CLK_DIVIDER 768	
#define FS_DIVIDER 24576		

// DAC register settings
#define AK4396_CTRL1_DEF   (0x07) 
#define AK4396_CTRL2_DEF   (0x02)
#define AK4396_CTRL3_DEF   (0x00)
#define AK4396_LCH_ATT_DEF (0xFF)
#define AK4396_RCH_ATT_DEF (0xFF)

#define BUFFER_LENGTH 256
#define POT_BUFFER_LENGTH 8

// for masking out possible address offset when only interested in pointer relative positions			
#define BUFFER_MASK 0x000000FF    

#define DELAY_LENGTH 9600

// Configure the PLL for a core-clock of 266MHz and SDCLK of 133MHz
extern void initPLL_SDRAM(void);

// local setup functions
void initSRU(void);
void initSPI(unsigned int SPI_Flag);
void configureAK4396Register(unsigned int address, unsigned int data);
void initDMA(void);
void initSPDIF(void);
void clearDAIpins(void);

// aux adc setup
void initPCGA(void);

// dsp functions
void delayWithFeedback(int delaySpeed);
void firFilter(void);
void iirFilter(void);
void downSample(int rate);
void formatInput(void);
void formatOutput(void);

void delay(int times);

// SPORT0 receive buffer a - also used for transmission
int rx0a_buf[BUFFER_LENGTH] = {0};

// buffer for storing floats	
double float_buffer[BUFFER_LENGTH] = {0.0};

// SPORT1 transmit dummy buffer - for making sure tx is behind rx
//int tx1a_buf_dummy[BUFFER_LENGTH/2] = {0};

// SPORT1 transmit buffer	
int tx1a_buf[BUFFER_LENGTH] = {0};

// SPORT2 receive buffer for pot values
int rx2a_buf[POT_BUFFER_LENGTH] = {0};
double potValue = 0.0;

/* 
TCB format:       ECx (length of destination buffer),
				  EMx (destination buffer step size),
				  EIx (destination buffer index (initialized to start address)),
				  GPx ("general purpose"),
				  CPx ("Chain Point register"; points to last address (IIx) of
			   								   next TCB to jump to
				                               upon completion of this TCB.),
				  Cx  (length of source buffer),
				  IMx (source buffer step size),
				  IIx (source buffer index (initialized to start address)) 
*/

int rx0a_tcb[8]  = {0,0,0,0, 0, BUFFER_LENGTH, 1, 0};				// SPORT0 receive a tcb from SPDIF
int tx1a_tcb[8]  = {0,0,0,0, 0, BUFFER_LENGTH, 1, 0};				// SPORT1 transmit a tcb to DAC

int tx1a_delay_tcb[8]  = {0, 0, 0, 0, 0, BUFFER_LENGTH/2, 1, 0};	// SPORT1 transmit a tcb to DAC

int rx2a_tcb[8]  = {0,0,0,0, 0, POT_BUFFER_LENGTH, 1, 0};			// SPSORT2 receive for pot values
	
int auxDivisor = 0;

// ------------------------ general globals ------------- //
// main processing index
int dsp = 0;
// when each effect plays hot potato with the eventual output sample
double potato = 0.0;

// ------------------------ delay globals --------------- //
// delay buffer index
int delay_ptr = 0;
// determines delay speed
int delay_counter = 0;
// buffer for storing delay samples
double delay_buffer[2*DELAY_LENGTH] = {0};

// ------------------------ downSample globals --------------- //

unsigned int idx = 0;

// ----------------------- IIR globals ------------------ //



void main(void) {

	initPLL_SDRAM();

	initSPI(DS0EN);
	initSRU();

	configureAK4396Register(AK4396_CTRL2, AK4396_CTRL2_DEF);
	delay(10);
	
	//Set the reset so that the device is ready to initialize registers.
	configureAK4396Register(AK4396_CTRL1, AK4396_CTRL1_RST);
	delay(10);
        	
    configureAK4396Register(AK4396_CTRL1, AK4396_CTRL1_DEF);
	delay(10);
	
	configureAK4396Register(AK4396_CTRL3, AK4396_CTRL3_DEF);
	delay(10);
	
	configureAK4396Register(AK4396_LCH_ATT, AK4396_LCH_ATT_DEF);
	delay(10);
	
	configureAK4396Register(AK4396_RCH_ATT, AK4396_RCH_ATT_DEF);
	delay(10);

	initDMA();

	initSPDIF();

	initPCGA();

	while(1){
		while( ( ((int)rx0a_buf + dsp) & BUFFER_MASK ) != ( *pIISP0A & BUFFER_MASK ) ) {
			formatInput();
			
			delayWithFeedback(potValue);
			iirFilter();
			//firFilter();

			// if this is called you cannot use formatOutput
			// downSample(2);

			//printf("%d\n", (int)(potValue/128));
			//printf("rx2a_buf[0] = %p\n", rx2a_buf[0]);
			//printf("potValue = %f\n", potValue);

			formatOutput();			
		}
	}  
}

void initSPDIF()
{
    // Use default setting of SPDIF
    *pDIRCTL=0x0;
}

void initSRU() 
{
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

void initSPI(unsigned int SPI_Flag)
{
	// Configure the SPI Control registers
    // First clear a few registers
    *pSPICTL  =(TXFLSH | RXFLSH) ; //Clear TXSPI and RXSPI
    *pSPIFLG = 0; //Clear the slave select

    //BAUDR is bits 15-1 and 0 is reserved
    *pSPIBAUD = 25;   //SPICLK baud rate = PCLK / (4*BAUDR)
    //PCLK = 200MHz and 200MHz/100 = 2 MHz? - double check this...

    // Setup the SPI Flag register using the Flag specified in the call
    *pSPIFLG = (0xF00|SPI_Flag);

    // Now setup the SPI Control register
    *pSPICTL = (SPIEN | SPIMS | WL16 | MSBF | TIMOD1 | CLKPL|GM);
    
    // SPIEN - SPI System Enable
    // SPIMS - Master Slave Mode Bit - 1 indicates we are a master device
    // WL16 - Word length is 16 bits
    // MSBF - MSB sent/received first
    // (NOT) CPHASE - SPICLK starts toggling at the middle of 1st data bit
    // CLKPL - Clock Polarity - Active low SPICLK (SPICLK high is the idle state)
    // (NOT) SMLS - Seamless Transfer disabled - delay before the next word starts, done to ensure frame on osciliscope, but check if you can change this.
    // GM - Get Data. When RXSPI is full, get more data which overwrites previous data.
}

void configureAK4396Register(unsigned int address, unsigned int data)
{
    unsigned short message = 0;

    SELECT_SPI_SLAVE(DS0EN);

    /// MSB                                LSB
    // [0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15]

    //Read/Write (stay at 1, write only, as per diagram).
    message |= 0x2000; //message[3]
    
    //Register Address
    message |= (address << 8);
        
    message |= data;
    
    *pTXSPI = message;

    //Wait for the SPI to indicate that it has finished.
    while ((*pSPISTAT & TXS))

    //Wait for the SPI to indicate that it has finished.
    while (!(*pSPISTAT & SPIFE))
    
	delay(10);
    
    DESELECT_SPI_SLAVE(DS0EN);

}

void initDMA() {

	*pSPMCTL0 = 0; // ******* ONLY SET ONCE 
	*pSPMCTL1 = 0;
	*pSPMCTL2 = 0;
	
	*pSPCTL0 = 0;
	*pSPCTL1 = 0;
	*pSPCTL2 = 0;

	/* ------------- RECEIVE DMA ------------------------------ */
	// where rx goes next (itself)
	rx0a_tcb[4] = ((int) rx0a_tcb  + 7) & 0x7FFFF | (1<<19);
	// where rx fills
	rx0a_tcb[7] = (unsigned int) rx0a_buf;

	// where SPORT0 fills first (and always)
	*pCPSP0A = ((int) rx0a_tcb  + 7) & 0x7FFFF | (1<<19);

	/* ------------- TRANSMIT DMA ------------------------------ */
	// where tx goes next (itself)
	tx1a_tcb[4] = ((int) tx1a_tcb  + 7) & 0x7FFFF | (1<<19);
	// where tx gets data
	tx1a_tcb[7] = (unsigned int) tx1a_buf;

	// where tx dummy goes next (regular tx)
	//tx1a_delay_tcb[4] = ((int) tx1a_tcb  + 7) & 0x7FFFF | (1<<19);
	// where tx dummy gets data
	//tx1a_delay_tcb[7] = (unsigned int) tx1a_buf_dummy;

	// where SPORT1 gets from first (and always)
	//*pCPSP1A = ((int) tx1a_delay_tcb  + 7) & 0x7FFFF | (1<<19);
	*pCPSP1A = ((int) tx1a_tcb  + 7) & 0x7FFFF | (1<<19);

	/* ------------- POTENTIOMETER DMA ------------------------- */
	// where pot storage buffer fills next (itself)
	rx2a_tcb[4] = ((int) rx2a_tcb + 7) & 0x7FFFF | (1<<19);
	// where pot strage buffer fills
	rx2a_tcb[7] = (unsigned int) rx2a_buf;

	// where SPORT2 fills first (and always)
	*pCPSP2A = ((int) rx2a_tcb + 7) & 0x7FFFF | (1<<19);


	// SPORT0 as receiver
	*pSPCTL0 = 	SPEN_A | 	// enable channel A
				SDEN_A |	// enable channel A DMA
				SCHEN_A | 	// enable channel A DMA chaining
				OPMODE |	// enable I2S mode 
				L_FIRST | 	// I2S sends left first
				SLEN24; 	// 24-bit word length

				
	// SPORT1 as transmitter
	*pSPCTL1 = 	SPEN_A |	// enable channel A
				SDEN_A |	// enable channel A DMA
				SCHEN_A |	// enable channel A DMA chaining
				OPMODE | 	// enable I2S mode 
				L_FIRST | 	// I2S sends left first
				SLEN24 | 	// 24-bit word length
				SPTRAN;		// set as transmitter
	
	// SPORT2 as receiver (sampled pot values) 
	*pSPCTL2 = 	SPEN_A | 	// enable channel A
				SDEN_A |	// enable channel A DMA
				SCHEN_A | 	// enable channel A DMA chaining
				OPMODE |	// enable I2S mode 
				L_FIRST | 	// I2S sends left first
				SLEN12; 	// 24-bit word length

				
}

void initPCGA(void) {

	// in the .h file is the following:
	// #define CLKASOURCE  (BIT_31)        /*External CLK A Source */
	// CLK_DIVIDER defined above

	*pPCG_CTLA0 = ENCLKA | ENFSA | FS_DIVIDER; 

	*pPCG_CTLA1 = CLK_DIVIDER;

	

}

void delay(int times)
{
	int i;

    for(i = times; i > 0; --i)
    	asm("nop;");
}

void clearDAIpins(void)
{

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

// ----------------------- FEEDBACK DELAY --------------------- //

void delayWithFeedback(int delaySpeed) 
{
	// delay_ptr is putting what rx just took in into the delay_buffer.
	// once the delay length is satisfied, dsp pointer is adding to the receive buffer what rx
	// already put there PLUS what's just ahead of where delay_ptr is now. this way, 
	// the desired delay time is satisfied constantly.


	delaySpeed = (int)(delaySpeed/127);

	if (delaySpeed > 0)
	 delay_counter =  (delay_counter + 1) % delaySpeed;

	if  (delay_counter == 0) {

		//*** write current input + attenuated delay into delay buffer at delay_ptr
		delay_buffer[delay_ptr] = 0.5*delay_buffer[delay_ptr] + float_buffer[dsp];

		//*** increment delay_ptr - now points to oldest spot in delay buffer
		delay_ptr = (delay_ptr + 1)%DELAY_LENGTH;

		//*** put the oldest delay value (from 1 delay ago) into receive float buffer
	    float_buffer[dsp] = potato = delay_buffer[ delay_ptr];
	}
	

    return;
}
// ----------------------- FIR FILTER ---------------------- //

void firFilter() {

	double acc = 0.0;
	int i = 0;

	// convolution of input (going bakcwards) and coeffs (going forward)
	for (i = 0 ; i < FILTER_LENGTH ; i++)
		acc += float_buffer[ (dsp - i + BUFFER_LENGTH) % BUFFER_LENGTH ] * coeffs[i];

	potato = acc;

	return;              
}
// ----------------------- IIR FILTER ---------------------- //

void iirFilter() {

	int i;
	
	// accumulators for IIR filter
	double acc_iir = 0.0;
	double new_hist = 0.0;

	// the two delay values in each stage
	// turns out things have to be initialized - didn't and errors ensued
	double history1 = 0.0;
	double history2 = 0.0;	

	// pointer to beginning of coeffsIIR array
	double * coeffPtr = coeffsIIR;

	// pointers to beginning two spots of history array
	double * hist1Ptr = history;
	double * hist2Ptr = history + 1;

	// initial gain before going through each 2nd order IIR stage
	acc_iir = gain*float_buffer[dsp];

	for(i = 0; i < stages; i++ ) {

		// fill delay elements with current stage's history values
		history1 = *hist1Ptr;
		history2 = *hist2Ptr;

		//printf("history1 = %f\n",history1);
		//printf("acc_iir = %f\n",acc_iir);

		// poles 
		acc_iir = acc_iir - (*coeffPtr++) * (history1);
		new_hist = acc_iir - (*coeffPtr++) * (history2);

		// zeros
		acc_iir = acc_iir * (*coeffPtr++);
		acc_iir = acc_iir + (*coeffPtr++) * (history1);
		acc_iir = acc_iir + (*coeffPtr++) * (history2);

		//printf("acc_iir = %f\n",acc_iir);

		// shift in new delay element values for this stage
		*hist2Ptr++ = *hist1Ptr;
		*hist1Ptr++ = new_hist;

		// leap frog over by two to be at right spot in next stage
		hist1Ptr++;
		hist2Ptr++;
	}
	

	
	// output accumulator contains the filtered sample for output
	potato = acc_iir;

	return;
}

void downSample(int rate) {
	
	// if (dsp % rate == 0)
	// 	potato = float_buffer[dsp];
	// else {
	// 	dsp++;
	// }


	// float_buffer[dsp/rate] = float_buffer[(dsp + BUFFER_LENGTH)/rate] = float_buffer[dsp];


	if (!(dsp % rate)){
		
		tx1a_buf[dsp/rate] = tx1a_buf[(dsp + BUFFER_LENGTH)/rate] =  (int)(float_buffer[dsp] * hanning[idx]);
		// tx1a_buf[dsp/rate] = tx1a_buf[(dsp + BUFFER_LENGTH)/rate] = (int)(1000000*hanning[idx]);
		//printf("idx = %u\n", idx);	
		if (idx == ((BUFFER_LENGTH/rate) - 1))
			idx = 0;
		else
			idx++;
	}
	

	tx1a_buf[dsp] &= 0x00FFFFFF;	

	dsp = (dsp + 1)%(BUFFER_LENGTH);

	return;

}

// ----------------------- FORMAT INPUT ---------------------- //

void formatInput(void){

	// format audio input

	if (rx0a_buf[dsp] & 0x00800000)			// is rx negative?
		rx0a_buf[dsp] |= 0xFF000000;		// sign-extend it

    float_buffer[dsp] = potato = (double)rx0a_buf[dsp];

    // format potentiometer input
    potValue = (double)rx2a_buf[0];	

    return;
}

// ----------------------- FORMAT OUTPUT --------------------- //

void formatOutput(void){

	tx1a_buf[dsp] = (int)potato;
	tx1a_buf[dsp] &= 0x00FFFFFF;	

	dsp = (dsp + 1)%BUFFER_LENGTH;

	return;
}









































