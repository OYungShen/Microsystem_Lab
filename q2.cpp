#include "mbed.h"
#include "uLCD_4DGL.h"

InterruptIn keyboard0(D3);
InterruptIn keyboard1(D4);
InterruptIn keyboard2(D5);
InterruptIn keyboard3(D6);

AnalogOut Aout(DAC0_OUT);
uLCD_4DGL uLCD(D1, D0, D2);
Timer debounce;

EventQueue queue(64 * EVENTS_EVENT_SIZE);
Thread t;
int idplay = -1;
int idlcd = -1;

int state = 0;
char dischar[5];
int coun = 0;
Serial pc(USBTX, USBRX);

void chardisplay(char playword){
    uLCD.cls();
    uLCD.locate(1,0);
    uLCD.printf("%x",playword);
}

void ptext(int state){
    pc.printf("test %x\r\n",state);
}

void sendchar(){
    int i;
    for (i = 0; i < 5; i++){
        pc.printf("%d\r\n",dischar[i]);
    }
}

void encode(void){
    wait_us(100000);
    int _press = keyboard0.read() | keyboard1.read() << 1 | keyboard2.read() << 2;
    _press |= keyboard3.read() << 3;
    if (debounce.read_ms()>1000) {
        switch (state) {
            case 0:
                dischar[coun] = '\0';
                dischar[coun] |= _press << 4;
                state = 1;
            //    queue.call(ptext,2);
                debounce.reset();
                break;
            case 1:
                dischar[coun] |= _press;
                idlcd =queue.call(chardisplay,dischar[coun]);
            //    queue.call(ptext,3);
                state = 0;
                debounce.reset();
                coun++;
                break;
            default:
                break;
        
        }
    }
    if (coun == 5){
        queue.call(sendchar);
    //    queue.call(ptext,4);
        coun = 0;
    }

}


int main(void){
    t.start(callback(&queue, &EventQueue::dispatch_forever));
    keyboard0.rise(&encode);
    keyboard1.rise(&encode);
    keyboard2.rise(&encode);
    keyboard3.rise(&encode);
    debounce.start();
    uLCD.cls();
    uLCD.locate(1,0);
    uLCD.printf("start");
    while(1);
}