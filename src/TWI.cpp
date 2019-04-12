#include "../headers/TWI.h"

/**
 * Init da classe. Define a frequência que vai ser trabalhada e o endereço
 * do escravo.
 * @param address endereço do escravo
 * @param freq frequência utilizada
 */
TWI::TWI(int freq) {

	this->cont = 0;
	this->numRepeatStart = 0;

	switch (freq) {
		case 55:
			// Programar MAX freq SCL = 55,55 kHz (T = 18 useg)
			// Só usar com relógio = 1 MHz
			TWBR = 1;  //SCL = 55,55 kHz
			TWSR = 0;  //TWPS1 = TWPS0 = 0 Pré-escalonador
			break;
		case 100:
			// Programar frequencia do SCL para 100k
			// Com relógio = 1 MHz --> 6,25 kHz
			TWBR = 72; //SCL = 100 kHz
			TWSR = 0;  //TWPS1 = TWPS0 = 0
			break;
		case 400:
			// Programar frequencia do SCL para 400k
			// Com relógio = 1 MHz --> 25 kHz
			TWBR = 12; //SCL = 400 kHz
			TWSR = 0;  //TWPS1 = TWPS0 = 0
			break;
		default:
			break;
	}

}

/**
 * Espera TWINT = 1. Espera incrementando um contador e quando chega no máximo
 * e TWINT ainda não é 1, retorna FALSE.
 * @return TRUE se TWINT = 1 e FALSE caso contrário
 */
bool TWI::waitTask() {

	while (!(TWCR & (1<<TWINT))){
		if (++this->cont == MAX_CONT) {
			//erro
			this->cont = 0;
			return false;
		}
	}

	this->cont = 0;

	return true;
}

/**
 * Envia um sinal de start. Caso falhe, repete REPEAT_START vezes.
 * @return TRUE se start ocorreu e FALSE caso contrário
 */
bool TWI::sendStart() {
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);

	if (waitTask()) {
		this->numRepeatStart = 0;

		if ((TWSR & 0xF8) == START_OK) {
			return true;
		} else {
			return false;
		}
	} else {
		// não enviou o start, repete o envio
		if (++this->numRepeatStart != REPEAT_START) {
			sendStart();
		} else {
			this->numRepeatStart = 0;
			return false;
		}
	}
}

/**
 * Envia um sinal de start repetido. Caso falhe, repete REPEAT_START vezes.
 * @return TRUE se start repetido ocorreu e FALSE caso contrário
 */
bool TWI::sendRepeatedStart() {
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);

	if (waitTask()) {
		this->numRepeatStart = 0;

		if ((TWSR & 0xF8) == START_REP_OK) {
			return true;
		} else {
			return false;
		}
	} else {
		// não enviou o start, repete o envio
		if (++this->numRepeatStart != REPEAT_START) {
			sendRepeatedStart();
		} else {
			this->numRepeatStart = 0;
			return false;
		}
	}
}

/**
 * Envia um sinal de stop.
 */
void TWI::sendStop() {
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
	delay(1); //(para CLK=1 MHz) atraso para que STOP seja percebido
}

/**
 * Envia o endereço de escrita.
 * @param address endereço de escrita
 * @return TRUE se endereço foi enviado e FALSE caso contrário
 */
bool TWI::sendWriteAddress(byte address) {
	TWDR = address;
	TWCR = (1<<TWINT) | (1<<TWEN);

	if (waitTask()) {
		if ((TWSR & 0xF8) == SLA_WR_ACK) {
			return true;
		} else {
			return false;
		}
	} else {
		//erro
		return false;
	}
}

/**
 * Envia o endereço de leitura.
 * @param address endereço de leitura
 * @return TRUE se endereço foi enviado e FALSE caso contrário
 */
bool TWI::sendReadAddress(byte address) {
	TWDR = address;
	TWCR = (1<<TWINT) | (1<<TWEN);

	if (waitTask()) {
		if ((TWSR & 0xF8) == SLA_RD_ACK) {
			return true;
		} else {
			return false;
		}
	} else {
		//erro
		return false;
	}
}

/**
 * Envia um dado.
 * @param dado dado que vai ser enviado
 * @return TRUE se o dado foi enviado e FALSE caso contrário
 */
bool TWI::sendData(uint8_t dado) {

	TWDR = dado;
	TWCR = (1<<TWINT) | (1<<TWEN);

	if (waitTask()) {
		if ((TWSR & 0xF8) == TX_DATA_ACK) {
			return true;
		} else {
			// erro: ack não recebido
			sendStop();
			return false;
		}
	} else {
		// erro: tarefa nunca concluida
		sendStop();
		return false;
	}
}

/**
 * Lê um dado.
 * @param ack determina se quer receber ACK (TRUE) ou NACK (FALSE)
 * @return dado lido
 */
uint8_t TWI::readData(bool ack) {
	byte status;

	if (ack == true) { // envia ack
		TWCR = (1<<TWINT) | (1<<TWEA) | (1<<TWEN);  // Iniciar recepção (TWEA=1 --> ACK)
		status = RX_DATA_ACK;
	} else if (ack == false) { // envia nack
		TWCR = (1<<TWINT) | (1<<TWEN);      // Iniciar recepção (TWEA=0 --> NACK)
		status = RX_DATA_NACK;
	}

	if (waitTask()) {
		if ((TWSR & 0xF8) == status) {
			return TWDR;
		} else {
			//erro: info nunca recebida
			sendStop();
			return 0;
		}
	} else {
		//erro: tarefa nunca concluida
		sendStop();

		return 0;
	}
}
