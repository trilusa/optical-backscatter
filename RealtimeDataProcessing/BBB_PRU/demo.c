/*
This program does frequency idewntification fror the retorVLC demo
by Alex H and adrian S
*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>

//storage locations in shared memory for data and params
#define PRU0_ADC_EN			        	pru0_dram[0]
#define PRU0_ADC_NUM_SAMPLES			pru0_dram[2] //location PRU looks for number of sample to take
#define PRU0_ADC_SAMPLING_DONE_FLAG		pru0_dram[3] //PRU writes 0x01 when done writing
#define SAMPLE_OFFSET                   0x00004      //offset in PRU dram to start ofdata

#define SAMPLE_BLOCK_SIZE               2000         //number of samples in block
#define DT_ms                           0.71064  //sample period (approximate, by scope measurment lol)

//PRU memory adresses (Page 184 am335x TRM)
#define PRU_ADDR                        0x4A300000      // Start of PRU memory 
#define PRU_LEN                         0x80000         // Length of PRU memory
#define PRU0_DRAM                       0x00000         // Offset to PRU0 DRAM

int* read_pru_adc(int n);

int main(){
    unsigned int    *pru;       // Points to start of PRU memory.
    unsigned int    *pru0_dram; // Points to the start of PRU0 local DRAM

    int fd;
    fd = open ("/dev/mem", O_RDWR | O_SYNC);
    if (fd == -1) {
        printf ("ERROR: could not open /dev/mem.\n\n");
        return 1;
    }
    pru = mmap (0, PRU_LEN, PROT_READ | PROT_WRITE, MAP_SHARED, fd, PRU_ADDR);
    if (pru == MAP_FAILED) {
        printf ("ERROR: could not map memory.\n\n");
        return 1;
    }
    close(fd);

    
    pru0_dram = pru + PRU0_DRAM/4 + 0x200/4;   // Points to 0x200 of PRU0 memory
    PRU0_ADC_EN=0;                   //disable adc (ie. write 0 to pru0_dram[0])
    PRU0_ADC_SAMPLING_DONE_FLAG=0;              //clear ready flag
    PRU0_ADC_NUM_SAMPLES=SAMPLE_BLOCK_SIZE;

    //setup GPIO to turn on light
    FILE *P9_23ptr;
    P9_23ptr = fopen("/sys/class/gpio/gpio49/value","w");
    fseek(P9_23ptr,0,SEEK_SET);
    fprintf(P9_23ptr,"1"); fflush(P9_23ptr);    //turn on light


    int i,k;
    for(k=0;k<5;k++){
        PRU0_ADC_EN=0x01; //enable adc
        while(PRU0_ADC_SAMPLING_DONE_FLAG==0);  //wait for pru0 to finish writing samples to shared mem
        PRU0_ADC_EN=0x00; //disable adc
        PRU0_ADC_SAMPLING_DONE_FLAG=0; //clear ready flag
    
        // do whatever processing you need, print the first 100 samples 
        for(i=0;i<100;i++){
            printf("%d\n", pru0_dram[i]);
        }
    }

    //turn off light when done, free file ptr
    fprintf(P9_23ptr,"0"); fflush(P9_23ptr);  
    fclose(P9_23ptr); 
}
