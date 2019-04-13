#include "../headers/Leds.h"

/**
 * Construtor da classe.
 * Inicia as portas dos Leds como saída e os desliga.
 */
Leds::Leds() {
	DDRD |= (1<<LED_BIT_VD);    // Saída
    PORTD &=~(1<<LED_BIT_VD);   // Apagado

    DDRG |=  ((1<<LED_BIT_VM) | (1<<LED_BIT_AM));   // Saída
    PORTG &=  ~((1<<LED_BIT_VM) | (1<<LED_BIT_AM)); // Apagado
}

/**
 * Liga, desliga ou troca o estado do led verde
 * @param on ON para ligar, OFF para desligar e TOGGLE para trocar o estado
 */
void Leds::green(byte on) {
	if (on == ON) {
		PORTD |= (1<<LED_BIT_VD);   // acender Verde
	} else if (on == OFF) {
		PORTD &= ~(1<<LED_BIT_VD);  // apagar Verde
	} else if (on == TOGGLE) {
		PORTD ^= (1<<LED_BIT_VD);   // complementar Verde
	}
}

/**
 * Liga, desliga ou troca o estado do led amarelo
 * @param on ON para ligar, OFF para desligar e TOGGLE para trocar o estado
 */
void Leds::yellow(byte on) {
	if (on == ON) {
		PORTG |= (1<<LED_BIT_AM);   // acender Amarelo
	} else if (on == OFF) {
		PORTG &= ~(1<<LED_BIT_AM);  // apagar Amarelo
	} else if (on == TOGGLE) {
		PORTG ^= (1<<LED_BIT_AM);   // complementar Amarelo
	}
}

/**
 * Liga, desliga ou troca o estado do led vermelho
 * @param on ON para ligar, OFF para desligar e TOGGLE para trocar o estado
 */
void Leds::red(byte on) {
	if (on == ON) {
		PORTG |= (1<<LED_BIT_VM);   // acender Vermelho
	} else if (on == OFF) {
		PORTG &= ~(1<<LED_BIT_VM);  // apagar Vermelho
	} else if (on == TOGGLE) {
		PORTG ^= (1<<LED_BIT_VM);   // complementar Vermelho
	}
}

/**
 * Desliga todos os leds
 */
void Leds::turnOffAll() {
	red(OFF);
	yellow(OFF);
	green(OFF);
}

/**
 * Liga todos os leds
 */
void Leds::turnOnAll() {
	red(ON);
	yellow(ON);
	green(ON);
}
