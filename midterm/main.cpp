#include "mbed.h"
#include "fsl_port.h"
#include "fsl_gpio.h"
#include <math.h>
#include "uLCD_4DGL.h"

// for playnote
#define waveformLength (128)
#define lookUpTableDelay (10)

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

// for play note
InterruptIn keyboard0(D3);
InterruptIn keyboard1(D4);
InterruptIn keyboard2(D5);
InterruptIn keyboard3(D6);
InterruptIn keyboard4(D7);
InterruptIn keyboard5(D8);
InterruptIn keyboard6(D9);
InterruptIn keyboard7(D10);
InterruptIn keyboard8(D11);
InterruptIn keyboard9(D12);
InterruptIn keyboard10(D13);
InterruptIn keyboard11(D14);

InterruptIn button(SW2);
Timer debounce;

AnalogOut Aout(DAC0_OUT);
uLCD_4DGL uLCD(D1, D0, D2);

// for FXOS8700Q
I2C i2c( PTD9,PTD8);
int m_addr = FXOS8700CQ_SLAVE_ADDR1;

Serial pc(USBTX, USBRX);
EventQueue queue(64 * EVENTS_EVENT_SIZE);
Thread t;
int idplay = -1;
int idlcd = -1;

float waveform[waveformLength] = { 0.500, 0.525, 0.549, 0.574, 0.598,
                                   0.622, 0.646, 0.670, 0.693, 0.715,
                                   0.737, 0.759, 0.780, 0.800, 0.819,
                                   0.838, 0.856, 0.873, 0.889, 0.904,
                                   0.918, 0.931, 0.943, 0.954, 0.964,
                                   0.972, 0.980, 0.986, 0.991, 0.995,
                                   0.998, 1.000, 1.000, 0.999, 0.997,
                                   0.994, 0.989, 0.983, 0.976, 0.968,
                                   0.959, 0.949, 0.937, 0.925, 0.911,
                                   0.896, 0.881, 0.864, 0.847, 0.829,
                                   0.810, 0.790, 0.769, 0.748, 0.726,
                                   0.704, 0.681, 0.658, 0.634, 0.610,
                                   0.586, 0.562, 0.537, 0.512, 0.488,
                                   0.463, 0.438, 0.414, 0.390, 0.366,
                                   0.342, 0.319, 0.296, 0.274, 0.252,
                                   0.231, 0.210, 0.190, 0.171, 0.153,
                                   0.136, 0.119, 0.104, 0.089, 0.075,
                                   0.063, 0.051, 0.041, 0.032, 0.024,
                                   0.017, 0.011, 0.006, 0.003, 0.001,
                                   0.000, 0.000, 0.002, 0.005, 0.009,
                                   0.014, 0.020, 0.028, 0.036, 0.046,
                                   0.057, 0.069, 0.082, 0.096, 0.111,
                                   0.127, 0.144, 0.162, 0.181, 0.200,
                                   0.220, 0.241, 0.263, 0.285, 0.307,
                                   0.330, 0.354, 0.378, 0.402, 0.426,
                                   0.451, 0.475, 0.500 };

float Harmonicswave[128];

float note[12] = {131,139,147,156,165,175,185,196,208,220,233,247};
float wave[1000];

float getAmplitude();

void playHarmonics(int freq)
{
  float amplitude;
  int i = freq;
  int j = waveformLength;
  int waitTime = (1000000/waveformLength/freq - lookUpTableDelay) << 0;

  amplitude = getAmplitude();
  while(i--)
  {
    j = waveformLength;
    while(j--)
    {
      Aout = Harmonicswave[j] * amplitude;
      wait_us(waitTime);
    }
  }
}

void playNote(int freq)
{
  float amplitude;
  int i = freq;
  int j = waveformLength;
  int waitTime = (1000000/waveformLength/freq - lookUpTableDelay) << 0;

  amplitude = getAmplitude();
//  pc.printf("amplitude is %1.3f \r\n",amplitude);
  while(i--)
  {
    j = waveformLength;
    while(j--)
    {
      Aout = waveform[j] * amplitude;
      wait_us(waitTime);
    }
  }
}

