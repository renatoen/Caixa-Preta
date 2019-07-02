#include "../headers/EEPROM.h"

/**
 * Construtor padrão da classe
 */
EEPROM::EEPROM() {

}

/**
 * Construtor da classe. Recebe a frequência da comunicação TWI e os bytes A0 e
 * A1 do seletor do barramento da EEPROM. Inicializa contadores da classe da
 * posição da EEPROM. Inicializa também o endereço do chip
 * @param frequencia frequencia do TWI
 * @param byteA1 MSB do chip selector do barramento da eeprom
 * @param byteA0 LSB do chip selector do barramento da eeprom
 */
EEPROM::EEPROM(uint8_t frequencia, bool byteA1, bool byteA0) {
	this->frequencia = frequencia;
	this->position = 0;
	this->eepromPosition = 0xFFFFFFFF;
	this->deviceAddress = 0xA0 | (byteA1 << 2) | (byteA0 << 1); // 1010 controle, bloco 0, escrita
}

/**
 * Lê dados da memória. Lê no máximo 2 páginas (256 bytes). Trata leituras que
 * cruzam o bloco da memória.
 * @param address endereço de 17 bits em que vai ser lido o dado
 * @param data vetor de 8 bits com os dados lidos
 * @param numBytes número de bytes que vai ser lido
 * @return número de bytes que foram lidos
 */
uint32_t EEPROM::readData(uint32_t address, uint8_t *data, uint32_t numBytes) {

	TWI leitura(this->frequencia);
	uint16_t cont = 0;

	if (numBytes == 0 || address >= SIZE_EEPROM) {
		// leitura inválida
		return 0;
	} else if (address + numBytes > SIZE_EEPROM) {
		// só lê até o limite da memória
		numBytes = SIZE_EEPROM - address;
	}

	// endereço do primeiro bloco mas leitura vai até o segundo bloco
  	if (address < 65536 && address + numBytes > 65536) {

    	// Lê o primeiro bloco
		if (leitura.sendStart()) {
			if (leitura.sendWriteAddress(this->deviceAddress)) {
				if (leitura.sendData((uint8_t)(NORMALIZE(address) >> 8))) {
					if (leitura.sendData((uint8_t) NORMALIZE(address))) {
						if (leitura.sendRepeatedStart()) {
							if (leitura.sendReadAddress(this->deviceAddress | READ)) {
								bool status;
								uint8_t dataAux = 0;

								for (uint16_t i = 0; i < (65536 - address); i++) {

									status = leitura.readData(true, &dataAux);

									if (status) { // leitura certa
										data[cont] = dataAux;
										cont++;
									} else {
										this->eepromPosition = 0xFFFFFFFF;
										return 0;
									}
								}
								// tudo ocorreu como esperado
								leitura.sendStop();
							}
						}
					}
				}
			}
		}

		// Lê o segundo bloco
		if (leitura.sendStart()) {
			if (leitura.sendWriteAddress(this->deviceAddress | (1 << 3))) {
				if (leitura.sendData(0)) {
					if (leitura.sendData(0)) {
						if (leitura.sendRepeatedStart()) {
							if (leitura.sendReadAddress(this->deviceAddress | (1 << 3) | READ)) {

								bool status;
								uint8_t dataAux = 0;

								for (uint16_t i = 0; i < (numBytes - (65536 - address)); i++) {

									if (i + 1 < (numBytes - (65536 - address))) {
										status = leitura.readData(true, &dataAux);
									} else {
										// ultimo dado manda um NACK
										status = leitura.readData(false, &dataAux);
									}

									if (status) { // leitura certa
										data[cont] = dataAux;
										cont++;
									} else {
										this->eepromPosition = 0xFFFFFFFF;
										this->position += (65536 - address);
										if (this->position > SIZE_EEPROM) {
											this->position %= SIZE_EEPROM;
										}

										return (uint32_t) (65536 - address);
									}
								}

								// ocorreu tudo como esperado
								leitura.sendStop();
								this->eepromPosition = FULL_ADDR(1, (numBytes - (65536 - address)));
								this->position += numBytes;

								if (this->position > SIZE_EEPROM) {
									this->position %= SIZE_EEPROM;
								}

								return numBytes;
							}
						}
					}
				}
			}
		}
  	} else {
    	// leitura não precisa acontecer entre 2 blocos
    	uint8_t bloco = NUM_BLOCK(address);

		if (leitura.sendStart()) {
			if (leitura.sendWriteAddress(this->deviceAddress | (bloco << 3))) {
				if (leitura.sendData((uint8_t)(NORMALIZE(address) >> 8))) {
					if (leitura.sendData((uint8_t) NORMALIZE(address))) {
						if (leitura.sendRepeatedStart()) {
							if (leitura.sendReadAddress(this->deviceAddress | (bloco << 3) | READ)) {

								bool status;
								uint8_t dataAux = 0;

								for (uint16_t i = 0; i < numBytes; i++) {

									if (i + 1 < numBytes) {
										status = leitura.readData(true, &dataAux);
									} else {
										// ultimo dado manda um NACK
										status = leitura.readData(false, &dataAux);
									}

									if (status) { // leitura certa
										data[cont] = dataAux;
										cont++;
									} else {
										this->eepromPosition = 0xFFFFFFFF;
										return 0;
									}
								}

								// ocorreu tudo como esperado
								leitura.sendStop();
								this->eepromPosition += numBytes;
								this->position += numBytes;
								if (this->position > SIZE_EEPROM) {
									this->position %= SIZE_EEPROM;
								}

								return numBytes;
							}
						}
					}
				}
			}
		}
  	}
}

