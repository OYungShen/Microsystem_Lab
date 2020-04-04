#include "mbed.h"
#include "arm_math.h"
#include "FXOS8700CQ.h"
#include "bbcar.h"
#include <math.h>
#include <stdlib.h>

#define bound .9

Serial pc(USBTX, USBRX);
Ticker servo_ticker;
PwmOut pin9(D9), pin8(D8);
BBCar car(pin8, pin9, servo_ticker);

FXOS8700CQ acc(PTD9, PTD8, (0x1D<<1));

int main(){
  //pid control setup
  arm_pid_instance_f32* pid=(arm_pid_instance_f32 *)malloc(sizeof(arm_pid_instance_f32));
  pid->Kp = 1.0; pid->Ki = 1.0; pid->Kd = 0;
  arm_pid_init_f32(pid,1);

  //sensor setup
  acc.enable();
  SRAWDATA accel_data, magn_data;

  char rotation;
  char buff[256];
  float degree, target_degree, diff;

  while(1){
    wait(1);
    for( int i=0; i<1; i++ ) {
        rotation = pc.putc(pc.getc());
    }
    for( int i=0; i<2; i++ ) {
        buff[i] = pc.putc(pc.getc());
    }
    pc.printf("\r\n");

    int turn = atoi(buff);

    acc.get_data(&accel_data, &magn_data);
    degree = atan2(magn_data.y, magn_data.x) * 180 / PI;

    if(rotation == 'l'){
      target_degree = degree - turn;
    }else if(rotation == 'r'){
      target_degree = degree + turn;
    }else{
      target_degree = degree;
    }

    if(target_degree < -180){
        target_degree = 360 + target_degree;
    }else if(target_degree > 180){
        target_degree = 360 - target_degree;
    }

    diff = 0.1 * (degree - target_degree);

    //The car will continue to turn to the target degree until the error is small enough
    while(abs(diff) > 0.5){
      acc.get_data(&accel_data, &magn_data);
      degree = atan2(magn_data.y, magn_data.x) * 180 / PI;

      diff = 0.1 * (degree - target_degree);
      pc.printf("degree:%f, target: %f, diff:%f \r\n", degree, target_degree, diff);

      //Process the PID control
      float correction = arm_pid_f32(pid, diff);
      //bound the value from -9 to 9
      correction = car.clamp(correction, bound, -bound);
      printf("correction = %3.2f \r\n", correction);
      float turn = (rotation == 'l') ? (1-abs(correction)) : (-1+abs(correction));
      car.turn(car.turn2speed(turn),turn);

      wait(0.2);
    }
    car.stop();
    arm_pid_init_f32(pid,1);
  }
}