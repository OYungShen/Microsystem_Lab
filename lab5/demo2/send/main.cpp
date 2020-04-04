#include "mbed.h"
DigitalOut up(D10);
BusOut send(D0, D1, D2, D3, D4, D5, D6, D7);
Serial pc( USBTX, USBRX );

int main (void) {
    char a[18] = {'E','E',' ','N','T','H','U',' ','1','0','6','0','0','0','1','4','7','/0'};
    int i ;
    int test;
    up = 1;
    for (i = 0; i < 18 ; i++){
        send = a[i];
        up = 0;
       // test = send;
       //pc.printf("%c",test);
        wait(1);
        up = 1;
    }
    
}