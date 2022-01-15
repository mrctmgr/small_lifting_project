#include <msp430.h>

void delay ( unsigned int ); //delay function
void pulse ( unsigned int ); //stepper motor step control function
void seven_seg( unsigned int); //seven segment display control function

int n;
int current_floor = 0;


int main(void) {
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer

    P2DIR &= ~(BIT3 + BIT4 + BIT5); //p2.3 p2.4 p2.5 set as input
    P2REN = BIT3 + BIT4 + BIT5; //enable resistor
    P2OUT |= BIT3 + BIT4 + BIT5; //resistor configuration as pull-up

    P1DIR |= (BIT0 + BIT1 + BIT2 + BIT3 + BIT5 + BIT6 +BIT7); //p1.0,1,2,3,5,6,7 set output p1.0,1,2,3 use for stepper motor inputs
    P2DIR |= (BIT0 + BIT1 + BIT2 + BIT6 + BIT7); //p2.0,1,2,6,7 set output

    seven_seg(current_floor); //initially display 0

    P1SEL &= (~BIT4); //p1.4 emergency button and it supply interrupt service routine(ISR)
    P1DIR &= (~BIT4);
    P1IES |= (BIT4);
    P1IFG &= (~BIT4);
    P1IE |= (BIT4);
    __enable_interrupt();


    for (;;) {
        while(current_floor == 0){
            if(!(P2IN & BIT3)){ //at floor 0 if pressed button1
                current_floor = 1;

                for (n=1000;n>0;n--) { //stepper motor move up 1 floor
                    pulse(BIT0+BIT1);
                    pulse(BIT1+BIT2);
                    pulse(BIT2+BIT3);
                    pulse(BIT3+BIT0);
                }
                pulse(0);
                delay(1000);
                seven_seg(current_floor); //at floor 1 display 1
            }
            else if(!(P2IN & BIT4)){//at floor 0 if pressed button2
                current_floor = 2;
                for (n=2000;n>0;n--) {  //stepper motor move up 2 floor
                    pulse(BIT0+BIT1);
                    pulse(BIT1+BIT2);
                    pulse(BIT2+BIT3);
                    pulse(BIT3+BIT0);
                }
                pulse(0);
                seven_seg(current_floor);//at floor 2 display 2
                delay(1000);

            }
            else if(!(P2IN & BIT5)){//at floor 0 if pressed button3
                current_floor =3;
                for (n=3000;n>0;n--) {//stepper motor move up 3 floor
                    pulse(BIT0+BIT1);
                    pulse(BIT1+BIT2);
                    pulse(BIT2+BIT3);
                    pulse(BIT3+BIT0);
                }
                pulse(0);
                seven_seg(current_floor); //at floor 3 display 3
                delay(1000);

            }
        }
        while(current_floor == 1){
            if(!(P2IN & BIT4)){     //at floor 1 if pressed button2
                current_floor = 2;
                for (n=1000;n>0;n--) {  //stepper motor move up 1 floor
                    pulse(BIT0+BIT1);
                    pulse(BIT1+BIT2);
                    pulse(BIT2+BIT3);
                    pulse(BIT3+BIT0);
                }
                pulse(0);
                seven_seg(current_floor);   //at floor 2 display 2
                delay(1000);

            }
            else if(!(P2IN & BIT5)){        //at floor 1 if pressed button3
                current_floor = 3;
                for (n=2000;n>0;n--) {
                    pulse(BIT0+BIT1);
                    pulse(BIT1+BIT2);
                    pulse(BIT2+BIT3);
                    pulse(BIT3+BIT0);
                }
                pulse(0);
                delay(1000);

            }
        }
        while(current_floor == 2){
            if(!(P2IN & BIT5)){         //at floor 2 if pressed button3
                current_floor = 3;
                for (n=1000;n>0;n--) {
                    pulse(BIT0+BIT1);
                    pulse(BIT1+BIT2);
                    pulse(BIT2+BIT3);
                    pulse(BIT3+BIT0);
                }
                pulse(0);
                delay(1000);

            }
            else if(!(P2IN & BIT3)){        //at floor 2 if pressed button1
                current_floor = 1;
                for (n=1000;n>0;n--) {      //stepper motor move down 1 floor
                    pulse(BIT3+BIT0);
                    pulse(BIT2+BIT3);
                    pulse(BIT1+BIT2);
                    pulse(BIT0+BIT1);
                }
                pulse(0);
                seven_seg(current_floor);
                delay(1000);

            }
        }
        while(current_floor == 3){
           if(!(P2IN & BIT4)){
               current_floor = 2;
               for (n=1000;n>0;n--) {
                   pulse(BIT3+BIT0);
                   pulse(BIT2+BIT3);
                   pulse(BIT1+BIT2);
                   pulse(BIT0+BIT1);
               }
               pulse(0);
               delay(1000);

           }
           else if(!(P2IN & BIT3)){
               current_floor = 1;
               for (n=2000;n>0;n--) {
                   pulse(BIT3+BIT0);
                   pulse(BIT2+BIT3);
                   pulse(BIT1+BIT2);
                   pulse(BIT0+BIT1);
               }
               pulse(0);
               delay(1000);

           }
        }
    }
}

void pulse(unsigned int bit) {
    P1OUT = 0+bit;
    delay(4);
}


void delay(unsigned int ms)
{
 while (ms--)
    {
        __delay_cycles(1000); // set for 16Mhz change it to 1000 for 1 Mhz
    }
}

void seven_seg( unsigned int floor){
    switch(floor){
    case 1:
        P2OUT &= ~BIT1 + ~BIT2;
        P2OUT |= BIT0 +BIT6 + BIT7;
        P1OUT |= BIT5 + BIT6;
        break;
    case 2:
        P1OUT |= BIT6;
        P1OUT &= ~BIT5;
        P2OUT &= ~(BIT0 + BIT1 + BIT6 + BIT7);
        P2OUT |= BIT2;
        break;
    case 3:
        P1OUT |= BIT6;
        P1OUT &= ~BIT5;
        P2OUT &= ~(BIT0 + BIT1 + BIT2 + BIT6);
        P2OUT |= BIT7;
        break;

    }
}
#pragma vector=PORT1_VECTOR //when the emergency button was pressed elevator move down at first floor
__interrupt void Port_1(void){
    while(!(P1IN & BIT4)){
        while(current_floor == 1)
            break;
        while(current_floor == 2){
            for (n=1000;n>0;n--) {
              current_floor = 1;
              pulse(BIT3+BIT0);
              pulse(BIT2+BIT3);
              pulse(BIT1+BIT2);
              pulse(BIT0+BIT1);
                            }
              pulse(0);
              delay(1000);

        }
        while(current_floor == 3){
            for (n=2000;n>0;n--) {
            current_floor = 1;
            pulse(BIT3+BIT0);
            pulse(BIT2+BIT3);
            pulse(BIT1+BIT2);
            pulse(BIT0+BIT1);
                           }
            pulse(0);
            delay(1000);

        }

}
    P1IFG &= ~BIT4;
}
