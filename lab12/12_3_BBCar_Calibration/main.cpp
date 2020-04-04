#include "mbed.h"
#include "bbcar.h"

Ticker servo_ticker;
PwmOut pin8(D8), pin9(D9);

BBCar car(pin8, pin9, servo_ticker);

int main() {
    // please contruct you own calibration table with each servo
    double pwm_table0[] = {-150, -120, -90, -60, -30, 0, 30, 60, 90, 120, 150};
    double speed_table0[] = {-6.698, -6.857, -6.538, -6.307, -5.581, 0, 4.624, 5.10, 5.581, 6.060, 6.379 };
    double pwm_table1[] = {-150, -120, -90, -60, -30, 0, 30, 60, 90, 120, 150};
    double speed_table1[] = {-6.857, -6.733, -6.368, -6.698, -4.784, 0.000, 5.581, 6.219, 6.379, 6.560, 6.698};

    // first and fourth argument : length of table
    car.setCalibTable(11, pwm_table0, speed_table0, 11, pwm_table1, speed_table1);
    
    while (1) {
        car.goStraightCalib(5);
        wait(5);
        car.stop();
        wait(5);
    }
}