/**
 * Escreve um dado em um endereço da EEPROM. O dado não pode passar de 1 página
 * (128 bytes) e não pode transitar entre blocos.
 * @param address endereço de 17 bits em que vai ser escrito o dado
 * @param data vetor de 8 bits com os dados lidos
 * @param numBytes número de bytes que vai ser lido
 * @return número de bytes que foram escritos
 */
uint32_t EEPROM::writeOnSinglePageOnSingleBlock(uint32_t address, uint8_t *data, uint32_t numBytes) {

	TWI leitura(this->frequencia);
	uint8_t bloco = NUM_BLOCK(address);
	uint16_t cont = 0;

  	if (numBytes == 0 | numBytes > 128) {
		// O chip só aceita escritas de páginas de até 128 bytes.
    	return 0;
	}

	if (leitura.sendStart()) {
		if (leitura.sendWriteAddress(this->deviceAddress | (bloco << 3))) {
			if (leitura.sendData((uint8_t)(NORMALIZE(address) >> 8))) {
				if (leitura.sendData((uint8_t)NORMALIZE(address))) {
					bool status;

					for (uint16_t i = 0; i < numBytes; i++) {

						status = leitura.sendData(data[i]);

						if (status) {
							// escrita certa
						} else {
							this->eepromPosition = 0xFFFFFFFF;
							return 0;
						}
					}

					leitura.sendStop();
					this->eepromPosition += numBytes;
					this->position += numBytes;
					if (this->position > SIZE_EEPROM) {
						this->position %= SIZE_EEPROM;
					}
				}
			}
		}
	}

	/**
	* De acordo com o datasheet do 24LC1025, após escrever é necessário utilizar
	* uma técnica de acknowledge polling para verificar se a escrita ocorreu de
	* forma correta. É enviado um START e o control byte com R/W = 0. Se o chip
	* ainda estiver ocupado, não haverá resposta. Isso deve ser repetido até o
	* chip responder com um ACK.
	*/
	uint32_t start = millis();
	uint32_t stop;
	if (leitura.sendStart()) {
		while (!leitura.sendWriteAddress(this->deviceAddress | (bloco << 3))) {
			stop = millis();
			if (stop - start > 1000) {
				// está esperando o chip dar ACK a pelo menos 1s, ocorreu algum erro
				return 0;
			}
			delay(1);
		}
	}

	return numBytes;
}

/**
 * Escreve um dado em um endereço da EEPROM. O dado não pode passar de 1 página
 * (128 bytes).
 * @param address endereço de 17 bits em que vai ser escrito o dado
 * @param data vetor de 8 bits com os dados lidos
 * @param numBytes número de bytes que vai ser lido
 * @return número de bytes que foram escritos
 */
