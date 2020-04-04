#include "mbed.h"

Serial pc(USBTX, USBRX);
DigitalIn pin11(D11);

Ticker encoder_ticker;

volatile int steps;
volatile int last;

void encoder_control(){
    int value = pin11;
    if(!last && value) steps++;
    last = value;
}

int main() {
    pc.baud(9600);
    pc.printf("Before start\r\n");

    encoder_ticker.attach(&encoder_control, .01);

    steps = 0;

    while(1) {
        wait(2);
        pc.printf("encoder = %d\r\n", steps);
    }
}