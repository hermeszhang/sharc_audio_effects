#include "initDMA.h"

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
}
