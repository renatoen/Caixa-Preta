#ifndef UART_H
#define UART_H

//includes
#include <stdint.h>
#include <Arduino.h>
#include <math.h>

//defines

// Cálculo de Baud Rate para modo Assíncrono Double Speed
#define BAUD(x) (((F_CPU / (x * 8UL))) -1)

// Cálculo de Baud Rate para modo Assíncrono normal
// #define BAUD(x) (((F_CPU / (x * 16UL))) -1)

#define CHAR 8
#define DEC 10
#define HEX 16

class USART {
private:

	int serial;	/**< Define qual porta serial a classe vai utilizar */

	/**
	 * Envia um byte pela porta serial.
	 * @param dado byte que vai ser enviado
	 */
	void sendByte(byte dado);
public:

	/**
	 * Construtor default da classe.
	 */
	USART();

	/**
	 * Construtor da classe. Inicia uma comunicação serial em uma das portas do arduino
	 * utilizando o baud rate informado
	 * @param serialNumber qual porta serial será utilizada.
	 * No mega pode ser 0, 1, 2 ou 3
	 * @param baudRate baud Rate da comunicação.
	 */
	USART(int serialNumber, long baudRate);

	/**
	 * Verifica se existe algo para ler na porta serial
	 * @return TRUE se existir, FALSE se não
	 */
	bool isAvailable();

	/**
	 * Lê um byte da porta serial.
	 * @return byte lido
	 */
	byte readByte();

	/**
	 * Imprime uma string
	 * @param dado dado que será impresso
	 */
	void print(char dado[]);

	/**
	 * Imprime em decimal com sinal um long, retirando os zeros à esquerda
	 * @param dado dado que será impresso
	 */
	void print(long dado);

	/**
	 * Imprime em decimal com sinal um int, retirando os zeros à esquerda
	 * @param dado dado que será impresso
	 */
	void print(int dado);

	/**
	 * Imprime em double xx.xxxxxx
	 * @param dado dado que será impresso
	 */
	void print(double dado);

	/**
	 * Se o modo for decimal, imprime em decimal sem sinal um W16. Se o modo for
	 * hexadecimal, imprime em hexa um palavra de 16 bits.
	 * Possui como padrão o modo decimal.
	 * @param dado dado que será impresso
	 * @param modo DEC para decimal e HEX para hexadecimal
	 */
	void print(word dado, int modo = DEC);

	/**
	 * Se o modo for decimal, imprime em decimal sem sinal um byte retirando os
	 * zeros à esquerda. Se o modo for hexadecimal, imprime em Hexa um byte.
	 * Possui como padrão o modo decimal.
	 * @param dado dado que será impresso
	 * @param modo DEC para decimal e HEX para hexadecimal
	 */
	void print(byte dado, int modo = DEC);

	/**
	 * Imprime uma string e pula uma linha
	 * @param dado dado que será impresso
	 */
	void println(char dado[]);

	/**
	 * Imprime em decimal com sinal um long e pula uma linha
	 * @param dado dado que será impresso
	 */
	void println(long dado);

	/**
	 * Imprime em decimal com sinal um int e pula uma linha
	 * @param dado dado que será impresso
	 */
	void println(int dado);

	/**
	 * Imprime em double xx.xxxxxx e pula uma linha
	 * @param dado dado que será impresso
	 */
	void println(double dado);

	/**
	 * Se o modo for decimal, imprime em decimal sem sinal um W16 e pula uma linha.
	 * Se o modo for hexadecimal, imprime em hexa um palavra de 16 bits e pula uma
	 * linha. Possui como padrão o modo decimal.
	 * @param dado dado que será impresso
	 * @param modo DEC para decimal e HEX para hexadecimal
	 */
	void println(word dado, int modo = DEC);

	/**
	 * Se o modo for decimal, imprime em decimal sem sinal um byte retirando os
	 * zeros à esquerda e pula uma linha. Se o modo for hexadecimal,
	 * imprime em Hexa um byte e pula uma linha. Possui como padrão o modo decimal.
	 * @param dado dado que será impresso
	 * @param modo DEC para decimal e HEX para hexadecimal
	 */
	void println(byte dado, int modo = DEC);
};

#endif
