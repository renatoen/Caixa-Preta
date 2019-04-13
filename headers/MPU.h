#ifndef MPU_H
#define MPU_H

// Includes
#include <Arduino.h>
#include "TWI.h"

// Defines

/////////////// MPU 9250 - Constantes
#define MPU_ADR  0x68  // Endereço MPU-9250
#define MPU_EWR  0xD0  // MPU para escrita (0x68<<1)
#define MPU_ERD  0xD1  // MPU para leitura (ox68<<1 + 1)

//Escalas para Giroscópio
#define GIRO_FS_250  0   // +/- 250 graus/seg
#define GIRO_FS_500  1   // +/- 500 graus/seg
#define GIRO_FS_1000 2   // +/- 1000 graus/seg
#define GIRO_FS_2000 3   // +/- 2000 graus/seg

//Escalas para Acelerômetro
#define ACEL_FS_2G  0   // +/- 2g
#define ACEL_FS_4G  1   // +/- 4g
#define ACEL_FS_8G  2   // +/- 8g
#define ACEL_FS_16G 3   // +/- 16g

// Registradores do MPU-6050 que foram usados
#define SELF_TEST_X      0x0D
#define SELF_TEST_Y      0x0E
#define SELF_TEST_Z      0x0F
#define SELF_TEST_A      0x10
#define SMPLRT_DIV       0x19
#define CONFIG           0x1A
#define GYRO_CONFIG      0x1B
#define ACCEL_CONFIG     0x1C
#define FIFO_EN          0x23
#define INT_PIN_CFG      0x37
#define INT_ENABLE       0x38
#define INT_STATUS       0x3A
#define ACCEL_XOUT_H     0x3B
#define USER_CTRL        0x6A
#define PWR_MGMT_1       0x6B
#define FIFO_COUNTH      0x72
#define FIFO_COUNTL      0x73
#define FIFO_R_W         0x74
#define WHO_AM_I         0x75

class MPU {
private:

	int frequencia;	/**< frequência da comunicação I2C do mpu */
	float giro_res;	/**< resolução do giroscópio */
	float acel_res;	/**< resolução do acelerômetro */

public:

	int axi;	/**< leitura instântanea da aceleração no eixo x */
	int ayi;	/**< leitura instântanea da aceleração no eixo y */
	int azi;	/**< leitura instântanea da aceleração no eixo z */
	int tpi;	/**< leitura instântanea da temperatura interna do MPU */
	int gxi;	/**< leitura instântanea do giroscópio no eixo x */
	int gyi;	/**< leitura instântanea do giroscópio no eixo y */
	int gzi;	/**< leitura instântanea do giroscópio no eixo z */

	/**
	 * Construtor default da classe.
	 */
	MPU();

	/**
	 * Construtor da classe. Coloca o MPU num estado conhecido. Algumas operações
	 * podem ser redundantes.
	 * @param freq frequência utilizada
	 */
	MPU(int freq);

	/**
	 * Escrever em um registrador do MPU
	 * @param reg registrador
	 * @param dado dado que vai ser escrito
	 * @return TRUE se dado foi escrito, FALSE caso contrário
	 */
	bool writeRegister(byte reg, byte dado);

	/**
	 * Ler um registrador do MPU
	 * @param reg registrador
	 * @return dado que foi lido
	 */
	byte readRegister(byte reg);

	/**
	 * Escrever um bloco de dados no MPU a partir de um registrador
	 * @param reg registrador
	 * @param dado bloco de dados que vai ser escrito
	 * @param qtd tamanho do bloco de dados
	 * @return TRUE se dados foram escritos, FALSE caso contrário
	 */
	bool writeBlockData(byte reg, byte* dado, byte qtd);

	/**
	 * Lê um bloco de dados no MPU a partir de um registrador
	 * @param reg registrador
	 * @param dado bloco de dados em que os dados da leitura serão armazenados
	 * @param qtd tamanho do bloco de dados
	 */
	void readBlockData(byte reg, byte* dado, byte qtd);

	/**
	 * Lê o registrador WHO_AM_I
	 * @return valor do registrador
	 */
	byte whoAmI();

	/**
	 * Acordar o MPU e programar para usar relógio Giro X
	 */
	void wakeUp();

	/**
	 * Dormir o MPU e programar para usar relógio Giro X
	 */
	void sleep();

	/**
	 * Seleciona Fundo de Escalas para o MPU
	 * @param gfs FS do giroscópio
	 * @param afs FS do acelerômetro
	 */
	void setScale(byte gfs, byte afs);

	/**
	 * Lê a média da aceleração, temperatura e giroscópio
	 * @param rpt número de leituras que vai ser tirada a média
	 */
	void readAverageAccelTempGyros(word rpt);

	/**
	 * Lê a aceleração, temperatura e giroscópio
	 */
	void readAccelTempGyros();

	/**
	 * Faz um self test do MPU
	 * @return TRUE se passar do teste, FALSE caso contrário
	 */
	bool selfTest();

	/**
	 * Calibra o acelerômetro e o giroscópio do MPU
	 * @param bias
	 * @param valor
	 */
	void calibrate(int16_t *bias, float  *valor);
};

#endif
