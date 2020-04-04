#include "mbed.h"

DigitalOut greenLED(LED2);
Serial pc(USBTX, USBRX);
Serial xbee(D12, D11);

void reply_messange(char *xbee_reply, char *messange){
  xbee_reply[0] = xbee.getc();
  xbee_reply[1] = xbee.getc();
  xbee_reply[2] = xbee.getc();
  if(xbee_reply[1] == 'O' && xbee_reply[2] == 'K'){
   pc.printf("%s\r\n", messange);
   xbee_reply[0] = '\0';
   xbee_reply[1] = '\0';
   xbee_reply[2] = '\0';
  }
}

int main(void){
    int i=0;
    pc.baud(9600);

    char xbee_reply[4];


  // XBee setting
  xbee.baud(9600);
  xbee.printf("+++");
  xbee_reply[0] = xbee.getc();
  xbee_reply[1] = xbee.getc();
  if(xbee_reply[0] == 'O' && xbee_reply[1] == 'K'){
   pc.printf("enter AT mode.\r\n");
   xbee_reply[0] = '\0';
   xbee_reply[1] = '\0';
  }
  xbee.printf("ATMY 0x60\r\n");
  reply_messange(xbee_reply, "setting MY : 0x60");

  xbee.printf("ATDL 0x0\r\n");
  reply_messange(xbee_reply, "setting DL : 0x0");

  xbee.printf("ATID 0x1234\r\n");
  reply_messange(xbee_reply, "setting PAN ID : 0x1234");

  xbee.printf("ATWR\r\n");
  reply_messange(xbee_reply, "write config");

  xbee.printf("ATCN\r\n");
  reply_messange(xbee_reply, "exit AT mode");
  xbee.getc();

  // start
  pc.printf("start\r\n");
  char receive_data = 0;
  greenLED = 1;

  while(1){
    receive_data = xbee.getc();
    pc.printf("Receive data %c\r\n", receive_data);
    if (receive_data == 'b') {
      greenLED = 0;
      xbee.printf("B received the data.");
    }
    wait(0.5);
    greenLED = 1;
    receive_data = 0;
  }
}