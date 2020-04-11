/*
 * A2.c
 * Created on: Apr 9, 2020
 * Authors: Darian Primer & Alexander Goldstein
 * 
 * Description: Assignment #2 for EE329 course. 
 * Problem Statement: 
 *      1. Create a function to change the MSP432 MCLK using the DCO
 *      2. Create Delay Function
 *      3. Validate Delay Accuracy
 *      4. Generate and Observe Short Pulses
 *      5. Building a Header File and Associated C file
 */
#include "msp.h"

// Function prototypes
void set_DCO(int freq);


// DCORSEL Range Select values
#define FREQ_1_5_MHz    CS_CTL0_DCORSEL_0
#define FREQ_3_MHz      CS_CTL0_DCORSEL_1
#define FREQ_6_MHz      CS_CTL0_DCORSEL_2
#define FREQ_12_MHz     CS_CTL0_DCORSEL_3
#define FREQ_24_MHz     CS_CTL0_DCORSEL_4
#define FREQ_48_MHz     CS_CTL0_DCORSEL_5



void main() {
    // Set DCO to nominal frequency (programmer's choice, using DCORSEL in #define)
    set_DCO(FREQ_48_MHz);

    // Verify by routing MCLK to P4.3
    P4 -> SEL1 &=  ~BIT3;           //Set SEL1 to 0
    P4 -> SEL0 |=  BIT3;            //Set SEL0 to 1
    P4 -> DIR  |=  BIT3;            //Set P4.3 as output
    
}

/* Sets Digitally Controlled Oscillator (DCO) to frequency specified by freq.
 *      freq = FREQ_1.5_MHz, FREQ_12_MHz, etc. according to DCORSEL
*/
void set_DCO(int freq) {
    // Check that freq is valid (one of the nominal defined ranges)
    if(freq==FREQ_1_5_MHz || freq==FREQ_3_MHz || freq==FREQ_6_MHz || freq==FREQ_12_MHz || freq==FREQ_24_MHz) 
    {
            // changing DCO requires unlocking Clock System registers
            CS->KEY = CS_KEY_VAL;           // unlock CS registers
            CS->CTL0 = 0;                   // clear register CTL0

            // Set DCO to freq using CORSEL values (see TRM Table 6-4)
            printf("%d",freq);              //Check to see that frequency selecting variable is being passed correctly
            CS->CTL0 = freq;

            // select which clocks to route DCO to
            CS->CTL1 =  CS_CTL1_SELM_3;
            CS->KEY = 0;                // lock the CS registers
    } 
    else if(freq==FREQ_48_MHz)         // If it is 48MHz, warn that further action must be taken.
    {
        printf("WARN: 48MHz set as frequency. Changing VCORE LVL1, Flash wait-state 1 as well.");
        
        // From Lab Manual TN3:
        /* Transition to VCORE Level 1: AM0_LDO --> AM1_LDO */
        while ((PCM->CTL1 & PCM_CTL1_PMR_BUSY));
         PCM->CTL0 = PCM_CTL0_KEY_VAL | PCM_CTL0_AMR_1;
        while ((PCM->CTL1 & PCM_CTL1_PMR_BUSY));
        
        /* Configure Flash wait-state to 1 for both banks 0 & 1 */
        FLCTL->BANK0_RDCTL = (FLCTL->BANK0_RDCTL &
         ~(FLCTL_BANK0_RDCTL_WAIT_MASK)) | FLCTL_BANK0_RDCTL_WAIT_1;
        FLCTL->BANK1_RDCTL = (FLCTL->BANK0_RDCTL &
         ~(FLCTL_BANK1_RDCTL_WAIT_MASK)) | FLCTL_BANK1_RDCTL_WAIT_1;
        
        /* Configure DCO to 48MHz, ensure MCLK uses DCO as source*/
        CS->KEY = CS_KEY_VAL ; // Unlock CS module for register access
        CS->CTL0 = 0; // Reset tuning parameters
        CS->CTL0 = CS_CTL0_DCORSEL_5; // Set DCO to 48MHz
        /* Select MCLK = DCO, no divider */
        CS->CTL1 = CS->CTL1 & ~(CS_CTL1_SELM_MASK | CS_CTL1_DIVM_MASK) |
         CS_CTL1_SELM_3;
        CS->KEY = 0; // Lock CS module from unintended accesses
    }

}
