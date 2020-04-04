#include "mbed.h"

Serial pc(USBTX, USBRX);
DigitalInOut pin11(D11);
Timer t;

int main() {

    float ping;
    pc.baud(9600);

    while(1) {

        pin11.output();
        pin11 = 0; wait_us(200);
        pin11 = 1; wait_us(5);
        pin11 = 0; wait_us(5);

        pin11.input();
        while(pin11.read()==0);
        t.start();
        while(pin11.read()==1);
        ping = t.read();
        printf("Ping = %lf\r\n", ping*17700.4f);
        t.stop();
        t.reset();

        wait(1);
    }
}