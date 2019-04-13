#ifndef TWI_H
#define TWI_H

// Includes
#include <stdint.h>
#include <Arduino.h>

// Defines

//	Códigos de Status do TWI
#define START_OK      0x08  //	Start OK
#define START_REP_OK  0x10  //	Start Repetido OK
#define SLA_WR_ACK    0x18  //	EET enviado e ACK recebido
#define SLA_WR_NACK   0x20  //	EET enviado e NACK recebido
#define TX_DATA_ACK   0x28  //	Dado enviado e ACK recebido
#define TX_DATA_NACK  0x30  //	Dado transmitido e NACK recebido
#define SLA_RD_ACK    0x40  //	EER enviado e ACK recebido
#define SLA_RD_NACK   0x48  //	EER enviar e NACK recebido
#define RX_DATA_ACK   0x50  //	Dado recebido e ACK gerado
#define RX_DATA_NACK  0x58  //	Dado recebido e NACK gerado

//	valor máximo do contador de timer
#define MAX_CONT 1000

#define REPEAT_START 10 // valor máximo que o start deve ser reenviado em caso de falha

class TWI {
private:
	uint16_t cont;	/**< Contador para máximo de loops que o código deve esperar por resposta do MPU */
	uint16_t numRepeatStart; /**< Contador do número de vezes que o start foi reenviado em caso de falha */

	/**
	 * Espera TWINT = 1. Espera incrementando um contador e quando chega no máximo
	 * e TWINT ainda não é 1, retorna FALSE.
	 * @return TRUE se TWINT = 1 e FALSE caso contrário
	 */
	bool waitTask();

public:

	/**
	 * Construtor da classe. Define a frequência que vai ser trabalhada e o endereço
	 * do escravo.
	 * @param freq frequência utilizada
	 */
	TWI(int freq);

	/**
	 * Envia um sinal de start. Caso falhe, repete REPEAT_START vezes.
	 * @return TRUE se start ocorreu e FALSE caso contrário
	 */
	bool sendStart();

	/**
	 * Envia um sinal de start repetido. Caso falhe, repete REPEAT_START vezes.
	 * @return TRUE se start repetido ocorreu e FALSE caso contrário
	 */
	bool sendRepeatedStart();

	/**
	 * Envia um sinal de stop.
	 */
	void sendStop();

	/**
	 * Envia o endereço de leitura.
	 * @param address endereço de leitura
	 * @return TRUE se endereço foi enviado e FALSE caso contrário
	 */
	bool sendReadAddress(byte address);

	/**
	 * Envia o endereço de escrita.
	 * @param address endereço de escrita
	 * @return TRUE se endereço foi enviado e FALSE caso contrário
	 */
	bool sendWriteAddress(byte address);

	/**
	 * Envia um dado.
	 * @param dado dado que vai ser enviado
	 * @return TRUE se o dado foi enviado e FALSE caso contrário
	 */
	bool sendData(byte dado);

	/**
	 * Lê um dado.
	 * @param ack determina se quer receber ACK (TRUE) ou NACK (FALSE)
	 * @return dado lido
	 */
	uint8_t readData(bool ack);

};

#endif
