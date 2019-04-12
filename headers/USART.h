#ifndef UART_H
#define UART_H

//includes
#include <stdint.h>
#include <Arduino.h>

//defines

// Cálculo de Baud Rate para modo Assíncrono Double Speed
#define BAUD(x) (((F_CPU / (x * 8UL))) -1)

// Cálculo de Baud Rate para modo Assíncrono normal
// #define BAUD(x) (((F_CPU / (x * 16UL))) -1)

#define DEC 10
#define HEX 16

class USART {
private:
	// variáveis
	int serial;
	void sendByte(byte dado);
public:

	// métodos
	USART(int serialNumber, long baudRate);
	byte readByte();
	void print(char dado[]);
	void print(int dado);
	void print(float dado);
	void print(word dado, int modo = DEC);
	void print(byte dado, int modo = DEC);
	void println(char dado[]);
	void println(int dado);
	void println(float dado);
	void println(word dado, int modo = DEC);
	void println(byte dado, int modo = DEC);
};

#endif
