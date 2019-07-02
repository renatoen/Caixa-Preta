#ifndef SRAM_h
#define SRAM_h

#include <Arduino.h>
#include "SPI.h"
// #include <SPI.h>

/*---------------------- SRAM -----------------------*/
//Conjunto de instrucoes para a memoria 23LC1024
#define RDMR 5				// Ler registrador de modo
#define WRMR 1				// Escrever no registrador de modo
#define READ 3				// Ler dado da memoria
#define WRITE 2				// Escrever dado na memoria

// Registrador de modo: Modos de operação
#define MODO_BYTE 0x00 //Modo Byte
#define MODO_PAG 0x80 //Modo Página
#define MODO_SEQ 0x40 // Registrador de modo - Modo sequencial

#define CSPIN 48	// Pino de seleção do escravo

class SRAM {
private:
	SPI interfaceSPI; /**< Classe que cuida da interface SPI da SRAM */

public:
	/**
	 * Construtor default da classe.
	 */
	SRAM();

	/**
	 * Construtor que recebe qual o pino que o Arduino está usando para selecionar
	 * a SRAM
	 * @param port pino do arduino que conecta no SS da SRAM
	 */
	SRAM(int port);

	/**
	 * Lê o registrador de modo de operação da SRAM
	 * @return byte que representa o modo de operação atual
	 */
	uint8_t getMode();

	/**
	 * Define o modo de operação da SRAM escrevendo no registrador de modo
	 * @param byte que define o modo de operação que será utilizado
	 */
    void setMode(uint8_t mode);

	/**
	 * Escreve uma sequência de dados na SRAM a partir de um endereço
	 * @param address endereço que o dado será escrito
	 * @param data vetor de dados de 16 bits que será escrito
	 * @param num tamanho do vetor data
	 */
    void writeSeq(uint32_t address, int16_t *data, uint16_t num);

	/**
	 * Lê uma sequência de dados da SRAM a partir de um endereço
	 * @param address endereço em que o dado será lido
	 * @param data vetor de dados de 16 bits que receberá a informação
	 * @param num tamanho do vetor data
	 */
    void readSeq(uint32_t address, int16_t *data, uint32_t num);

};

#endif
