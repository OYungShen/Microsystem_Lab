#include "mbed.h"
#include "mbed_rpc.h"

Serial pc(USBTX, USBRX);
DigitalOut redLED(LED1);
DigitalOut blueLED(LED3);



void red(Arguments *in, Reply *out);
void blue(Arguments *in, Reply *out);

RPCFunction redBlink(&red, "red");
RPCFunction blueBlink(&blue, "blue");

int main(){
    char buf[256], outbuf[256];
    redLED = 1;
    blueLED = 1;
    while (true) {

        pc.gets(buf, 15);
        RPC::call(buf, outbuf);
        pc.printf("%s\r\n", outbuf);

   }
}

void red(Arguments *in, Reply *out) {
    char buf[256], outbuf[256];
    while(1) {
        redLED = 0;
        wait (1);
        redLED = 1;
        wait (1);
        if(pc.readable()){
            pc.gets(buf, 15);
            RPC::call(buf, outbuf);
            pc.printf("%s\r\n", outbuf);
        }
    }
}

void blue(Arguments *in, Reply *out) {
    char buf[256], outbuf[256];
    while(1) {
        blueLED = 0;
        wait (1);
        blueLED = 1;
        wait (1);
        if(pc.readable()){
            pc.gets(buf, 15);
            RPC::call(buf, outbuf);
            pc.printf("%s\r\n", outbuf);
        }
    }
}