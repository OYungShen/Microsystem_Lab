#include "mbed.h"

BusOut display(D6, D7, D9, D10, D11, D5, D4, D8);
PwmOut PWM1(D3);
DigitalIn  PWMIN(D2);
DigitalOut  DOT(D8);

char table[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};

int main(){
  int sample = 1;
  float co1 = 0;
  float co0 = 0;
  int start = 0;
  int get;
  int getstart = 1;
  float result;
  int digit[3];
  int i,j;

    PWM1.period(0.1);
    PWM1 = 0.4;
    while (getstart == 1){
        if (PWMIN == 0){
            get = PWMIN;
            getstart = 0;
        }
    }

    while (sample == 1) {
        if (get != PWMIN) {
            start = 1;
            co1++;
            wait_us(100);
        }

        while (start == 1){
            if (PWMIN == 1){
                co1++;
                wait_us(100);
            }
            else {
                start = 2;
                co0++;
                wait_us(100);
            }
        }

        while(start == 2){
            if(PWMIN == 0){
                 co0++;
                wait_us(100);
            }
            else {
                sample = 0;
                start = 0;
            } 
        }
    }


    result = co1/(co1+co0);
    digit[0] = result;
    digit[1] = (result-digit[0])*10; 
    digit[2] = (result*10-digit[0]*10-digit[1])*10;

    while(1) {
        if (result != 1) {
            for (i = 0; i < 3; i++) {
                j = digit[i];
                display = table[j];
                if (i == 0) {
                    DOT = 1;
                }
                else {
                    DOT = 0;
                }
                wait(1);
            }
        }
        else {
            display = table[1];
        }
    }
}