#include "mbed.h"
#include "TextLCD.h"

DigitalOut led(LED_RED);
TextLCD lcd(D2, D3, D4, D5, D6, D7);

int main()
{
      int x=30;
      lcd.printf("106000147\n");
      while(x > 0)
      {
            led = !led;             // toggle led
            lcd.locate(10,0);
            lcd.printf("%2i",x);    //conuter display
            wait(1);
            x--;
      }
}