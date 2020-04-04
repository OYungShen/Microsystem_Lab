#include "mbed.h"
#include "fsl_port.h"
#include "fsl_gpio.h"
#include "uLCD_4DGL.h"

#define UINT14_MAX        16383
// FXOS8700CQ I2C address
#define FXOS8700CQ_SLAVE_ADDR0 (0x1E<<1) // with pins SA0=0, SA1=0
#define FXOS8700CQ_SLAVE_ADDR1 (0x1D<<1) // with pins SA0=1, SA1=0
#define FXOS8700CQ_SLAVE_ADDR2 (0x1C<<1) // with pins SA0=0, SA1=1
#define FXOS8700CQ_SLAVE_ADDR3 (0x1F<<1) // with pins SA0=1, SA1=1
// FXOS8700CQ internal register addresses
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

I2C i2c( PTD9,PTD8);
RawSerial pc(USBTX, USBRX);
uLCD_4DGL uLCD(D1, D0, D2);
int m_addr = FXOS8700CQ_SLAVE_ADDR1;

EventQueue queue(32 * EVENTS_EVENT_SIZE);
Thread t;

char st[7];
float angle = 99.99 ;
char text[11];

void _display(){
   uLCD.locate(1,0);
   uLCD.printf("%s",text);       // display the message
   uLCD.locate(1,2);
   uLCD.printf("%1.4f",angle);   // display the angle
}

void callback_ex() {
   if(pc.readable()) {
      text[0] = pc.getc();       // get text from python
      text[1] = pc.getc();
      text[2] = pc.getc();
      text[3] = pc.getc();
      text[4] = pc.getc();
      text[5] = pc.getc();
      text[6] = pc.getc();
      text[7] = pc.getc();
      text[8] = pc.getc();
      text[9] = '\0';
      
      st[0] = pc.getc();         // get angle from python
      st[1] = pc.getc();
      st[2] = pc.getc();
      st[3] = pc.getc();
      st[4] = pc.getc();
      st[5] = pc.getc();
      st[6] = '\0';
      angle = (float) atof(st);
      queue.call( _display);
   }
}


void FXOS8700CQ_readRegs(int addr, uint8_t * data, int len);
void FXOS8700CQ_writeRegs(uint8_t * data, int len);

int main() {
   t.start(callback(&queue, &EventQueue::dispatch_forever));
//   pc.baud(115200);


   uint8_t who_am_i, data[2], res[6];
   int16_t acc16;
   float t[3];

   // Enable the FXOS8700Q

   FXOS8700CQ_readRegs( FXOS8700Q_CTRL_REG1, &data[1], 1);
   data[1] |= 0x01;
   data[0] = FXOS8700Q_CTRL_REG1;
   FXOS8700CQ_writeRegs(data, 2);
   
   // Get the slave address
   FXOS8700CQ_readRegs(FXOS8700Q_WHOAMI, &who_am_i, 1);
   pc.attach(&callback_ex);
   printf("start\r\n");
   while (true) {

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
      printf("%1.4f\r\n",t[0]);
      printf("%1.4f\r\n",t[1]);
      printf("%1.4f\r\n",t[2]);
      wait(1.0);
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