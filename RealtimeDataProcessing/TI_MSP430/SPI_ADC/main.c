#include <msp430.h>
volatile int bytestate = 0;
volatile unsigned int sample = 0x62F5;
int main(void)
{
    WDTCTL = WDTPW + WDTHOLD;                 // Stop watchdog timer

        //setup spi

    UCA0CTL1 |= UCSWRST;                      // Disable SPI for setup
    UCA0CTL0 = 0b01100001;                    // see User Guide p. 445

    // Set P1.4 to SCLK
    P1SEL |= BIT4;
    P1SEL2 |= BIT4;

    // Set P1.1 to SOMI
    P1SEL |= BIT1;
    P1SEL2 |= BIT1;

    // Set P1.2 to MOSI
    P1SEL |= BIT2;
    P1SEL2 |= BIT2;

    // Set P1.4 to CS (UCA0STE)
    P1SEL |= BIT5;
    P1SEL2 |= BIT5;


//    IE2 |= UCA0TXIE;                           //Enable TX interupts
    IFG2 &= ~UCA0TXIFG ;                       // clear TX done flag

    /* Enable ADC */

    ADC10CTL0 |= ADC10ON;

    /* Setup Interupt on P2.0 */
//    P2IE |= BIT0;
//    P2IES |= BIT0;
//    P2IFG &= ~BIT0;

    UCA0CTL1 &= ~UCSWRST;                               // Enable SPI
//    __enable_interrupt();
//    IFG2 |= UCA0TXIFG;                                // manually trigger to load TXBUF
    while(1){
        UCA0TXBUF = 0xFF; //default TX
        if((P2IN & BIT0)==0){
            ADC10CTL0 |= ENC;
            ADC10CTL0 |= ADC10SC;                    //start adc read
            while((ADC10CTL0 & ADC10IFG) == 0);         //wait for conversion to finish
            sample = ADC10MEM;                          //grab sample
            ADC10CTL0 &= ~ENC;

            UCA0TXBUF =  (sample >> 8);                 //TX high byte
            while((IFG2 & UCA0TXIFG) == 0);             //wait for TX done

            UCA0TXBUF = (sample & 0x00FF);              //TX low byte
            while((IFG2 & UCA0TXIFG) == 0);              //wait for TX done

            while(~(P2IN & BIT0)==BIT0);                //wait for CS to go high
        }
    }
}

//
//#pragma vector= PORT2_VECTOR
//__interrupt void ISR_PORT2(void){
////    ADC10CTL0 |= ADC10SC;                       //start adc read
////    while((ADC10CTL0 & ADC10IFG) == 0);         //wait for conversion to finish
//    __delay_cycles(100);
////    sample = ADC10MEM;                          //retrieve result
//    UCA0TXBUF =  (sample >> 8);  //put high byte of result into SPI TX buffer
//    bytestate = 1;
////    while(UUCA0STAT & CBUSY);
//    UCA0TXBUF = 0x15;
//    P1IFG &= ~BIT0;
////    IFG2 |= UCA0TXIFG;
//}
//
//#pragma vector = USCIAB0TX_VECTOR
//__interrupt void  ISR_USCIAB0TX(void){
//    //TX sample Low byte
////    if (bytestate==1){
////        UCA0TXBUF = 0x15;// (sample & 0x00FF); //load low byte into TX register
//        bytestate=0;
////    }
//}
