#include "mbed.h"

#define CENTER_BASE 1500

Serial pc(USBTX, USBRX);
DigitalIn pin10(D10);
PwmOut pin11(D11);

Timer t;
Ticker encoder_ticker;

volatile int steps;
volatile int last;

void servo_control(int speed){
    if (speed > 200)       speed = 200;
    else if (speed < -200) speed = -200;

    pin11=(CENTER_BASE + speed)/20000.0f;
}

void encoder_control(){
    int value = pin10;
    if(!last && value) steps++;
    last = value;
}


int main() {

    pc.baud(9600);

    encoder_ticker.attach(&encoder_control, .01);

    pin11.period(.02);

    while(1) {

    //TODO: revise this value according to your result
    servo_control(-25.83);

    steps = 0;
    t.reset();
    t.start();

    wait(5);

    float time = t.read();

    pc.printf("%1.3f\r\n", (float)steps*6.5*3.14/32/time);

    servo_control(42.0653);

    steps = 0;
    t.reset();
    t.start();

    wait(5);

    time = t.read();

    pc.printf("%1.3f\r\n", (float)steps*6.5*3.14/32/time);
    }
}