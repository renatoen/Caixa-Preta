#ifndef SPI_H
#define SPI_H

// includes
#include <stdint.h>
#include <Arduino.h>

class SPI {
private:
	int slaveSelect; /**< Pino que o escravo está conectado */

public:
	/**
	 * Construtor default da classe.
	 */
	SPI();

	/**
	 * Construtor da classe SPI. Define a porta que o escravo está e se vai iniciar
	 * como escravo ou mestre, sendo default inciar como mestre
	 * @params slaveSelect porta em que o escravo está conectado
	 * @params mestre TRUE se for mestre (padrão) e FALSE se for escravo
	 */
	SPI(int slaveSelect, bool mestre = true);

	/**
	 * Coloca a porta do slave em nível baixo, sinalizando que o Arduino o escolheu
	 */
	void start();

	/**
	 * Coloca a porta do slave em nível alto, liberando o canal para outra escolha
	 * de escravo
	 */
	void stop();

	/**
	 * Envia e recebe um byte ao mesmo tempo pelo protocolo de comunicação SPI
	 * @param dado dado em 8 bits que vai ser enviado
	 * @return dado que foi recebido
	 */
	uint8_t transferir(uint8_t dado);
};
#endif
