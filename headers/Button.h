#ifndef BUTTON_H
#define BUTTON_H

//includes
#include <Arduino.h>

//defines

// Valores de 8 bit lidos pelo conversor AD0
#define  SW_VAL_DIR    122      //10 bits --> 491
#define  SW_VAL_CIMA    81      //10 bits --> 324
#define  SW_VAL_BAIXO  187      //10 bits --> 748
#define  SW_VAL_ESQ     34      //10 bits --> 135
#define  SW_VAL_SEL      0      //10 bits --> 0
#define  SW_VAL_NADA   255      //10 bits --> 1023

// Parametros para leitura das chaves
#define  SW_TOL     20  // tolerancia para identificar chave
#define  SW_BUF_TAM  4  // tamanho do buffer do teclado

// Codigos para as chaves
#define  SW_NADA   0
#define  SW_CIMA   1
#define  SW_ESQ    2
#define  SW_SEL    3
#define  SW_DIR    4
#define  SW_BAIXO  5
#define  SW_NAOSEI 99

class Button {
private:

	volatile byte sw_buf[SW_BUF_TAM];	/**< buffer para o teclado */
	volatile byte sw_v;		/**< valor velho da chave */
	volatile byte sw_n;		/**< valor novo da chave */
	volatile byte sw_1;		/**< valor velho da chave */
	volatile byte sw_2;		/**< valor novo da chave */
	volatile byte sw_pin;	/**< ponteiro indexador para o início do buffer */
	volatile byte sw_pout;	/**< ponteiro indexador para o final do buffer */

	/**
	 * Retorna o código da chave acionada
	 * @return código do botão que foi acionado
	 */
	byte demuxCode();

	/**
	 * Coloca uma chave no buffer
	 * @param key chave que foi acionada e vai ser colocada no buffer
	 * @return TRUE se sucesso e FALSE se a fila estiver cheia
	 */
	bool loadBuffer(char key);

	/**
	 * Inicialização do ADC que é usado na laeitura das chaves.
	 * Modo Free Running e leitura apenas do ADCH
	 */
	void startADC();

	/**
	 * Zera o buffer do teclado e inicializa os ponteiros
	 */
	void clearBuffer();
public:

	/**
	 * Construtor da classe.
	 * Inicializa os botões.
	 */
	Button();

	/**
	 * Lê as chaves e coloca no buffer. Monitora as chaves em 20 Hz
	 */
	void readButtons();

	/**
	 * Tira uma chave do buffer e retorna se houve sucesso
	 * @param key ponteiro para a variável que vai receber o valor da chave acionada
	 * @return TRUE se havia chave no buffer e FALSE se não havia chave no buffer
	 */
	bool readBuffer(char *key);
};

#endif
