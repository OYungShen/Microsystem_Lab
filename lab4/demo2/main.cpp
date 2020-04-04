// uLCD-144-G2 basic text demo program for uLCD-4GL LCD driver library
//
#include "mbed.h"
#include "uLCD_4DGL.h"
#include <stdlib.h>

uLCD_4DGL uLCD(D1, D0, D2); // serial tx, serial rx, reset pin;

int main()
{
    int color;
    // basic printf demo = 16 by 18 characters on screen
    uLCD.printf("\n106000147\n"); //Default Green on black text
    uLCD.text_width(4); //4X size text
    uLCD.text_height(4);
    uLCD.color(RED);
    for (int i=30; i>=0; i--) {
        color = rand()%16777216;
        uLCD.color(color);
        uLCD.background_color(color);
        uLCD.locate(1,2);
        uLCD.printf("%2D",i);
        wait(1);
    }
}