uint32_t EEPROM::writeData(uint32_t address, uint8_t *data, uint32_t numBytes) {

  	if (numBytes == 0 || numBytes > 128 || address >= SIZE_EEPROM) {
	 	/** Só faz escritas de até 128 bytes, dessa forma a função sabe que no
		* pior caso, a escrita irá passar por 2 páginas. */
		return 0;
	}

  	if (address + numBytes > SIZE_EEPROM) {
		// se os bytes a serem lidos ultrapassarem o limite da eeprom, corta os bytes até o limite
    	numBytes = SIZE_EEPROM - address;
	}

  	uint32_t pageAddress = NORMALIZE(address); // endereço independente do bloco
  	uint8_t bloco1 = NUM_BLOCK(address); // bloco do endereço
  	uint8_t bloco2 = NUM_BLOCK(address + numBytes - 1); // verifica se com o número de bytes a ser lido se ultrapassa o limite do bloco

  	// pega a página que o endereço está
  	uint16_t page1 = pageAddress / 128;
	// verifica se com o número de bytes a ser lido se ultrapassa a página de 128 bytes
  	uint16_t page2 = (NORMALIZE(pageAddress + numBytes - 1))/128;

  	if (bloco1 == bloco2 && page1 == page2) {
    	// não ultrapassa bloco nem página, faz uma operação simples
    	return writeOnSinglePageOnSingleBlock(address, data, numBytes);
  	} else {
		/** Como a escrita ultrapassa o bloco ou a página, a escrita deve ser
		* divida de forma manual com necessáriamente 2 operações */
    	uint8_t bytesPrimeiraParte = ((page1 + 1) * 128) - pageAddress;
    	uint8_t bytesSegundaParte = numBytes - bytesPrimeiraParte;

    	uint8_t bytesEnviados = writeOnSinglePageOnSingleBlock(FULL_ADDR(bloco1, pageAddress), data, bytesPrimeiraParte);

    	if (bytesEnviados != bytesPrimeiraParte) {
			// não enviou todos os bytes que deveria
      		return (uint32_t) bytesEnviados;
    	}

		bytesEnviados = writeOnSinglePageOnSingleBlock(FULL_ADDR(bloco2, page2 * 128), (data + bytesPrimeiraParte), bytesSegundaParte);
    	if (bytesEnviados != bytesSegundaParte) {
			// não enviou todos os bytes que deveria
      		return (uint32_t) (bytesPrimeiraParte + bytesEnviados);
    	}
  	}

  	return numBytes;
}

/**
 * Lê um byte da memória utilizando como endereço o local atual na memória,
 * definido pelo contador interno da classe
 * @return valor do byte lido
 */
uint8_t EEPROM::readByte() {

	uint8_t dado[1];
	uint32_t bytesEscritos = readData(this->position, dado, 1);

	return (uint8_t)dado[0];
}

/**
 * Lê dados da memória utilizando como endereço o local atual na memória,
 * definido pelo contador interno da classe
 * @param data valor que foi lido
 * @param numBytes número de bytes que serão lidos
 * @return número de bytes efetivamente lidos
 */
uint32_t EEPROM::read(const void *data, uint32_t numBytes) {
  	return read(this->position, data, numBytes);
}

/**
 * Lê dados da memória. Se a leitura passar por mais de 2 páginas, divide as
 * leituras.
 * @param address endereço de 17 bits em que vai ser escrito o dado
 * @param data valor que foi lido
 * @param numBytes número de bytes que serão lidos
 * @return número de bytes efetivamente lidos
 */
uint32_t EEPROM::read(uint32_t address, const void *data, uint32_t numBytes) {
  	if (numBytes == 0) {
    	return 0;
	}

  	if (numBytes < 256) {
		// vai passar por no máximo 2 páginas (2 * 128 = 256)
    	return readData(address, data, numBytes);
	}

  	if (address + numBytes >= SIZE_EEPROM) {
		// se os bytes a serem lidos ultrapassarem o limite da eeprom, corta os bytes até o limite
    	numBytes = SIZE_EEPROM - address;
	}

	// Passa por mais de 2 páginas, então divide entre várias leituras menores que 255 bytes

	// Tamanho da página
  	const uint32_t pageSize = 255;

  	uint32_t bytesLidos = 0;
  	uint32_t aux = 0;

  	while (bytesLidos < numBytes) {
    	aux = readData(address + bytesLidos, ((uint8_t *)data + bytesLidos), min(pageSize, numBytes - bytesLidos));
    	if (aux == min(pageSize, numBytes - bytesLidos)) {
      		bytesLidos += aux;
    	} else {
      		return bytesLidos;
		}
  	}

  	return numBytes;
}

/**
 * Escreve 1 byte na memória utilizando como endereço o local atual na memória,
 * definido pelo contador interno da classe
 * @param data byte que vai ser escrito
 * @return TRUE se ocorreu tudo bem, FALSE caso contrário
 */
bool EEPROM::writeByte(uint8_t data) {
	/**
	 * Escrever um byte utiliza a escrita da página inteira, contando como uma
	 * escrita do ciclo de vida da EEPROM. Evitar fazer várias escritas seguidas
	 * de 1 byte e fazer uma escrita só.
	 */

	uint32_t bytesEscritos = writeData(this->position, data, 1);

	if (bytesEscritos == 1) {
		return true;
	} else {
		return false;
	}
}

