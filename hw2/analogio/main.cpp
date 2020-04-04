#include "mbed.h"
AnalogOut Aout(DAC0_OUT);
BusOut display(D6, D7, D9, D10, D11, D5, D4, D8);
DigitalIn  SININ(D2);
DigitalOut  DOT(D8);
#define N 0.00077

char table[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};
int main(){
  float i;
  int sample = 0;
  int start = 0;
  float co1 = 0;
  float co0 = 0;
  int finished = 0;
  float freq;
  int digit[4];
  int j,k;

  while(finished != 1){
    for( i=0; i<2 && finished !=1; i+=0.05 ){
        Aout = 0.5 + 0.5*sin(i*3.14159);
    
    
        if (SININ < 0.5 && sample == 0){
            wait(1);
            sample = 1;
        }
    
        if (sample == 1) {
            if (SININ >= 0.5 && start == 0) {
                start = 1;
            }
            if (start == 1) {
                if (SININ >= 0.5) {
                    co1++;
                }
                else {
                    start = 2;
                }
            }
            
            if (start == 2) {
                if (SININ >= 0.5) {
                    finished = 1;
                }
                else {
                    co0++;
                }
            }
        }
      wait(N);
    }
  }

  freq = 1/(N*(co0+co1));
  digit[0] = freq / 10;
  digit[1] = freq - digit[0] * 10;
  digit[2] = (freq- digit[0]*10 - digit[1])*10;
  digit[3] = freq * 100 - digit[0] * 1000 - digit[1] * 100 - digit[2] * 10;

    while (1) {
        for (j = 0; j < 4; j++) {
            if (j == 0 && digit[j] == 0)
                j++;
            k = digit[j];
            display = table[k];
            if (j == 1)
                DOT = 1;
            else
                DOT = 0;
            wait(1);
        }
    }

}