void playdtfm(){
    int i;
    float amplitude;
    int waitTime = (1000000/1000 - lookUpTableDelay) << 0;
    amplitude = getAmplitude();
  //  pc.printf("amplitude is %1.3f \r\n",amplitude);
    for (i = 0; i < 1000; i++){
        Aout = wave[i] * amplitude;
        wait_us(waitTime);
    }
}

int key = 0;
int key2 = -1;
int twokey = 0;
int mode = 0;
void dtfm(int freq1, int freq2){
    float interval = 1.0/1000.0;
    int i;

    for (i = 0; i < 1000; i++){
        wave[i] = 0.5 + 0.25*(sin(i*freq1*2*3.1415926*interval) + sin(i*freq2*2*3.1415926*interval));
    }

}


void ptext(int state){
    pc.printf("test %x\r\n",state);
}

void oneKeyDisplay(int key, float freq){
  uLCD.cls();
  uLCD.locate(1,0);
  if (mode == 0)
    uLCD.printf("Normal mode");
  else 
    uLCD.printf("Harmonic mode");
  uLCD.locate(1,2);
  uLCD.printf("key %d is pressed",key);
  uLCD.locate(1,4);
  uLCD.printf("Current note is %1.3f",freq);

}

void twoKeyDisplay(int key, int key2){
  uLCD.cls();
  
  uLCD.locate(1,0);
  uLCD.printf("key %d and %d is pressed",key, key2);
  uLCD.locate(1,3);
  uLCD.printf(" %1.1f and %1.1f",note[key], note[key2]);

}

void read_and_play(void) {
    twokey = 0;
    int _press = keyboard0.read() | keyboard1.read() << 1 | keyboard2.read() << 2;
    _press |= keyboard3.read() << 3 | keyboard4.read() << 4 | keyboard5.read() << 5;
    _press |= keyboard6.read() << 6 | keyboard7.read() << 7 | keyboard8.read() << 8;
    _press |= keyboard9.read() << 9 | keyboard10.read() << 10 | keyboard11.read() << 11;   ////////////////
    int state = 0;                      // switch state
    //_press &= 0x0fff;                     // clear otherbit  ///////////////////

    if(idplay != -1){
       queue.cancel(idplay);
       queue.cancel(idlcd);
    }
    
    queue.call(ptext,_press);

    // scan
    for (int i = 0; i < 12; i++) {      ////////////////////
        if (_press & (1 << i)) {
            key = i;
            state = 0;                  // 1 for 1 key pressed
            if (twokey == 0){           // check whethere there are two key
                key2 = key;
                twokey = 1;
            } else {
                twokey = 2;
                state = 1;             // 2 for 2 key pressed
            }
        }
    }
    if(!_press)
        key = -1;
    // no key is pressed
    if(mode == 0) {
      switch(state) {
          case 0 :
              idlcd = queue.call(oneKeyDisplay, key, note[key]);
              idplay = queue.call_every(1, playNote, note[key]);
              break;
          case 1 :
              idlcd =queue.call(twoKeyDisplay,key2,key);
              dtfm(note[key],note[key2]);
              idplay = queue.call_every(1, playdtfm);
              break;
          default:
            break;
      }
    }
    else {
      idlcd = queue.call(oneKeyDisplay, key, note[key]);
      idplay = queue.call_every(1, playHarmonics, note[key]);
    }
}

void changemode(){
  if(debounce.read_ms()>1000){
        mode = !mode;
        debounce.reset();
    }
}

void getHarmonicwave();

void FXOS8700CQ_readRegs(int addr, uint8_t * data, int len);
void FXOS8700CQ_writeRegs(uint8_t * data, int len);

float getAngle();

