#ifndef LEDS_H
#define LEDS_H

// Includes
#include <Arduino.h>

// Defines

#define ON 1
#define OFF 0
#define TOGGLE 2

// Led original --> PB7 (pino 13)
// Led amarelo --> PL0 (pino 49)
// Led verde --> PL1 (pino 48)
// Led vermelho --> PL2 (pino 47)

//#define LED_VM 47
//#define LED_VD 48
//#define LED_AM 49
//#define LED_OR 13

#define LED_BIT_VD 7    //Porta D
#define LED_BIT_VM 1    //Porta G
#define LED_BIT_AM 2    //Porta G

class Leds {
private:

public:
	Leds();
	void green(byte on);
	void yellow(byte on);
	void red(byte on);
	void turnOffAll();
	void turnOnAll();
};

#endif
