#include "mbed.h"
#include "uLCD_4DGL.h"
AnalogOut Aout(DAC0_OUT);
AnalogIn  SININ(A0);
uLCD_4DGL uLCD(D1, D0, D2); // serial tx, serial rx, reset pin;
#define N 0.00077
Serial pc( USBTX, USBRX );

int main(){
  float i;
  int sample = 0;
  int start = 0;
  float co1 = 0;
  float co0 = 0;
  int finished = 0;
  float freq;
  float min = 1, max = 0;
  float amp;
  float value;

  while(finished != 1){
    for( i=0; i<2 && finished !=1; i+=0.05 ){
        Aout = 0.5 + 0.5*sin(i*3.14159);
    
        value = SININ;
        if (value < 0.5 && sample == 0){
            pc.printf("sample start\r\n");          // For debug
            sample = 1;
        }
    
        if (sample == 1) {
            if (value >= 0.5 && start == 0) {
                start = 1;
            }
            if (start == 1) {
                if (value >= 0.5) {
                    pc.printf("hight test\r\n");    // For debug
                    co1++;
                    if (value > max) {
                        max = value;
                    }
                    
                }
                else {
                    start = 2;
                }
            }
            
            if (start == 2) {
                if (value >= 0.5) {
                    if ((co1+co0) < 10) {       // If error sample, resample 
                        max = 0;
                        min = 1;
                        co1 = 0;
                        co0 = 0;
                        sample = 0;
                        start = 0;
                    }
                    else
                        finished = 1;
                }
                else {
                    co0++;
                    pc.printf("low test\r\n");
                    if (value < min) {
                        min = value;
                    }
                }
            }
        }
      wait(N);
    }
  }
 

  amp = 3.3*(max-min);
  freq = 1/(N*(co0+co1));
    uLCD.color(RED);
    uLCD.locate(0,2);
    uLCD.printf("Anplitude=%.3f v",amp);
    uLCD.locate(0,3);
    uLCD.printf("Frequency= %.3f HZ",freq);

}