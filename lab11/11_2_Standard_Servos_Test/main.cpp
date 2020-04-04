#include "mbed.h"

PwmOut pin11(D11);

void stdservo_write(int value){
    int pwm_ms = (value)*750/90+1500;
    pin11 = pwm_ms/20000.0f;
}

int main() {
    pin11.period(.02);

    while(1) {
        stdservo_write(90);
        wait(1);
        stdservo_write(0);
        wait(1);
        stdservo_write(-90);
        wait(1);
    }
}