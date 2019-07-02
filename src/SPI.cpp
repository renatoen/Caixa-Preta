#include "../headers/SPI.h"
/**
 * Construtor default da classe.
 */
SPI::SPI() {

}

/**
 * Construtor da classe SPI. Define a porta que o escravo está e se vai iniciar
 * como escravo ou mestre, sendo default inciar como mestre
 * @params slaveSelect porta em que o escravo está conectado
 * @params mestre TRUE se for mestre (padrão) e FALSE se for escravo
 */
 SPI::SPI(int slaveSelect, bool mestre) {
 	this->slaveSelect = slaveSelect;
	pinMode(slaveSelect, OUTPUT);

 	if (mestre) {
 		SPCR = (1<<SPE) | (1<<MSTR) | (1<<SPR0); //Hab. SPI Mestre, SCK = 1 MHz
 	} else {
 		SPCR = (1<<SPE) | (1<<SPR0); //Hab. SPI Mestre, SCK = 1 MHz
 	}
 }

/**
 * Envia e recebe um byte ao mesmo tempo pelo protocolo de comunicação SPI
 * @param dado dado em 8 bits que vai ser enviado
 * @return dado que foi recebido
 */
uint8_t SPI::transferir(uint8_t dado){

	SPDR = dado;
	while(!(SPSR & (1<<SPIF)));
	return SPDR;
}

/**
 * Coloca a porta do slave em nível baixo, sinalizando que o Arduino o escolheu
 */
void SPI::start() {
	digitalWrite(this->slaveSelect, LOW);
    delay(5);
}

/**
 * Coloca a porta do slave em nível alto, liberando o canal para outra escolha
 * de escravo
 */
void SPI::stop() {
	digitalWrite(this->slaveSelect, HIGH);
}
