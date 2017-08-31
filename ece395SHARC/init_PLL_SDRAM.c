/********************************************************************************************
**
**  File:   initPLL.c
**  Date:   02-22-2010
**  Author: VS
**  Use:    Initialize the DSP PLL for the required CCLK and HCLK rates.
**  Note:   CLKIN will be 25 MHz from an external oscillator.  The PLL is programmed
**          to generate a core clock (CCLK) of 400 MHz and SDCLK of 160MHz
**          
**
*********************************************************************************************/
#include <def21489.h>
#include <cdef21489.h>

/*
** The following definition is a good programming practice to follow,
** in order to prevent the compiler from optimizing out any instructions
** that it may think are non-essential.
** It also makes code MISRA rule 2.1 compliant.
*/
#define NOP asm volatile("nop;")

void initPLL_SDRAM(){

/********************************************************************************************/




int i, pmctlsetting;

	// Set INDIV bit in PMCTL register
	pmctlsetting = *pPMCTL;
	pmctlsetting |= INDIV;
	*pPMCTL= pmctlsetting;

	// Program PLL multiplier to same value as CLK_CFGx pins/previously programmed value in software……
	*pPMCTL = pmctlsetting;

	// then place PLL in bypass mode
	pmctlsetting |= PLLBP;
	*pPMCTL = pmctlsetting;

    //Wait for recommended number of cycles
    for (i=0; i<4096; i++)
          NOP;

	// Bring PLL out of bypass mode by clearing PLLBP bit
    *pPMCTL ^= PLLBP;

	for (i=0; i<16; i++)
          NOP;

	pmctlsetting = *pPMCTL;
	// Clear the previous PLL multiplier
	pmctlsetting &= ~PLLM63;
	// Clear the INDIV bit
	pmctlsetting &= ~INDIV;
	// or set the INDIV bit
	// *pMCTL |= INDIV;
	*pPMCTL= pmctlsetting;
	
// CLKIN= 25 MHz, Multiplier= 16, Divisor= 2, CCLK_SDCLK_RATIO 2.5.
// Fcclk = (CLKIN * 2 * M) / (N * D)
// VCO frequency = 2*fINPUT*PLLM = 2*25*16 = 800 <= fVCOmax (800 MHz)
// M = 1 to 64, N = 2,4,8,16 and D = 1 if INDIV = 0, D = 2 if INDIV = 1

   
    pmctlsetting=SDCKR2_5|PLLM16|DIVEN|PLLD2;
    *pPMCTL= pmctlsetting;

    pmctlsetting|= PLLBP;	//Setting the Bypass bit
    pmctlsetting^= DIVEN;	//Clearing the DIVEN bit
    *pPMCTL= pmctlsetting;	// Putting the PLL into bypass mode

    //Wait for around 4096 cycles for the pll to lock.
    for (i=0; i<5000; i++)
          NOP;

   pmctlsetting = *pPMCTL;
	pmctlsetting ^= PLLBP;          //Clear Bypass Mode
	*pPMCTL = pmctlsetting;

    //Wait for around 15 cycles for the output dividers to stabilize.
     for (i=0; i<16; i++)
          NOP;


   InitSDRAM:
   
// SDRAM memory on EZ-Board - MT48LC16M16A2-6A
// 5.4ns @ CL = 3 ,167 MHz speed
// Parameters 
// Config - 64M x 16(16M x 16 x 4)
// Speed -        167 MHz
// CAS Latency -   3
// Row addressing - 8K(A0-A12)
// Column addressing - 512(A0-A8)
// No of Banks - 4

//tRAS - 42ns
//tRCD - 18ns
//tRP - 18ns
//tRC - 60ns( tRP + tRAS >= tRC)
//tWR - (1CLK + 6ns)

//For the 160 MHz case, tSDCLK = 1/160 MHz = 6.25ns 
// CAS Latency = 3
// tRCD = 18 / 6.25 = 3(2.88)
// tRP = 18 / 6.25 = 3(2.88)
// tRC = 60 / 6.25 = 10
// tRAS = 42/ 6.25 = 7(6.72)
// tWR = (6.25 + 6)/6.25 = 2(1.96)
// fSDCLK = 160 MHz
// tREF=  64ms
// NRA = 8192

// RDIV = ((f SDCLK X t REF)/NRA) - (t RAS + t RP)
// RDIV = (((160 X 10^6 x 64 x 10^-3)/8192) - (7 + 3))
// RDIV = 1240 = 0x4D8


    *pSYSCTL |= MSEN; // Enables FLAG2 and 3 (IRQ2 and TIMEXP) as MS2 and 3
    

     // Mapping Bank 0 to SDRAM
    *pEPCTL |=B0SD;
    *pEPCTL &= ~(B1SD|B2SD|B3SD);
    
    
    //Programming the SDCTL register
     *pSDCTL= SDCL3|SDPSS|SDCAW9|SDRAW13|SDTRAS7|SDTRP3|SDTWR2|SDTRCD3|X16DE;


          
    // Change this value to optimize the performance for quazi-sequential accesses (step > 1)
    
    #define SDMODIFY 1                    // Setting the Modify to 1
 
    *pSDRRC= (0x4D8)|(SDMODIFY<<17)|SDROPT;
    
    
    
    *pSYSCTL |= EPDATA32; // Data [31:0] pins are configured as data lines
    
    
    // Flash is connected on Bank 1 
	// Programming maximum waitstates
	
	*pAMICTL1 = AMIEN | BW8 | WS31 ; 
	
	
	// SRAM is connected on Bank 3
	// SRAM part used - IS61WV102416BLL
	// As per datasheet access time is 10 ns, 8ns
	// Programming waitstates = 4
	
	
	*pAMICTL3 =  AMIEN | BW16 | WS4;
   
    


     

    

}








