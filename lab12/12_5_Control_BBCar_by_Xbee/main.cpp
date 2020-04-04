#include "mbed.h"
#include "bbcar.h"
#include "bbcar_rpc.h"

Ticker servo_ticker;
PwmOut pin9(D9), pin8(D8);
Serial xbee(D12, D11);
BBCar car(pin8, pin9, servo_ticker);
Serial pc(USBTX, USBRX);


int main() {
    pc.baud(9600);
    char xbee_reply[3];
    char message[50];

    // XBee setting
    xbee.baud(9600);
    xbee.printf("+++");
    xbee_reply[0] = xbee.getc();
    xbee_reply[1] = xbee.getc();
    if(xbee_reply[0] == 'O' && xbee_reply[1] == 'K'){
    xbee_reply[0] = '\0';
    xbee_reply[1] = '\0';
    }
    xbee.printf("ATMY 0x107\r\n");
    xbee.printf("ATDL 0x207\r\n");
    xbee.printf("ATWR\r\n");
    xbee.printf("ATMY\r\n");
    xbee.printf("ATDL\r\n");
    xbee.printf("ATCN\r\n");


    char buf[256], outbuf[256];
    while (1) {
        for( int i=0; ;i++ ) {
            buf[i] = xbee.getc();
            if(buf[i] == '\n') break;
        }

        pc.printf("%s\r\n",buf);

        RPC::call(buf, outbuf);
        xbee.printf("%s\r\n", outbuf);
    }
}