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

	// variáveis
	volatile byte sw_buf[SW_BUF_TAM];   // buffer para o teclado
	volatile byte sw_v,sw_n,sw_1,sw_2;  // valor velho e valor novo da chave
	volatile byte sw_pin, sw_pout;      // ponteiros (indexadores para buffer)

	// métodos
	byte demuxCode();
	bool loadBuffer(char key);
	void startADC();
	void clearBuffer();
public:

	// métodos
	Button();
	void readButtons();
	bool readBuffer(char *key);
};

#endif
