#include "mbed.h"
#include "uLCD_4DGL.h"
AnalogOut Aout(DAC0_OUT);
AnalogIn  SININ(A0);
DigitalIn DIN(D3);
uLCD_4DGL uLCD(D1, D0, D2); // serial tx, serial rx, reset pin;
#define N 0.01
Serial pc( USBTX, USBRX );

int main(){
  float i;
  int sample = 0;
  int start = 0;
  int point = 100;  
  int finished = 0;
  float co1 = 0;
  float co0 = 0;
  float freq;
  float min = 1, max = 0;
  float amp;
  float value;
  float Asignal[100];                   // sample point of analog
  int Dsignal[100];                     // sample point of digital
  int j = 0;                            // compute sample point

  while(j < point){
    for( i=0; i<2 && j < point; i+=0.1 ){
        Aout = 0.5 + 0.5*sin(i*3.14159);
    
        value = SININ;
        if (value < 0.5 && sample == 0){
            sample = 1;
        }
    
        if (sample == 1) {
            if (value >= 0.5 && start == 0) {
                start = 1;
            }
            if (start == 1) {
                if (value >= 0.5) {
                    Asignal[j] = SININ;         // get value
                    Dsignal[j] = DIN;
                    j++;
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
                        j = 0;
                    }
                    else
                        start = 3;
                }
                else {
                    Asignal[j] = SININ;         // get value
                    Dsignal[j] = DIN;
                    j++;
                    co0++;
                    if (value < min) {
                        min = value;
                    }
                }
            }

            if(start == 3){
                Asignal[j] = SININ;         // get value
                    Dsignal[j] = DIN;
                    j++;
            }

        }
      wait(N);
    }
  }
 
  for( j = 0; j < point; j++){
    pc.printf("%1.3f\n",Asignal[j]);        // output the result
  }
  for( j = 0; j < point; j++){
    pc.printf("%d\n",Dsignal[j]);
  }

  amp = 3.3*(max-min);
  freq = 1/(N*(co0+co1));
    uLCD.color(RED);
    uLCD.locate(0,2);
    uLCD.printf("Anplitude=%.3f v",amp);
    uLCD.locate(0,3);
    uLCD.printf("Frequency= %.3f HZ",freq);

}