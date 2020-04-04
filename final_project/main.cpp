#include "mbed.h"
#include "bbcar.h"
#include "fsl_port.h"
#include "fsl_gpio.h"

// for FXOS8700Q
#define UINT14_MAX        16383
// FXOS8700CQ I2C address
#define FXOS8700CQ_SLAVE_ADDR0 (0x1E<<1) // with pins SA0=0, SA1=0
#define FXOS8700CQ_SLAVE_ADDR1 (0x1D<<1) // with pins SA0=1, SA1=0
#define FXOS8700CQ_SLAVE_ADDR2 (0x1C<<1) // with pins SA0=0, SA1=1
#define FXOS8700CQ_SLAVE_ADDR3 (0x1F<<1) // with pins SA0=1, SA1=1
// FXOS8700Q internal register addresses
#define FXOS8700Q_STATUS 0x00
#define FXOS8700Q_OUT_X_MSB 0x01
#define FXOS8700Q_OUT_Y_MSB 0x03
#define FXOS8700Q_OUT_Z_MSB 0x05
#define FXOS8700Q_M_OUT_X_MSB 0x33
#define FXOS8700Q_M_OUT_Y_MSB 0x35
#define FXOS8700Q_M_OUT_Z_MSB 0x37
#define FXOS8700Q_WHOAMI 0x0D
#define FXOS8700Q_XYZ_DATA_CFG 0x0E
#define FXOS8700Q_CTRL_REG1 0x2A
#define FXOS8700Q_M_CTRL_REG1 0x5B
#define FXOS8700Q_M_CTRL_REG2 0x5C
#define FXOS8700Q_WHOAMI_VAL 0xC7

EventQueue queue(64 * EVENTS_EVENT_SIZE);
Thread t;
int idplay = -1;

I2C i2c( PTD9,PTD8);
int m_addr = FXOS8700CQ_SLAVE_ADDR1;

Ticker servo_ticker;
DigitalInOut pin10(D10);
PwmOut pin9(D9), pin8(D8);
BBCar car(pin8, pin9, servo_ticker);
Serial pc(USBTX, USBRX);
Serial xbee(D12, D11);


Ticker encoder_ticker;
DigitalIn pin3(D3);

void goOneGrid();
bool findObstacle();
void forwardLeft();
void forwardRight();
void backLeft();
void backRight();
void leftCircle();

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



void FXOS8700CQ_readRegs(int addr, uint8_t * data, int len);
void FXOS8700CQ_writeRegs(uint8_t * data, int len);
void send();

int main() {
   char xbee_reply[3];

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
   xbee.printf("ATMY 0x107\r\n");
   reply_messange(xbee_reply, "setting MY : 0x107");

   xbee.printf("ATDL 0x207\r\n");
   reply_messange(xbee_reply, "setting DL : 0x207");

   xbee.printf("ATID 0x0707\r\n");
   reply_messange(xbee_reply, "setting PAN ID : 0x0707");

   xbee.printf("ATWR\r\n");
   reply_messange(xbee_reply, "write config");

   xbee.printf("ATCN\r\n");
   reply_messange(xbee_reply, "exit AT mode");
   xbee.getc();

   uint8_t data[2] ;
   // Enable the FXOS8700Q
   FXOS8700CQ_readRegs( FXOS8700Q_CTRL_REG1, &data[1], 1);
   data[1] |= 0x01;
   data[0] = FXOS8700Q_CTRL_REG1;
   FXOS8700CQ_writeRegs(data, 2);

   t.start(callback(&queue, &EventQueue::dispatch_forever));
   idplay = queue.call_every(0.1, send);
    
   while(1){
      car.goStraight(100);
      wait(1);
      car.stop();
      wait(0.5);
      car.goStraight(-100);
      wait(1);
      car.stop();
      wait(0.5);
   } 
}


void FXOS8700CQ_readRegs(int addr, uint8_t * data, int len) {
   char t = addr;
   i2c.write(m_addr, &t, 1, true);
   i2c.read(m_addr, (char *)data, len);
}

void FXOS8700CQ_writeRegs(uint8_t * data, int len) {
   i2c.write(m_addr, (char *)data, len);
}

void send() {
   int16_t acc16;
   float t[3];
   uint8_t res[6];
   FXOS8700CQ_readRegs(FXOS8700Q_OUT_X_MSB, res, 6);

   acc16 = (res[0] << 6) | (res[1] >> 2);
   if (acc16 > UINT14_MAX/2)
      acc16 -= UINT14_MAX;
   t[0] = ((float)acc16) / 4096.0f;

   acc16 = (res[2] << 6) | (res[3] >> 2);
   if (acc16 > UINT14_MAX/2)
      acc16 -= UINT14_MAX;
   t[1] = ((float)acc16) / 4096.0f;

   acc16 = (res[4] << 6) | (res[5] >> 2);
   if (acc16 > UINT14_MAX/2)
      acc16 -= UINT14_MAX;
   t[2] = ((float)acc16) / 4096.0f;

   xbee.printf("%1.3f %1.3f %1.3f\r\n",t[0],t[1],t[2]);     // x, y , z

}
