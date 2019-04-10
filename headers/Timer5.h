#ifndef TIMER5_H
#define TIMER5_H

// Includes
#include <Arduino.h>
#include "LCD.h"
#include "Button.h"

// Defines

#define  DELAY_20ms   1250  //gerar 20ms Timer 5

class Timer5 {
private:

public:
	Timer5(LCD* initLCD, Button* initBtn);

};

#endif
