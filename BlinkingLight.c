#include "msp.h"
#include "Set_DCO.h"

void delay_us(int us);


int main(void)
{
    set_DCO(FREQ_6_MHz);

    P2 -> SEL1 &=~ BIT3;        //Set P2.3 as GPIO
    P2 -> SEL0 &=~ BIT3;        //Set P2.3 as GPIO
    P2 -> DIR  |=  BIT3;        //Set P2.3 as output

    while(1)
    {
        P2 -> OUT |=  BIT3;     //Turn P2.3 High
        delay_us(500);
        P2 -> OUT &=~ BIT3;     //Turn P2.3 LOW
        delay_us(500);
    }
}

// Delay milliseconds function
void delay_us (int us) {
    int freq;
    //Read in value of DCO divider to determine MCLK frequency. Mask in order to seperate bits of interest(bits 16->18).
    int CTL0reg = CS->CTL0 & 0x70000;   // Masking <-- 0x70000 == '111' at bits [18:16], which is where DCORSEL "lives"
    
    if (CTL0reg == CS_CTL0_DCORSEL_5)
    {
        freq = 48000000;        //Frequency of MCLK is 48MHz
    }
    
    else if (CTL0reg == CS_CTL0_DCORSEL_1)
    {
        freq = 3000000;         //Frequency of MCLK is 3MHz
    }

    else if (CTL0reg == CS_CTL0_DCORSEL_2)
    {
        freq = 6000000;         //Frequency of MCLK is 6MHz
    }

    else if (CTL0reg == CS_CTL0_DCORSEL_3)
    {
        freq = 12000000;        //Frequency of MCLK is 12MHz
    }

    else if (CTL0reg == CS_CTL0_DCORSEL_4)
    {
        freq = 24000000;        //Frequency of MCLK is 24MHz
    }

    else                        // CTL0REG defaults to 1.5MHz if invalid register value
    {
        freq = 1500000;         //Frequency of MCLK is 1.5MHz
    }

    printf("The frequency is: %d",freq);

    int i, j;
    for (j = 0; j < us; j++)
        for (i = 585*(freq/FREQ_3_MHz); i > 0; i--); // delay 1 ms(approx)
}
