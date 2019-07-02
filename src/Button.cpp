#include "../headers/Button.h"

/**
 * Construtor da classe.
 * Inicializa os botões.
 */
Button::Button() {
	this->startADC();
	this->clearBuffer();
	this->sw_1 = this->sw_2 = this->sw_n = this->sw_v = SW_NADA;
}

/**
 * Lê as chaves e coloca no buffer. Monitora as chaves em 20 Hz
 */
void Button::readButtons() {
  	sw_2 = this->demuxCode();
  	if (sw_2 != SW_NAOSEI) {
    	if (sw_1 == sw_2) {
      		sw_n = sw_2;
      		if (sw_v != sw_n) {
        		if (sw_v == SW_NADA) {
					sw_v = sw_n;
					this->loadBuffer(sw_n);
				}else {
					sw_v = sw_n;
				}
      		}
    	}else {
			sw_1 = sw_2;
		}
	}
}

/**
 * Retorna o código da chave acionada
 * @return código do botão que foi acionado
 */
byte Button::demuxCode() {
  	byte valor_ADC0 = ADCH;  //Receber resultado do ADC

  	if (abs(valor_ADC0-SW_VAL_NADA)  < SW_TOL)
		return(SW_NADA);
  	if (abs(valor_ADC0-SW_VAL_CIMA)  < SW_TOL)
		return(SW_CIMA);
  	if (abs(valor_ADC0-SW_VAL_ESQ)   < SW_TOL)
		return(SW_ESQ);
  	if (abs(valor_ADC0-SW_VAL_SEL)   < SW_TOL)
		return(SW_SEL);
  	if (abs(valor_ADC0-SW_VAL_DIR)   < SW_TOL)
		return(SW_DIR);
  	if (abs(valor_ADC0-SW_VAL_BAIXO) < SW_TOL)
		return(SW_BAIXO);

  	return(SW_NAOSEI);
}

/**
 * Coloca uma chave no buffer
 * @param key chave que foi acionada e vai ser colocada no buffer
 * @return TRUE se sucesso e FALSE se a fila estiver cheia
 */
bool Button::loadBuffer(char key) {
  	if (sw_pin == sw_pout) {
  		return false;    // fila cheia
  	}else {
		sw_buf[sw_pin++] = key;
		if (sw_pin == SW_BUF_TAM) {
			sw_pin=0;
		}
		return true;
	}
}

/**
 * Tira uma chave do buffer e retorna se houve sucesso
 * @param key ponteiro para a variável que vai receber o valor da chave acionada
 * @return TRUE se havia chave no buffer e FALSE se não havia chave no buffer
 */
bool Button::readBuffer(char *key) {
  	int sw_pout_aux = sw_pout+1;

	if (sw_pout_aux == SW_BUF_TAM) {
		sw_pout_aux=0;
	}

  	if (sw_pout_aux == sw_pin) {
		return false;
  	}else {
		*key = sw_buf[sw_pout_aux];
		sw_pout = sw_pout_aux;
		return true;
	}
}

/**
 * Zera o buffer do teclado e inicializa os ponteiros
 */
void Button::clearBuffer() {
  	int i;
  	for(i = 0; i < SW_BUF_TAM; i++) {
		sw_buf[i]=SW_NADA;
  	}
  	sw_pin = 1;
  	sw_pout = 0;
}

/**
 * Inicialização do ADC que é usado na leitura das chaves.
 * Modo Free Running e leitura apenas do ADCH
 */
void Button::startADC(){
  ADMUX=(1<<REFS0) | (1<<ADLAR);  //Justificar pela esquerda
  ADCSRA=(1<<ADEN) | (1<<ADATE) | (1<<ADPS2);
  ADCSRB=0;
  ADCSRA=(1<<ADEN) | (1<<ADSC) | (1<<ADATE) | (1<<ADPS2);
}
