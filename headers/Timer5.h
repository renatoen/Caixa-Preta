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
	/**
	 * Construtor da classe.
	 * Inicia os parâmetros do timer 5 e guarda a referencia da classe do LCD
	 * @param initLCD endereço da classe que cuida do LCD
	 * @param initBtn endereço da classe que cuida dos botões
	 */
	Timer5(LCD* initLCD, Button* initBtn);

};

#endif
