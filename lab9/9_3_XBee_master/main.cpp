#include "mbed.h"

Serial pc(USBTX, USBRX);
Serial xbee(D12, D11);

void reply_message(char *xbee_reply, char *message){
  xbee_reply[0] = xbee.getc();
  xbee_reply[1] = xbee.getc();
  xbee_reply[2] = xbee.getc();
  if(xbee_reply[1] == 'O' && xbee_reply[2] == 'K'){
   pc.printf("%s\r\n", message);
   xbee_reply[0] = '\0';
   xbee_reply[1] = '\0';
   xbee_reply[2] = '\0';
  }
}

void check_addr(char *xbee_reply, char *message){
  xbee_reply[0] = xbee.getc();
  xbee_reply[1] = xbee.getc();
  xbee_reply[2] = xbee.getc();
  pc.printf("%s = %c%c\r\n", message, xbee_reply[1], xbee_reply[2]);
  xbee_reply[0] = '\0';
  xbee_reply[1] = '\0';
  xbee_reply[2] = '\0';
}

int main(){
  int i=0;
  pc.baud(9600);

  char xbee_reply[5];

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
  xbee.printf("ATMY 0x51\r\n");
  reply_message(xbee_reply, "setting MY : 0x51");

  xbee.printf("ATDL 0x50\r\n");
  reply_message(xbee_reply, "setting DL : 0x50");

  xbee.printf("ATWR\r\n");
  reply_message(xbee_reply, "write config");

  xbee.printf("ATMY\r\n");
  check_addr(xbee_reply, "MY");

  xbee.printf("ATDL\r\n");
  check_addr(xbee_reply, "DL");

  xbee.printf("ATCN\r\n");
  reply_message(xbee_reply, "exit AT mode");

  // start
  pc.printf("Type something\r\n");

  while(1){
    while(pc.readable()){
      char c = pc.getc();
      if(c!='\r' && c!='\n'){
        pc.printf("%c", c);
        xbee.printf("%c", c);
      }
      else{
        pc.printf("\r\n");
        xbee.printf("\r\n");
      }
    }
    wait(0.001);
  }
}