/**
 * Escreve dados na memória utilizando como endereço o local atual na memória,
 * definido pelo contador interno da classe
 * @param data valor que vai ser escrito
 * @param numBytes número de bytes que serão escritos
 * @return número de bytes efetivamente escritos
 */
uint32_t EEPROM::write(const void *data, uint32_t numBytes) {
  	return write(this->position, data, numBytes);
}

/**
 * Escreve dados na memória. Se a escrita passar por mais de 1 página (128 bytes),
 * divide as escritas.
 * @param address endereço de 17 bits em que vai ser escrito o dado
 * @param data valor que foi lido
 * @param numBytes número de bytes que serão lidos
 * @return número de bytes efetivamente lidos
 */
uint32_t EEPROM::write(uint32_t address, const void *data, uint32_t numBytes) {

  	if (numBytes == 0) {
    	return 0;
	}

  	if (numBytes <= 128) {
		// vai escrever até no máximo 128 bytes
    	return writeData(address, data, numBytes);
	}

  	if (address + numBytes >= SIZE_EEPROM) {
		// se os bytes a serem escritos ultrapassarem o limite da eeprom, corta os bytes até o limite
    	numBytes = SIZE_EEPROM - address;
	}

  	uint32_t bytesEscritos = 0;
  	uint32_t aux = 0;

  	while (bytesEscritos < numBytes) {
    	aux = writeData(address + bytesEscritos, ((uint8_t *)data + bytesEscritos), min(128, numBytes - bytesEscritos));
    	if (aux == min(128, numBytes - bytesEscritos)) {
      		bytesEscritos += aux;
    	} else {
      		return (uint32_t) bytesEscritos;
		}
  	}

  	return numBytes;
}

/**
 * Lê um float da memória
 * @return valor do float ou NAN se inválido
 */
float EEPROM::readFloat() {
  	float data;
  	if (read(this->position, (const void*)&data, sizeof(float)) == sizeof(float)) {
    	return data;
  	} else {
    	return NAN;
	}
}

/**
 * Escreve um float na memória
 * @param data float que vai ser escrito
 * @return TRUE se operação ocorreu com sucesso, FALSE caso contrário
 */
bool EEPROM::writeFloat(float data) {
  	if (write(this->position, (const void*)&data, sizeof(float)) == sizeof(float)) {
    	return true;
  	} else {
    	return false;
	}
}

/**
 * Lê um unsigned int da memória
 * @return valor do unsigned int
 */
uint32_t EEPROM::readUInt() {
  	uint32_t data;
  	if (read(this->position, (const void*)&data, sizeof(uint32_t)) == sizeof(uint32_t)) {
    	return data;
  	} else {
    	return 0;
	}
}

/**
 * Escreve um unsigned int na memória
 * @param data unsigned int que vai ser escrito
 * @return TRUE se operação ocorreu com sucesso, FALSE caso contrário
 */
bool EEPROM::writeUInt(uint32_t data) {
  	if (write(this->position, (const void*)&data, sizeof(uint32_t)) == sizeof(uint32_t)) {
    	return true;
  	} else {
    	return false;
	}
}

/**
 * Lê um inteiro da memória
 * @return valor do inteiro
 */
int32_t EEPROM::readInt() {
  	int32_t data;
  	if (read(this->position, (const void*)&data, sizeof(int32_t)) == sizeof(int32_t)) {
    	return data;
  	} else {
    	return 0;
	}
}

/**
 * Escreve um inteiro na memória
 * @param data inteiro que vai ser escrito
 * @return TRUE se operação ocorreu com sucesso, FALSE caso contrário
 */
bool EEPROM::writeInt(int32_t data) {
  	if (write(this->position, (const void*)&data, sizeof(int32_t)) == sizeof(int32_t)) {
    	return true;
  	} else {
    	return false;
	}
}

/**
 * Pega a posição/endereço que a classe está utilizando para fazer as operações
 * @return posição/endereço atual
 */
uint32_t EEPROM::getPosition() {
	return this->position;
}

/**
 * Define a posição/endereço que a classe vai utilizar para fazer as operações
 * @param position nova posição/endereço
 * @return TRUE se operação ocorreu com sucesso, FALSE caso contrário
 */
bool EEPROM::setPosition(uint32_t position) {
	if (position < SIZE_EEPROM) {
		this->position = position;
	    return true;
  	} else {
	 	return false;
  	}
}
