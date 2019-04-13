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

	/**
	 * Construtor da classe.
	 * Inicia as portas dos Leds como saída e os desliga.
	 */
	Leds();

	/**
	 * Liga, desliga ou troca o estado do led verde
	 * @param on ON para ligar, OFF para desligar e TOGGLE para trocar o estado
	 */
	void green(byte on);

	/**
	 * Liga, desliga ou troca o estado do led amarelo
	 * @param on ON para ligar, OFF para desligar e TOGGLE para trocar o estado
	 */
	void yellow(byte on);

	/**
	 * Liga, desliga ou troca o estado do led vermelho
	 * @param on ON para ligar, OFF para desligar e TOGGLE para trocar o estado
	 */
	void red(byte on);

	/**
	 * Desliga todos os leds
	 */
	void turnOffAll();

	/**
	 * Liga todos os leds
	 */
	void turnOnAll();
};

#endif
