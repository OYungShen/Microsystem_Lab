#include "mbed.h"

InterruptIn din(D9);
BusIn receive(D0, D1, D2, D3, D4, D5, D6, D7);
DigitalIn c1(D0);
DigitalIn c2(D1);
DigitalIn c3(D2);
DigitalIn c4(D3);
DigitalIn c5(D4);
DigitalIn c6(D5);
DigitalIn c7(D6);
DigitalIn c8(D7);

Serial pc( USBTX, USBRX );

void get(){

}

int main(void) {
    int w;
    w = 'y';
    w = c1 + c2<<1 + c3<<2 + c4<<3 +c5<<4 + c6<<5 + c7<<6 + c8<<7;
    pc.printf("%c",w);
    din.rise(&get);
    while(1){
        wait(1);
    };
}