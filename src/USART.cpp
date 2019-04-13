#include "../headers/USART.h"

USART::USART() {

}

/**
 * Init da classe. Inicia uma comunicação serial em uma das portas do arduino
 * utilizando o baud rate informado
 * @param serialNumber qual porta serial será utilizada.
 * No mega pode ser 0, 1, 2 ou 3
 * @param baudRate baud Rate da comunicação.
 */
USART::USART(int serialNumber, long baudRate) {
	this->serial = serialNumber;

	switch (serialNumber) {
		case 0:
			UBRR0H = (byte) BAUD(baudRate) >>8; //Baud Rate MSB
	    	UBRR0L = (byte) BAUD(baudRate);     //Baud Rate LSB
	    	UCSR0A = (1<<U2X0);                 //Dobrador de velocidade
	    	UCSR0B = (1 << RXEN0) | (1<<TXEN0); //Hab TX e RX
	    	UCSR0C = (1<<UCSZ01) | (1<<UCSZ00); //8 bits
			break;
		case 1:
			UBRR1H = (byte) BAUD(baudRate) >>8; //Baud Rate MSB
			UBRR1L = (byte) BAUD(baudRate);     //Baud Rate LSB
			UCSR1A = (1<<U2X1);                 //Dobrador de velocidade
			UCSR1B = (1 << RXEN1) | (1<<TXEN1); //Hab TX e RX
			UCSR1C = (1<<UCSZ11) | (1<<UCSZ10); //8 bits
			break;
		case 2:
			UBRR2H = (byte) BAUD(baudRate) >>8; //Baud Rate MSB
			UBRR2L = (byte) BAUD(baudRate);     //Baud Rate LSB
			UCSR2A = (1<<U2X2);                 //Dobrador de velocidade
			UCSR2B = (1 << RXEN2) | (1<<TXEN2); //Hab TX e RX
			UCSR2C = (1<<UCSZ21) | (1<<UCSZ20); //8 bits
			break;
		case 3:
			UBRR3H = (byte) BAUD(baudRate) >>8; //Baud Rate MSB
	    	UBRR3L = (byte) BAUD(baudRate);     //Baud Rate LSB
	    	UCSR3A = (1<<U2X3);                 //Dobrador de velocidade
	    	UCSR3B = (1<<RXEN3) | (1<<TXEN3);   //Hab TX e RX
	    	UCSR3C = (1<<UCSZ31) | (1<<UCSZ30); //8 bits
			break;
		default:
			break;
	}
}

/**
 * Envia um byte pela porta serial.
 * @param dado byte que vai ser enviado
 */
void USART::sendByte(byte dado) {

	switch (this->serial) {
		case 0:
			UDR0 = dado;                      //Enviar letra
	    	while ( (UCSR0A&(1<<TXC0)) == 0); //Esperar transmitir (TXC0 = 1)
	    	UCSR0A = UCSR0A | (1<<TXC0);      //Apagar bit TXC0
			break;
		case 1:
			UDR1 = dado;                      //Enviar letra
			while ( (UCSR1A&(1<<TXC1)) == 0); //Esperar transmitir (TXC1 = 1)
			UCSR1A = UCSR1A | (1<<TXC1);      //Apagar bit TXC1
			break;
		case 2:
			UDR2 = dado;                      //Enviar letra
			while ( (UCSR2A&(1<<TXC2)) == 0); //Esperar transmitir (TXC2 = 1)
			UCSR2A = UCSR2A | (1<<TXC2);      //Apagar bit TXC2
			break;
		case 3:
			UDR3 = dado;                      //Enviar letra
			while ( (UCSR3A&(1<<TXC3)) == 0); //Esperar transmitir (TXC3 = 1)
			UCSR3A = UCSR3A | (1<<TXC3);      //Apagar bit TXC3
			break;
		default:
			break;
	}
}

/**
 * Lê um byte da porta serial.
 * @return byte lido
 */
byte USART::readByte() {
	byte dado = 0;

	switch (this->serial) {
		case 0:
			while ( (UCSR0A&(1<<RXC0)) == 0); //Esperar receber(RXC0 = 1)
			dado = UDR0;                      //Receber byte a já apaga RXC0
			break;
		case 1:
			while ( (UCSR1A&(1<<RXC1)) == 0); //Esperar receber(RXC1 = 1)
			dado = UDR1;                      //Receber byte a já apaga RXC1
			break;
		case 2:
			while ( (UCSR2A&(1<<RXC2)) == 0); //Esperar receber(RXC2 = 1)
			dado = UDR2;                      //Receber byte a já apaga RXC2
			break;
		case 3:
			while ( (UCSR3A&(1<<RXC3)) == 0); //Esperar receber(RXC3 = 1)
			dado = UDR3;                      //Receber byte a já apaga RXC3
			break;
		default:
			break;
	}

	return dado;
}

/**
 * Imprime uma string
 * @param dado dado que será impresso
 */
void USART::print(char dado[]) {
	word i;
	while (dado[i] != '\0') {
		sendByte(dado[i++]);
	}
}

/**
 * Imprime um char e pula uma linha
 * @param dado dado que será impresso
 */
void USART::println(char dado[]) {
	print(dado);
	sendByte('\n');
}

