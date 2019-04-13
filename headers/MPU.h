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
	// variáveis
	int frequencia;
	float giro_res, acel_res;

public:
	// variáveis
	int axi,ayi,azi,tpi,gxi,gyi,gzi;      // leituras instantâneas do MPU

	// métodos
	MPU();
	MPU(int freq);
	bool writeRegister(byte reg, byte dado);
	byte readRegister(byte reg);
	bool writeBlockData(byte reg, byte* dado, byte qtd);
	void readBlockData(byte reg, byte* dado, byte qtd);
	byte whoAmI();
	void wakeUp();
	void sleep();
	void setScale(byte gfs, byte afs);
	void readAverageAccelTempGyros(word rpt);
	void readAccelTempGyros();
	bool selfTest();
	void calibrate(int16_t *bias, float  *valor);
};

#endif
