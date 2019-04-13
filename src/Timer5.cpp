#include "../headers/Timer5.h"

LCD* lcd;
Button* btn;

/**
 * Construtor da classe.
 * Inicia os parâmetros do timer 5 e guarda a referencia da classe do LCD
 * @param initLCD endereço da classe que cuida do LCD
 * @param initBtn endereço da classe que cuida dos botões
 */
Timer5::Timer5(LCD* initLCD, Button* initBtn) {
	lcd = initLCD;
	btn = initBtn;

	cli(); // disable interrupts
    TCCR5A = 0;
    TCCR5B = 0;
    OCR5A = DELAY_20ms;  // Interromper a cada 20 ms
    TCCR5B = (1<<WGM52)|(1<<CS52);  // modo CTC e Prescaler=1/256
    TIMSK5 |= (1<<OCIE5A);  // Hab interrup por comparacao
    sei(); // enable interrupts
}

/**
 * Rotina de interrupção do timer 5.
 * Ocorre a cada 20 ms
 * @param TIMER5_COMPA_vect vetor de interrupções do timer 5
 */
ISR(TIMER5_COMPA_vect){
	// LCD
  	if (lcd->lcd_flag0 == true){
		lcd->loadLCD(0, 0, 19);
		lcd->lcd_flag0 = false;
	}

  	if (lcd->lcd_flag1 == true){
		lcd->loadLCD(1,0,19);
		lcd->lcd_flag1 = false;
	}

	// Botões
	btn->readButtons();
}
