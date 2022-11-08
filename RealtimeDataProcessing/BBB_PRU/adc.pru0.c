/*
This code runs on pru0 on the BBB. It implements a basic SPI protocol to recieve samples from an external SPI ADC IC
by Adrian Salustri
*/

#include <stdint.h>
#include <pru_cfg.h>
#include "resource_table_empty.h"
#include "prugpio.h"

#define PRU0_DRAM			0x00000         // Offset to DRAM

volatile register unsigned int __R30;
volatile register unsigned int __R31;

#define CS P9_27
#define SDO (1<<16)
#define SCLK P9_30

#define adc_en				pru0_dram[0]
#define sample_time_ns		pru0_dram[1]
#define sample_time_c		sample_time_ns/5
#define num_samples			pru0_dram[2]
#define sample_done			pru0_dram[3]
#define sample_start_idx	0x00000004


// Skip the first 0x200 byte of DRAM since the Makefile allocates
// 0x100 for the STACK and 0x100 for the HEAP.
volatile unsigned int *pru0_dram = (unsigned int *) (PRU0_DRAM + 0x200);

void main(void) {
	CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;				 // Clear SYSCFG[STANDBY_INIT] to enable OCP master port */
	__R30 |= (CS | SCLK);
	unsigned int s,hb,lb;
	while(1){
		while(adc_en!=1);							//adc not enabled yet
		
		for(s=0;s<=num_samples;s++){
			__R30 &=~  CS;							//CS Low, start adc
			__delay_cycles(50000);

			hb=spiReadBits(8);
			 __delay_cycles(10000);
			lb=spiReadBits(8);
			if(s>0){								//first read is trash
				pru0_dram[sample_start_idx+s-1] = (hb<<8) | lb;
			}
			__R30 |= CS;							//CS High
			__delay_cycles(50000);

			
		}
		sample_done = 1;
		adc_en=0;
	}
}

unsigned char spiReadBits(int numBits){
	unsigned int i,sample;
	sample=0;
	for(i=0;i<numBits;i++){
		sample = (sample << 1);
		__R30 &=~ SCLK;						//SCLK Low
		__delay_cycles(1000);

		__R30 |= SCLK;						//SCLK High
		if(__R31 & SDO) {
			sample += 1;
		}
		__delay_cycles(1000);
	}
	return sample;
}
