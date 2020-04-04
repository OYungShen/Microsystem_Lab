#include "mbed.h"

Ticker time_up;
DigitalOut redLED(LED1);
DigitalOut greenLED(LED2);
DigitalOut blueLED(LED3);

void resetLed(){
    redLED = 1;
    greenLED = 1;
    blueLED = 1;
}

int state = 0;

void blink(){
    /*
    resetLed();
    switch(state){
        case 0 :
            redLED = 0;
            state = 1;
            break;
        case 1 :
            greenLED = 0;
            state = 2;
            break;
        case 2 :
            blueLED = 0;
            state = 0;
            break;
        default:
            break;  
            */
        redLED = !redLED;
    //}
}



int main(){
    resetLed();
    time_up.attach( &blink, 0.5 );
    while(1);
}