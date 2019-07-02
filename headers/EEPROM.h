#ifndef EEPROM_H
#define EEPROM_H

#include <Arduino.h>
#include <stdint.h>
#include "TWI.h"

// defines
#define SIZE_EEPROM 131072 // 0x00 até 0x1FFFF
#define READ 1
#define WRITE 0

#define NUM_BLOCK(addr) (((addr) & 0x10000) >> 16) /**< Determina de qual bloco o endereço pertence utilizando um endereço de 17 bits */

#define NORMALIZE(addr) ((uint16_t)(addr & 0xffff)) /**< Normaliza os endereços que possuem 17 bits para 16 bits */

#define FULL_ADDR(block, addr) (((uint32_t) (((uint32_t)block) << 16)) | ((uint32_t) (addr))) /**< Cria o endereço de 17 bits utilizando o endereço de 16 bits e o número do bloco */

class EEPROM {
private:
	uint8_t deviceAddress; /**< Endereço do dispositivo para a comunicação TWI */
	uint32_t position; /*<< Ponteiro para a posição do endereço da memória da EEPROM. Vai de 0 até 0x1FFFF, necessitando 17 bits de precisão */
	uint32_t eepromPosition; /**< Contador de posição interno da EEPROM */
	uint8_t frequencia; /**< Frequencia da comunicação TWI */

	/**
	 * Lê dados da memória. Lê no máximo 2 páginas (256 bytes). Trata leituras que
	 * cruzam o bloco da memória.
	 * @param address endereço de 17 bits em que vai ser lido o dado
	 * @param data vetor de 16 bits com os dados lidos
	 * @param numBytes número de bytes que vai ser lido
	 * @return número de bytes que foram lidos
	 */
	uint32_t readData(uint32_t address, uint8_t *data, uint32_t numBytes);

	/**
	 * Escreve um dado em um endereço da EEPROM. O dado não pode passar de 1 página
	 * (128 bytes) e não pode transitar entre blocos.
	 * @param address endereço de 17 bits em que vai ser escrito o dado
	 * @param data vetor de 8 bits com os dados lidos
	 * @param numBytes número de bytes que vai ser lido
	 * @return número de bytes que foram escritos
	 */
	uint32_t writeOnSinglePageOnSingleBlock(uint32_t address, uint8_t *data, uint32_t numBytes);

	/**
	 * Escreve um dado em um endereço da EEPROM. O dado não pode passar de 1 página
	 * (128 bytes).
	 * @param address endereço de 17 bits em que vai ser escrito o dado
	 * @param data vetor de 8 bits com os dados lidos
	 * @param numBytes número de bytes que vai ser lido
	 * @return número de bytes que foram escritos
	 */
	uint32_t writeData(uint32_t address, uint8_t *data, uint32_t numBytes);
public:

	/**
	 * Construtor padrão da classe
	 */
	EEPROM();

	/**
	 * Construtor da classe. Recebe a frequência da comunicação TWI e os bytes A0 e
	 * A1 do seletor do barramento da EEPROM. Inicializa contadores da classe da
	 * posição da EEPROM. Inicializa também o endereço do chip
	 * @param frequencia frequencia do TWI
	 * @param byteA1 MSB do chip selector do barramento da eeprom
	 * @param byteA0 LSB do chip selector do barramento da eeprom
	 */
	EEPROM(uint8_t frequencia, bool byteA1, bool byteA0);

	/**
	 * Lê um byte da memória utilizando como endereço o local atual na memória,
	 * definido pelo contador interno da classe
	 * @return valor do byte lido
	 */
	uint8_t readByte();

	/**
	 * Lê dados da memória utilizando como endereço o local atual na memória,
	 * definido pelo contador interno da classe
	 * @param data valor que foi lido
	 * @param numBytes número de bytes que serão lidos
	 * @return número de bytes efetivamente lidos
	 */
	uint32_t read(const void *data, uint32_t numBytes);

	/**
	 * Lê dados da memória. Se a leitura passar por mais de 2 páginas, divide as
	 * leituras.
	 * @param address endereço de 17 bits em que vai ser escrito o dado
	 * @param data valor que foi lido
	 * @param numBytes número de bytes que serão lidos
	 * @return número de bytes efetivamente lidos
	 */
	uint32_t read(uint32_t address, const void *data, uint32_t numBytes);

	/**
	 * Escreve 1 byte na memória utilizando como endereço o local atual na memória,
	 * definido pelo contador interno da classe
	 * @param data byte que vai ser escrito
	 * @return TRUE se ocorreu tudo bem, FALSE caso contrário
	 */
	bool writeByte(uint8_t data);

	/**
	 * Escreve dados na memória utilizando como endereço o local atual na memória,
	 * definido pelo contador interno da classe
	 * @param data valor que vai ser escrito
	 * @param numBytes número de bytes que serão escritos
	 * @return número de bytes efetivamente escritos
	 */
	uint32_t write(const void *data, uint32_t numBytes);

	/**
	 * Escreve dados na memória. Se a escrita passar por mais de 1 página (128 bytes),
	 * divide as escritas.
	 * @param address endereço de 17 bits em que vai ser escrito o dado
	 * @param data valor que foi lido
	 * @param numBytes número de bytes que serão lidos
	 * @return número de bytes efetivamente lidos
	 */
	uint32_t write(uint32_t address, const void *data, uint32_t numBytes);

	/**
	 * Lê um float da memória
	 * @return valor do float ou NAN se inválido
	 */
	float readFloat();

	/**
	 * Escreve um float na memória
	 * @param data float que vai ser escrito
	 * @return TRUE se operação ocorreu com sucesso, FALSE caso contrário
	 */
	bool writeFloat(float data);

	/**
	 * Lê um unsigned int da memória
	 * @return valor do unsigned int
	 */
	uint32_t readUInt();

	/**
	 * Escreve um unsigned int na memória
	 * @param data unsigned int que vai ser escrito
	 * @return TRUE se operação ocorreu com sucesso, FALSE caso contrário
	 */
	bool writeUInt(uint32_t data);

	/**
	 * Lê um inteiro da memória
	 * @return valor do inteiro
	 */
	int32_t readInt();

	/**
	 * Escreve um inteiro na memória
	 * @param data inteiro que vai ser escrito
	 * @return TRUE se operação ocorreu com sucesso, FALSE caso contrário
	 */
	bool writeInt(int32_t data);

	/**
	 * Pega a posição/endereço que a classe está utilizando para fazer as operações
	 * @return posição/endereço atual
	 */
	uint32_t getPosition();

	/**
	 * Define a posição/endereço que a classe vai utilizar para fazer as operações
	 * @param position nova posição/endereço
	 * @return TRUE se operação ocorreu com sucesso, FALSE caso contrário
	 */
	bool setPosition(uint32_t position);
};

#endif
