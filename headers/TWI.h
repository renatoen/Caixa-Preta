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
#define MAX_CONT 1000 //colocar como variável

#define REPEAT_START 10

class TWI {
private:
	uint16_t cont, numRepeatStart;

	bool waitTask();

public:
	TWI(int freq);

	bool sendStart();

	bool sendRepeatedStart();

	void sendStop();

	bool sendReadAddress(byte address);

	bool sendWriteAddress(byte address);

	bool sendData(byte dado);

	uint8_t readData(bool ack);

};

#endif
