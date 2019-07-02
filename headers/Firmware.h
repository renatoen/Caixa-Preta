#ifndef FIRMWARE_H
#define FIRMWARE_H

//includes
#include "../headers/TimeLib.h"
#include "../headers/Timer5.h"
#include "../headers/Leds.h"
#include "../headers/MPU.h"
#include "../headers/USART.h"
#include "../headers/TinyGPS++.h"
#include "../headers/EEPROM.h"
#include "../headers/SRAM.h"

typedef struct {
	float lat;
	float lng;
	float veloc;
	uint8_t hora;
	uint8_t minuto;
	uint8_t segundo;
	uint8_t dia;
	uint8_t mes;
	uint16_t ano;
} GPSdata;

class Firmware {
private:
	//-----Classes-----
	MPU mpu;
	USART monitor0, monitor3;
	TinyGPSPlus gps;
	LCD* screen;
	Leds leds;
	Button* key;
	EEPROM eeprom;
	SRAM sram;

	//-----GPS-----
	int UTC_offset = -3; /**< offset do UTC do GPS */
	GPSdata dataGPS[16];
	int indexGPS = 0;

	//-----Parâmetros da batida-----
	// parametros de parada da rotina principal
	// 2g para ter uma batida
	uint16_t dAx = 16384, dAy = 16384, dAz = 16384;
	// 500 graus por segundo para ter uma derrapagem
	uint16_t dgx = 8192, dgy = 8192, dgz = 8192;

	//-----Parâmetros do filtro-----
	float ax, ay, az, gx, gy, gz, mx, my, mz; /**< Variáveis para guardar os últimos dados dos sensores */
	float q[4] = {1.0f, 0.0f, 0.0f, 0.0f}; /**< Vetor para guardar o quaternion */

	float sum = 0.0f;
	float deltat = 0.0f;
	uint32_t lastUpdateMpu = 0;
	uint32_t nowTime = 0;

	// Global constants for 9 DoF fusion and AHRS (Attitude and Heading Reference System)
	float gyroMeasError = 3.14159265358979 * (40.0f / 180.0f);// gyroscope measurement error in rads/s (start at 40 deg/s)
	float gyroMeasDrift = 3.14159265358979 * (2.0f / 180.0f);
	float beta = sqrt(3.0f / 4.0f) * gyroMeasError;
	float zeta = sqrt(3.0f / 4.0f) * gyroMeasDrift;

	float calibrated[3];

	//-----MPU------
	uint16_t mediaMPU[6];
	float erroMPU[6];

	//-----Funções-----
	void getGPSdata(int index);

	void saveDataToEEPROM(uint32_t addrDataStart);

	void saveGPSdataToEEPROM(uint32_t addr);

	void readGPSdataEEPROM();

	void configMPU();

	void calibrateMPU();

	void MadgwickQuaternionUpdate(float ax, float ay, float az, float gx, float gy, float gz);
public:
	Firmware();

	Firmware(LCD* lcd, Button* btn);

	void testGPS();

	void testMPU();

	void rotina();

	void readData();

	void processData();

	void readProcessedData();
};
#endif