/**
 * Imprime em decimal com sinal, retirando os zeros à esquerda
 * @param dado dado que será impresso
 */
void USART::print(long dado) {
	int dv;
	long aux;
	bool aux2 = false;

	if (dado < 0) {
		sendByte('-');
	}

	dv = 10000;
	dado = abs(dado);
	if (dado == 0) {
		sendByte(0x30);
	} else {
		for(byte i = 5; i > 0; i--) {
			aux = dado/dv;
			if (aux != 0 || aux2) {
				aux2 = true;
				sendByte(0x30+aux);
			}
			dado = dado - (dv*aux);
			dv = dv/10;
		}
	}
}

/**
 * Imprime em decimal com sinal, retirando os zeros à esquerda
 * @param dado dado que será impresso
 */
void USART::print(int dado) {
	print(long(dado));
}

/**
 * Imprime em decimal com sinal e pula uma linha
 * @param dado dado que será impresso
 */
void USART::println(long dado) {
	print(dado);
	sendByte('\n');
}

/**
 * Imprime em decimal com sinal e pula uma linha
 * @param dado dado que será impresso
 */
void USART::println(int dado) {
	print(long(dado));
	sendByte('\n');
}

/**
 * Imprime em double xx.xxxxxx
 * @param dado dado que será impresso
 */
void USART::print(double dado) {

  	long inteiro, decimal, aux = 0;

	if (dado < 0) {
		sendByte('-');
	}

	dado = fabs(dado);
  	inteiro = long(dado);
  	print(inteiro);

	sendByte(',');

	for (int i = 1; i < 7; i++) {
		decimal = (pow(10,i) * (dado - inteiro)) - 10*aux;
		aux = aux*10 + decimal;
		print(decimal);
	}
}

/**
 * Imprime em double xx.xxxxxx e pula uma linha
 * @param dado dado que será impresso
 */
void USART::println(double dado) {
	print(dado);
	sendByte('\n');
}

/**
 * Se o modo for decimal, imprime em decimal sem sinal um W16. Se o modo for
 * hexadecimal, imprime em hexa um palavra de 16 bits.
 * Possui como padrão o modo decimal.
 * @param dado dado que será impresso
 * @param modo DEC para decimal e HEX para hexadecimal
 */
void USART::print(word dado, int modo) {
  	int aux,dv;
	bool aux2 = false;

	if (modo == DEC) {
	  	dv = 10000;
	  	dado = abs(dado);

		if (dado == 0) {
			sendByte(0x30);
		} else {
		  	for (byte i = 5; i > 0; i--) {
		    	aux = dado/dv;
				if (aux != 0 || aux2) {
					aux2 = true;
					sendByte(0x30+aux);
				}
		    	dado = dado - (dv*aux);
		    	dv = dv/10;
		  	}
		}
	} else if (modo == HEX) {
		print(byte(dado>>8), modo);
		print(byte(dado&0xFF), modo);
	}
}

/**
 * Se o modo for decimal, imprime em decimal sem sinal um W16 e pula uma linha.
 * Se o modo for hexadecimal, imprime em hexa um palavra de 16 bits e pula uma
 * linha. Possui como padrão o modo decimal.
 * @param dado dado que será impresso
 * @param modo DEC para decimal e HEX para hexadecimal
 */
void USART::println(word dado, int modo) {
	print(dado);
	sendByte('\n');
}

/**
 * Se o modo for decimal, imprime em decimal sem sinal um byte retirando os
 * zeros à esquerda. Se o modo for hexadecimal, imprime em Hexa um byte.
 * Possui como padrão o modo decimal.
 * @param dado dado que será impresso
 * @param modo DEC para decimal e HEX para hexadecimal
 */
void USART::print(byte dado, int modo) {
  	char msg[5];
  	byte val,aux;

	if (modo == DEC) {
		val = dado;
		aux = val/100;
		msg[0] = aux + 0x30;
		val = val - 100*aux;
		aux = val/10;
		msg[1] = aux + 0x30;
		msg[2] = (val - 10*aux) + 0x30;
		msg[3] = '\0';

		if (msg[0] != '0') {
			print(&msg[0]);
		}else if (msg[1] != '0') {
			print(&msg[1]);
		}else{
			print(&msg[2]);
		}
	} else if (modo == HEX) {
		aux = (dado>>4) & 0xF;
		if (aux > 9) {
			aux += 0x37;
		}else {
			aux += 0x30;
		}
		sendByte(aux);

		aux = dado & 0xF;
		if (aux > 9) {
			aux += 0x37;
		}else {
			aux += 0x30;
		}
		sendByte(aux);
	} else if (modo == CHAR) {
		sendByte(dado);
	}
}

/**
 * Se o modo for decimal, imprime em decimal sem sinal um byte retirando os
 * zeros à esquerda e pula uma linha. Se o modo for hexadecimal,
 * imprime em Hexa um byte e pula uma linha. Possui como padrão o modo decimal.
 * @param dado dado que será impresso
 * @param modo DEC para decimal e HEX para hexadecimal
 */
void USART::println(byte dado, int modo) {
  	print(dado, modo);
	sendByte('\n');
}
