#include "../headers/SRAM.h"

/**
 * Construtor default da classe
 */
SRAM::SRAM() {

}

/**
 * Construtor que recebe qual o pino que o Arduino está usando para selecionar
 * a SRAM
 * @param port pino do arduino que conecta no SS da SRAM
 */
SRAM::SRAM(int port) {
	this->interfaceSPI = SPI(port);
}

/**
 * Lê o registrador de modo de operação da SRAM
 * @return byte que representa o modo de operação atual
 */
uint8_t SRAM::getMode() {
    uint8_t mode;

	this->interfaceSPI.start();

	this->interfaceSPI.transferir(RDMR);
    mode = this->interfaceSPI.transferir(0x00);

	this->interfaceSPI.stop();

    return mode;
}

/**
 * Define o modo de operação da SRAM escrevendo no registrador de modo
 * @param byte que define o modo de operação que será utilizado
 */
void SRAM::setMode(uint8_t mode) {

	this->interfaceSPI.start();

	this->interfaceSPI.transferir(WRMR); // Indicar escrita no Reg de modo
	this->interfaceSPI.transferir(mode); // Escrever o modo

	this->interfaceSPI.stop();
}

/**
 * Escreve uma sequência de dados na SRAM a partir de um endereço
 * @param address endereço que o dado será escrito
 * @param data vetor de dados de 16 bits que será escrito
 * @param num tamanho do vetor data
 */
void SRAM::writeSeq(uint32_t address, int16_t *data, uint16_t num) {
    setMode(MODO_SEQ);

	this->interfaceSPI.start();

	this->interfaceSPI.transferir(WRITE);

	this->interfaceSPI.transferir((uint8_t)(address >> 16));
	this->interfaceSPI.transferir((uint8_t)(address >> 8));
	this->interfaceSPI.transferir((uint8_t) address);

    for (uint16_t i = 0; i < num; i++) {
		this->interfaceSPI.transferir((uint8_t)(data[i] >> 8));
		this->interfaceSPI.transferir((uint8_t)data[i]);
    }

    this->interfaceSPI.stop();
}

/**
 * Lê uma sequência de dados da SRAM a partir de um endereço
 * @param address endereço em que o dado será lido
 * @param data vetor de dados de 16 bits que receberá a informação
 * @param num tamanho do vetor data
 */
void SRAM::readSeq(uint32_t address, int16_t *data, uint32_t num) {
    setMode(MODO_SEQ);

	this->interfaceSPI.start();

	this->interfaceSPI.transferir(READ);

	this->interfaceSPI.transferir((uint8_t)(address >> 16));
	this->interfaceSPI.transferir((uint8_t)(address >> 8));
	this->interfaceSPI.transferir((uint8_t) address);

    for (uint16_t i = 0; i < num; i++) {
        data[i] = (int16_t)(this->interfaceSPI.transferir(0x00) << 8) | this->interfaceSPI.transferir(0x00);
        // reads two bytes sequentially to form a signed 16bit number
    }

	this->interfaceSPI.stop();
}