void stopPlay(void) {
  queue.cancel(idplay);
}

int main(void) {
    t.start(callback(&queue, &EventQueue::dispatch_forever));
    debounce.start();
    getHarmonicwave();

    keyboard0.rise(&read_and_play);
    keyboard1.rise(&read_and_play);
    keyboard2.rise(&read_and_play);
    keyboard3.rise(&read_and_play);
    keyboard4.rise(&read_and_play);
    keyboard5.rise(&read_and_play);
    keyboard6.rise(&read_and_play);
    keyboard7.rise(&read_and_play);
    keyboard8.rise(&read_and_play);
    keyboard9.rise(&read_and_play);
    keyboard10.rise(&read_and_play);
    keyboard11.rise(&read_and_play);

    button.rise(&changemode);

    keyboard0.fall(queue.event(stopPlay));
    keyboard1.fall(queue.event(stopPlay));
    keyboard2.fall(queue.event(stopPlay));
    keyboard3.fall(queue.event(stopPlay));
    keyboard4.fall(queue.event(stopPlay));
    keyboard5.fall(queue.event(stopPlay));
    keyboard6.fall(queue.event(stopPlay));
    keyboard7.fall(queue.event(stopPlay));
    keyboard8.fall(queue.event(stopPlay));
    keyboard9.fall(queue.event(stopPlay));
    keyboard10.fall(queue.event(stopPlay));
    keyboard11.fall(queue.event(stopPlay));

    uint8_t data[2] ;
    // Enable the FXOS8700Q
    FXOS8700CQ_readRegs( FXOS8700Q_CTRL_REG1, &data[1], 1);
    data[1] |= 0x01;
    data[0] = FXOS8700Q_CTRL_REG1;
    FXOS8700CQ_writeRegs(data, 2);

    while (1);
}

float getAngle() {
   int16_t acc16;
   float t[3];
   uint8_t res[6];
   float cosvalue;
   float angle;
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

  cosvalue = t[2] / sqrt((t[0]*t[0]+t[1]*t[1]+t[2]*t[2]));
  angle = acos(cosvalue) * 180 / 3.1415926;
  return angle;
}



void FXOS8700CQ_readRegs(int addr, uint8_t * data, int len) {
   char t = addr;
   i2c.write(m_addr, &t, 1, true);
   i2c.read(m_addr, (char *)data, len);
}

void FXOS8700CQ_writeRegs(uint8_t * data, int len) {
   i2c.write(m_addr, (char *)data, len);
}

float getAmplitude(){
  float angle;
  float amplitude;
  angle = getAngle();
    
      if (angle < 15.0)
        amplitude = 1;
      if (15.0 < angle && angle < 30.0)
        amplitude = 11.0/12.0;
      if (30 < angle && angle < 45)
        amplitude = 10.0/12.0;
      if (45 < angle && angle < 60)
        amplitude = 9.0/12.0;
      if (60 < angle && angle < 75)
        amplitude = 8.0/12.0;
      if (75 < angle && angle < 90)
        amplitude = 7.0/12.0;
      if (90 < angle && angle < 105)
        amplitude = 6.0/12.0;
      if (105 < angle && angle < 120)
        amplitude = 5.0/12.0;
      if (120 < angle && angle < 135)
        amplitude = 4.0/12.0;
      if (135 < angle && angle < 150)
        amplitude = 3.0/12.0;
      if (150 < angle && angle < 165)
        amplitude = 2.0/12.0;
      if (165 < angle && angle < 180)
        amplitude = 1.0/12.0;

      return amplitude;

}

void getHarmonicwave() {
  float interval = 1.0/128.0;
  int i;

    for (i = 0; i < 128; i++){
        Harmonicswave[i] = 0.5 + 0.5/3.0*(sin(i*2*3.1415926*interval) + sin(i*2*2*3.1415926*interval) + sin(i*3*2*3.1415926*interval));
    }
}
