/*
*  Delay_us.c
* Created on: Apr 10, 2020
*Authors: Darian Primer & Alexander Goldstein
*
*  Description: Assignment #2 Part 2 for EE329 course.
*  Problem Statement:
        3. Create a function delay_us() to cause a software delay of a specified time. (See LM_A2 for details.)
*/

// Include necessary headers
#include "msp.h"
#include "Set_DCO.h"
#include <math.h>

// Function prototypes
void delay_us(int us);

// main() for testing / debug & verification
int main(void) {

    // Debug variables
    set_DCO(FREQ_3_MHz);
    int delay_time = 100;

    // Verify by routing MCLK to P4.3
    P4 -> SEL1 &=  ~BIT3;           //Set SEL1 to 0    <-- (01) == MCLK on P4.3
    P4 -> SEL0 |=  BIT3;            //Set SEL0 to 1
    P4 -> DIR  |=  BIT3;            //Set P4.3 as output

    // Route P2.3 as GPIO
    P2 -> SEL1 &= ~BIT3;        //Set P2.3 as GPIO      <-- SEL[1:0] = (00)
    P2 -> SEL0 &= ~BIT3;
    P2 -> DIR  |=  BIT3;        //Set P2.3 as output

    // Run a loop setting P2.3 high/low, with a delay of delay_time between each change
    while(1)
    {
        P2 -> OUT |=  BIT3;     //Turn P2.3 High
        delay_us(delay_time);
        P2 -> OUT &= ~BIT3;     //Turn P2.3 LOW
        delay_us(delay_time);
    }
}

// Delay microseconds function
void delay_us (int delay_us) {
    int freq;
    int loops_per_second; // Track the number of for-loop cycles that occur per second, at each of the MCLK speeds (measured value)

    //Read in value of DCO divider to determine MCLK frequency. Mask in order to seperate bits of interest(bits 16->18).
    int CTL0reg = CS->CTL0 & 0x70000;   // Masking <-- 0x70000 == '111' at bits [18:16], which is where DCORSEL "lives"
    
    if (CTL0reg == CS_CTL0_DCORSEL_5)
    {
        freq = 48000000;        //Frequency of MCLK is 48MHz
        loops_per_second = 4077209;
    }
    
    else if (CTL0reg == CS_CTL0_DCORSEL_1)
    {
        freq = 3000000;         //Frequency of MCLK is 3MHz
        loops_per_second = 300601;
    }

    else if (CTL0reg == CS_CTL0_DCORSEL_2)
    {
        freq = 6000000;         //Frequency of MCLK is 6MHz
        loops_per_second = 604229;
    }

    else if (CTL0reg == CS_CTL0_DCORSEL_3)
    {
        freq = 12000000;        //Frequency of MCLK is 12MHz
        loops_per_second = 1207486;
    }

    else if (CTL0reg == CS_CTL0_DCORSEL_4)
    {
        freq = 24000000;        //Frequency of MCLK is 24MHz
        loops_per_second = 2415458;
    }

    else                        // CTL0REG defaults to 1.5MHz if invalid register value
    {
        freq = 1500000;         //Frequency of MCLK is 1.5MHz
        loops_per_second = 150705;
    }

    // Use loops/sec to find the number of loops to run
    int num_loops_todo = ceil(delay_us * (loops_per_second * 1e-6));

    // Run the delay
    int i;
    for (i = num_loops_todo; i > 0; i--); // delay

    return;
}
