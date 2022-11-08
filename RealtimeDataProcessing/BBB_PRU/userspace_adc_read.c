/*
This code runs the data collection process for the basic retroVLC testbed
It reads the adc data from the pru memory and prints it to a csv for later processing
by Adrian Salustri
*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/time.h>


//storage locations in shared memory for data and params
#define adc_en				pru0_dram[0]
#define num_samples			pru0_dram[2] //location PRU looks for number of sample to take
#define sample_done			pru0_dram[3] //PRU writes 0x01 when done writing
#define SAMPLE_OFFSET       0x00004      //offset in PRU dram to start ofdata
#define SAMPLE_BLOCK_SIZE   2000         //number of samples in block
#define TOTAL_SAMPLES       50000        // total number of samples to record

#define TIME_OFFSET         SAMPLE_OFFSET + NUMSAMPLES;
#define DT_ms                0.71064   //sample period (approximate, by scope measurment lol)

//PRU memory adress (Page 184 am335x TRM)
#define PRU_ADDR        0x4A300000      // Start of PRU memory 
#define PRU_LEN         0x80000         // Length of PRU memory
#define PRU0_DRAM       0x00000         // Offset to PRU0 DRAM


int* read_pru_adc(int n);

int main(){
    printf("initializing...\n");
    enum tests {DIST} test_type;
    enum panels{SMALL, MEDIUM, LARGE} panel;
    int i,j,k,lux,ang,n,shutter_n,dist,num_trials,freq;
    
    // int d_cm[] = {50, 100, 150, 200, 250, 300, 350, 400, 450, 500, 600, 700, 800, 900, 1000};
    // int d_ft[]={/*3,6,9,12,15,18,21,24,*/27,30,33,36,39,42,45};
    int d_m[]={10,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
    
    int samples[TOTAL_SAMPLES];
    float t[TOTAL_SAMPLES];
    float sample_tms = DT_ms;
    test_type = DIST; 
    FILE *fpt;
    time_t start,finish;
    
    switch (test_type){
        case DIST:
            printf("Test type:\tDistance\n");
            lux= 0; //lux reading at 1m
            ang=0;
            panel = LARGE;
            freq=20;//5, 10 or 15
            n = TOTAL_SAMPLES;
            shutter_n = 36;
            num_trials  = sizeof(d_m) / sizeof(d_m[0]);
            break;
    }
    
    //setup GPIO to turn on light
    FILE *P9_23ptr;
    P9_23ptr = fopen("/sys/class/gpio/gpio49/value","w");
    fseek(P9_23ptr,0,SEEK_SET);

   
   for(j=0;j<num_trials;j++){
       printf("Trial %d:\n",j+1);

        switch (test_type){
            case DIST:
                dist=d_m[j];
                printf("\tRecord for dist = %dm?\n\tENTER to continue...\n",dist);
                getchar();
                char filename_str[1000], header_str[1000];
                sprintf(filename_str,"d%02dm_f%01dhz_p%d.csv",dist,freq,panel);
                printf("\tOpening new file: %s\n", filename_str);
                fpt = fopen(filename_str, "w+");
                sprintf(header_str,"idx, t_ms, v, test_type, dist_cm, lux, ang_deg, panel, shutter_n, n_samples\n");
                fprintf(fpt,"%s-1,%f,0,%d, %d, %d, %d, %d, %d, %d,%d\n",
                             header_str,-DT_ms,test_type,dist,lux,ang,panel,freq,shutter_n,n);
                break;
        }
            
        
        printf("\trecording...\n");
        fprintf(P9_23ptr,"1"); fflush(P9_23ptr); //turn on light
        start = time(NULL);
            
    
           for(k=0;k<TOTAL_SAMPLES;k+=SAMPLE_BLOCK_SIZE){
                int *samples_temp = read_pru_adc(SAMPLE_BLOCK_SIZE);
        
                for(i=0;i<SAMPLE_BLOCK_SIZE;i++){
                    samples[k+i] = samples_temp[i];
                }
                for(i=0;i<100;i++){printf("%d\t(%f, %d)\n", k,t[i], samples[SAMPLE_OFFSET+i]);} //[print samples for debug
            }
        
        
        finish = time(NULL);
        printf("\tDone. Recorded for %ld seconds.\n", finish-start);
        
        fprintf(P9_23ptr,"0"); fflush(P9_23ptr);
        
    
        for(i=0;i<TOTAL_SAMPLES;i++){
            t[i]=i*DT_ms;
        }
        
        for(i=0;i<TOTAL_SAMPLES;i++){
                fprintf(fpt,"%d, %f, %d\n", i, t[i], samples[i]);//samples[SAMPLE_OFFSET+i]);
        }
        
        fclose(fpt);
    }
    fclose(P9_23ptr);

}


int* read_pru_adc(int n) {
    
    unsigned int    *pru0_dram; // Points to the start of PRU0 local DRAM
    unsigned int    *pru;       // Points to start of PRU memory.
    
    int fd,i;

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

   
    adc_en=0;                   //disable adc (ie. write 0 to pru0_dram[0])
    sample_done=0;              //clear ready flag
    num_samples=n;
    
    
    adc_en=0x01;
    while(sample_done==0);  //wait for pru0 to finish writing
    adc_en=0;
    
    
    sample_done=0; //reset ready flag
    adc_en=0x00;
    pru0_dram += 4;
    return pru0_dram;
      
}