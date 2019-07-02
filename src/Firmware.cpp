#include "../headers/Firmware.h"

Firmware::Firmware() {
}

Firmware::Firmware(LCD* lcd, Button* btn) {

	this->screen = lcd;
	this->key = btn;

	//USART
	this->monitor0 = USART(0, 115200);
	this->monitor3 = USART(3, 9600);

	// MPU
	this->mpu = MPU(55);

	//eeprom
	this->eeprom = EEPROM(400, 0, 0);

	//sram
	this->sram = SRAM(48);
	this->sram.setMode(MODO_SEQ);
}

void Firmware::testGPS() {
	byte tecla;

	while(true) {

		if (this->key->readBuffer(&tecla) == true) {
			break;
		}

		while(this->monitor3.isAvailable()) {
			char c = this->monitor3.readByte();

			if (this->gps.encode(c)) {
				if (this->gps.location.isValid() && this->gps.date.isValid() &&
				this->gps.time.isValid() && this->gps.speed.isValid()) {
					this->monitor0.print("Localização: ");
					this->monitor0.print(this->gps.location.lat());
					this->monitor0.print(", ");
					this->monitor0.println(this->gps.location.lng());
					this->monitor0.print("Velocidade: ");
					this->monitor0.print(this->gps.speed.kmph());
					this->monitor0.println("km/h");

					this->monitor0.print("Data: ");
					this->monitor0.print(this->gps.date.day());
					this->monitor0.print("/");
					this->monitor0.print(this->gps.date.month());
					this->monitor0.print("/");
					this->monitor0.println(this->gps.date.year());
					this->monitor0.print("Hora: ");
					this->monitor0.print(this->gps.time.hour());
					this->monitor0.print(":");
					this->monitor0.print(this->gps.time.minute());
					this->monitor0.print(":");
					this->monitor0.println(this->gps.time.second());
				}
				delay(500);
			}
		}

		if(millis() > 5000 && this->gps.charsProcessed() < 10) {
			this->monitor0.println("-> GPS não identificado.");
			this->monitor0.println("-> Verifique os fios.");
			this->monitor0.println("-> Aperte qualquer botão para sair.");
			this->monitor0.println("");
			delay(2000);
			// while(true);
		}
	}
}

void Firmware::testMPU() {
	byte tecla;

	this->configMPU();

	while(true) {

		if (this->key->readBuffer(&tecla) == true) {
			return;
		}

		this->mpu.readAccelTempGyros();
		this->monitor0.print("ax: ");
		this->monitor0.print(this->mpu.axi);
		this->monitor0.print("  ");
		this->monitor0.print("ay: ");
		this->monitor0.print(this->mpu.ayi);
		this->monitor0.print("  ");
		this->monitor0.print("az: ");
		this->monitor0.print(this->mpu.azi);
		this->monitor0.print(" | ");
		this->monitor0.print("gx: ");
		this->monitor0.print(this->mpu.gxi);
		this->monitor0.print("  ");
		this->monitor0.print("gy: ");
		this->monitor0.print(this->mpu.gyi);
		this->monitor0.print("  ");
		this->monitor0.print("gz: ");
		this->monitor0.println(this->mpu.gzi);
	}
}

void Firmware::calibrateMPU() {
	this->mpu.calibrate(this->mediaMPU, this->erroMPU);
}

void Firmware::configMPU() {
	int i;
	byte tecla;
	i = this->mpu.whoAmI();

	if (i == 0x73) {
		this->monitor0.println("MPU conectado");
		this->calibrateMPU();
		// if (mpu.selfTest()) {
		// 	this->monitor0.println("MPU passou no teste");
		// } else {
		// 	this->monitor0.println("MPU falhou no teste");
		// 	while(true) {
		// 		if (key->readBuffer(&tecla) == true) {
		// 			return;
		// 		}
		// 	}
		// }
		delay(2000);
	} else {
		this->monitor0.println("MPU desconectado");
		while(true) {
			if (this->key->readBuffer(&tecla) == true) {
				return;
			}
		}
	}
}

void Firmware::rotina() {
	int16_t dados[288]; // Armazena os dados de 32 leituras de 18 bytes cada do mpu = 576 bytes
	uint8_t endRoutine = 0, evento = 0;
	int32_t addrSram = 0; // Comeca a escrita a partir do início da SRAM
	int32_t addrEvento; // Endereco do evento
	int32_t addrDataStart = 0;
	uint32_t t_f;
	uint16_t writes = 0;
	int16_t dadosGPS[20]; // Dados GPS - 8 amostras de 40 bytes
	int contLeituras = 0, contGPS = 0;
	byte tecla;

	this->screen->loadBufferWhite();

	this->configMPU();

	delay(100);

	while(!endRoutine) {

  		if (this->key->readBuffer(&tecla) == true) {
			evento = 1;   // Para encerrar o loop pressione qualquer botao
		}

	  	do {
			delayMicroseconds(10);
	  	} while((micros() - t_f) < 10000);

		// Le um conjunto de dados do MPU (ax, ay, az, gx, gy, gz)
		//TODO: adicionar magnetometro
		this->mpu.readAccelTempGyros();
		dados[contLeituras*6 + 0] = this->mpu.axi;
		dados[contLeituras*6 + 1] = this->mpu.ayi;
		dados[contLeituras*6 + 2] = this->mpu.azi;
		dados[contLeituras*6 + 3] = this->mpu.gxi;
		dados[contLeituras*6 + 4] = this->mpu.gyi;
		dados[contLeituras*6 + 5] = this->mpu.gzi;

	  	if(contGPS == 14) {
			this->monitor0.print("GPS antes do evento ");
			this->monitor0.println(this->indexGPS);
			this->getGPSdata(this->indexGPS);
			this->indexGPS++;
			if(this->indexGPS == 16) {
				this->indexGPS = 0;
			}
			contGPS = 0;
	  	}

	  	contLeituras++;
	  	t_f = micros();

	  	// TODO: Colocar aqui os parametros para indicar a batida, por enquanto o evento é apertar o botao
	  	if((addrSram + contLeituras*18) == 65088) {
			evento = 1;
	  	}

	  	// Quando contLeituras chegar a 32, armazenar da SRAM
	  	if(contLeituras == 32) {
			this->sram.writeSeq(addrSram, dados, 288);
			contGPS++;

			if(evento) {
		  		endRoutine = 1;
		  		addrEvento = addrSram + contLeituras*18;
			}

			// Incrementar endereco para a proxima escrita na SRAM
			addrSram += sizeof(dados);

			if(addrSram >= 131071) {
				addrSram -= 131071;
			}

			contLeituras = 0;

			this->screen->loadBufferWhite();
			this->screen->loadBuffer("Escritas:",0,0);
			this->screen->printDec8(writes,0,0);
			writes++;
	  	}
	}

	// Loop para preencher o restante da memória no pós evento
	for(int i = 0; i < 112; i++) {
  		for(int j = 0; j < 32; j++) {
			do {
	  			delayMicroseconds(10);
			} while((micros() - t_f) < 10000);

			// Le um conjunto de dados do MPU (ax, ay, az, gx, gy, gz)
			//TODO: adicionar magnetometro
			this->mpu.readAccelTempGyros();
			dados[contLeituras*6 + 0] = this->mpu.axi;
			dados[contLeituras*6 + 1] = this->mpu.ayi;
			dados[contLeituras*6 + 2] = this->mpu.azi;
			dados[contLeituras*6 + 3] = this->mpu.gxi;
			dados[contLeituras*6 + 4] = this->mpu.gyi;
			dados[contLeituras*6 + 5] = this->mpu.gzi;

			if(contGPS == 14) {
	  			this->monitor0.print("GPS depois do evento ");
				this->monitor0.println(this->indexGPS);
	  			this->getGPSdata(this->indexGPS);
	  			this->indexGPS++;
	  			if(this->indexGPS == 16) {
					this->indexGPS = 0;
				}
	  			contGPS = 0;
			}

			contLeituras++;
			t_f = micros();

			if(contLeituras == 32) {
	  			this->sram.writeSeq(addrSram, dados, 288);
	  			addrSram += sizeof(dados);
	  			if(addrSram >= 131071) {
					addrSram -= 131071;
				}
	  			contGPS++;

				this->screen->loadBufferWhite();
				this->screen->loadBuffer("Escritas:",0,0);
				this->screen->printDec8(writes,0,0);

	  			writes++;
	  			contLeituras = 0;
			}
  		}
	}

	// Dados GPS - 8 amostras de 40 bytes
	int g;
	for(g = 0; g < 20; g++)
		dadosGPS[g] = 0;
	for(g = 0; g < 8; g++) {
  		this->sram.writeSeq(addrSram, dadosGPS, 20);
  		addrSram += sizeof(dadosGPS);
	}

	// Definir endereco do inicio dos dados
	uint32_t dataPreEvent = 65088; // 32*18*113 - Dados antes do evento
	addrDataStart = (addrEvento - dataPreEvent); // Dados do pre-evento
	if(addrDataStart < 0) {
		// Garante que o inicio tem um valor válido
		addrDataStart += 131071;
	}
	this->saveDataToEEPROM(addrDataStart);
}

void Firmware::readData() {
	uint8_t leitura[18];


	uint32_t j;
	for(j = 0; j < 130752; j+=18) {
		this->eeprom.read(j, leitura, sizeof(leitura));

	    for(uint8_t i = 0; i < 18; i += 2) {
	      	this->monitor0.print(((int16_t)leitura[i+1] << 8 | leitura[i]));
			this->monitor0.print("\t");
	    }
	    this->monitor0.println("");
	}

	this->monitor0.println("Dados GPS");
	this->readGPSdataEEPROM();
}

void Firmware::processData() {
	uint8_t leitura[18];
	uint32_t now = 0;
	uint32_t start;
	int count = 0;

	this->screen->loadBufferWhite();


	for(uint32_t j = 0; j < 0x20000; j+=18) {
  		this->eeprom.read(j, leitura, sizeof(leitura));

  		unsigned long t_now = millis();
  		start = micros();

		this->ax = (float)(((int16_t)leitura[1] << 8) | leitura[0]) * (this->mpu.getAcelRes());
		this->ay = (float)(((int16_t)leitura[3] << 8) | leitura[2]) * (this->mpu.getAcelRes());
		this->az = (float)(((int16_t)leitura[5] << 8) | leitura[4]) * (this->mpu.getAcelRes());

		this->gx = (float)(((int16_t)leitura[7] << 8) | leitura[6]) * (this->mpu.getGiroRes()); //- mpu.gyroBias[0];
		this->gy = (float)(((int16_t)leitura[9] << 8) | leitura[8]) * (this->mpu.getGiroRes()); //- mpu.gyroBias[1];
		this->gz = (float)(((int16_t)leitura[11] << 8) | leitura[10]) * (this->mpu.getGiroRes()); //- mpu.gyroBias[2];

		this->nowTime = micros();
		this->deltat = ((this->nowTime - this->lastUpdateMpu)/1000000.0f);
		this->lastUpdateMpu = this->nowTime;
		this->sum += this->deltat;

  		do {
			delayMicroseconds(10);
  		} while((micros() - start) < 10000);

  		MadgwickQuaternionUpdate(-ax, ay, az, gx*PI/180.0f, -gy*PI/180.0f, -gz*PI/180.0f);

  		// Teste para a simulacao
  		this->monitor0.print(this->q[0]);
		this->monitor0.print(" ");
  		this->monitor0.print(this->q[1]);
		this->monitor0.print(" ");
  		this->monitor0.print(this->q[2]);
		this->monitor0.print(" ");
  		this->monitor0.print(this->q[3]);
		this->monitor0.print(" ");

  		this->monitor0.print(ax);
		this->monitor0.print(" ");
  		this->monitor0.print(ay);
		this->monitor0.print(" ");
  		this->monitor0.println(az);
	}

}

void Firmware::readProcessedData() {

}

void Firmware::getGPSdata(int index) {
	bool ready = false;

  	while(!ready) {
    	if(this->monitor3.isAvailable()) {
      		char c = this->monitor3.readByte();

      		if(this->gps.encode(c)) {
        		if(this->gps.time.isValid() && this->gps.location.isValid() && this->gps.speed.isValid() && this->gps.date.isValid()) {
          			ready = true;
			        this->dataGPS[index].lat = this->gps.location.lat();
			        this->dataGPS[index].lng = this->gps.location.lng();
			        this->dataGPS[index].veloc = this->gps.speed.kmph();
			        this->dataGPS[index].hora = this->gps.time.hour();
			        this->dataGPS[index].minuto = this->gps.time.minute();
			        this->dataGPS[index].segundo = this->gps.time.second();
			        this->dataGPS[index].dia = this->gps.date.day();
			        this->dataGPS[index].mes = this->gps.date.month();
			        this->dataGPS[index].ano = this->gps.date.year();
        		}
      		}
    	}
  	}
}

void Firmware::saveDataToEEPROM(uint32_t addrDataStart) {
  	int16_t dataSRAMtoEEPROM[64], dataSRAMtoEEPROM1[32], gps_dataSRAMtoEEPROM[40];
  	uint32_t cont, cont1, addrEscritaEEPROM, addrData = 0;

	this->screen->loadBufferWhite();
	this->screen->loadBuffer("Save to EEPROM",0,0);
  	delay(50);
  	this->screen->loadBufferWhite();

  	// Escrever na EEPROM de 128 em 128 bytes
  	for(cont = 0; cont < 1021; cont++) {
    	addrData = (addrDataStart + cont*128);
    	if(addrData > 131071)
			addrData -= 131071;
    	this->sram.readSeq(addrData, dataSRAMtoEEPROM, sizeof(dataSRAMtoEEPROM));
    	this->eeprom.write(cont*128, dataSRAMtoEEPROM, sizeof(dataSRAMtoEEPROM));
  	}

  	// 64 bytes seguintes
  	addrData = addrDataStart + cont*128;
  	if(addrData > 131071) {
		addrData -= 131071;
	}
  	this->sram.readSeq(addrData, dataSRAMtoEEPROM1, 64);
  	this->eeprom.write(cont*128, dataSRAMtoEEPROM1, sizeof(dataSRAMtoEEPROM1));

  	// Ultimos dados - GPS - 320 bytes - 4 leituras de 80 bytes
  	addrEscritaEEPROM = cont*128 + 64;
  	// this->monitor0.print("addrEscrita: ");
	// this->monitor0.println(addrEscritaEEPROM);

  	this->saveGPSdataToEEPROM(addrEscritaEEPROM);
  	delay(500);
  	this->monitor0.println("Dados salvos na EEPROM");
}

void Firmware::saveGPSdataToEEPROM(uint32_t addr) {
  	int16_t zeros[8];
  	for(int i = 0; i < 16; i++) {
    	this->eeprom.setPosition(addr);
	    this->eeprom.writeFloat(this->dataGPS[i].lat);
		addr += sizeof(float);
		this->eeprom.setPosition(addr);

	    this->eeprom.writeFloat(this->dataGPS[i].lng);
		addr += sizeof(float);
		this->eeprom.setPosition(addr);

	    this->eeprom.writeFloat(this->dataGPS[i].veloc);
		addr += sizeof(float);
		this->eeprom.setPosition(addr);

	    this->eeprom.writeByte(this->dataGPS[i].hora);
		addr += sizeof(byte);
		this->eeprom.setPosition(addr);

	    this->eeprom.writeByte(this->dataGPS[i].minuto);
		addr += sizeof(byte);
		this->eeprom.setPosition(addr);

	    this->eeprom.writeByte(this->dataGPS[i].segundo);
		addr += sizeof(byte);
		this->eeprom.setPosition(addr);

	    this->eeprom.writeByte(this->dataGPS[i].dia);
		addr += sizeof(byte);
		this->eeprom.setPosition(addr);

	    this->eeprom.writeByte(this->dataGPS[i].mes);
		addr += sizeof(byte);
		this->eeprom.setPosition(addr);

	    this->eeprom.writeInt(this->dataGPS[i].ano);
		addr += sizeof(int);
		this->eeprom.setPosition(addr);
  	}

  	for(int j = 0; j < 8; j++) {
		zeros[j] = 0;
	}

  	this->eeprom.write(addr, zeros, sizeof(zeros));
}

void Firmware::readGPSdataEEPROM() {
  	float latitude, longitude, velocidade;
  	byte dia, mes, hora, minuto, segundo;
  	int ano;
  	uint32_t addr = 130752;
  	this->eeprom.setPosition(addr);
  	for(int i = 0; i < 16; i++) {
    	latitude = eeprom.readFloat();
		addr += sizeof(float);
		eeprom.setPosition(addr);

    	longitude = eeprom.readFloat();
		addr += sizeof(float);
		eeprom.setPosition(addr);

    	velocidade = eeprom.readFloat();
		addr += sizeof(float);
		eeprom.setPosition(addr);

    	hora = eeprom.readByte();
		addr += sizeof(byte);
		eeprom.setPosition(addr);

    	minuto = eeprom.readByte();
		addr += sizeof(byte);
		eeprom.setPosition(addr);

    	segundo = eeprom.readByte();
		addr += sizeof(byte);
		eeprom.setPosition(addr);

    	dia = eeprom.readByte();
		addr += sizeof(byte);
		eeprom.setPosition(addr);

    	mes = eeprom.readByte();
		addr += sizeof(byte);
		eeprom.setPosition(addr);

    	ano = eeprom.readInt();
		addr += sizeof(int);
		eeprom.setPosition(addr);

    	setTime(hora, minuto, segundo, dia, mes, ano);
    	adjustTime(UTC_offset * SECS_PER_HOUR);

    	this->monitor0.print("Localização: ");
    	this->monitor0.print(latitude);
		this->monitor0.print(" ");
		this->monitor0.print(longitude);
		this->monitor0.println(" ");
    	this->monitor0.print("Velocidade: ");
    	this->monitor0.println(velocidade);
		// this->monitor0.println("km/h");
    	this->monitor0.print("Data: ");
		this->monitor0.print(day());
		this->monitor0.print("/");
		this->monitor0.print(month());
		this->monitor0.print("/");
		this->monitor0.println(year());
    	this->monitor0.print("Hora: ");
		this->monitor0.print(hour());
		this->monitor0.print(":");
		this->monitor0.print(minute());
		this->monitor0.print(":");
		this->monitor0.println(second());
  	}
}

// Implementation of Sebastian Madgwick's "...efficient orientation filter for... inertial/magnetic sensor arrays"
// (see http://www.x-io.co.uk/category/open-source/ for examples and more details)
// which fuses acceleration and rotation rate to produce a quaternion-based estimate of relative
// device orientation -- which can be converted to yaw, pitch, and roll. Useful for stabilizing quadcopters, etc.
// The performance of the orientation filter is at least as good as conventional Kalman-based filtering algorithms
// but is much less computationally intensive---it can be performed on a 3.3 V Pro Mini operating at 8 MHz!
void Firmware::MadgwickQuaternionUpdate(float ax, float ay, float az, float gx, float gy, float gz) {
  	float q1 = this->q[0], q2 = this->q[1], q3 = this->q[2], q4 = this->q[3];         // short name local variable for readability
  	float norm;                                               // vector norm
  	float f1, f2, f3;                                         // objetive funcyion elements
  	float J_11or24, J_12or23, J_13or22, J_14or21, J_32, J_33; // objective function Jacobian elements
  	float qDot1, qDot2, qDot3, qDot4;
  	float hatDot1, hatDot2, hatDot3, hatDot4;
  	float gerrx, gerry, gerrz, gbiasx, gbiasy, gbiasz;        // gyro bias error

  	// Auxiliary variables to avoid repeated arithmetic
  	float _halfq1 = 0.5f * q1;
  	float _halfq2 = 0.5f * q2;
  	float _halfq3 = 0.5f * q3;
  	float _halfq4 = 0.5f * q4;
  	float _2q1 = 2.0f * q1;
  	float _2q2 = 2.0f * q2;
  	float _2q3 = 2.0f * q3;
  	float _2q4 = 2.0f * q4;
  	float _2q1q3 = 2.0f * q1 * q3;
  	float _2q3q4 = 2.0f * q3 * q4;

  	// Normalise accelerometer measurement
  	norm = sqrt(ax * ax + ay * ay + az * az);
  	if (norm == 0.0f) {
		return; // handle NaN
	}
  	norm = 1.0f/norm;
  	ax *= norm;
  	ay *= norm;
  	az *= norm;

  	// Compute the objective function and Jacobian
  	f1 = _2q2 * q4 - _2q1 * q3 - ax;
  	f2 = _2q1 * q2 + _2q3 * q4 - ay;
  	f3 = 1.0f - _2q2 * q2 - _2q3 * q3 - az;
  	J_11or24 = _2q3;
  	J_12or23 = _2q4;
  	J_13or22 = _2q1;
  	J_14or21 = _2q2;
  	J_32 = 2.0f * J_14or21;
  	J_33 = 2.0f * J_11or24;

  	// Compute the gradient (matrix multiplication)
  	hatDot1 = J_14or21 * f2 - J_11or24 * f1;
  	hatDot2 = J_12or23 * f1 + J_13or22 * f2 - J_32 * f3;
  	hatDot3 = J_12or23 * f2 - J_33 *f3 - J_13or22 * f1;
  	hatDot4 = J_14or21 * f1 + J_11or24 * f2;

  	// Normalize the gradient
  	norm = sqrt(hatDot1 * hatDot1 + hatDot2 * hatDot2 + hatDot3 * hatDot3 + hatDot4 * hatDot4);
  	hatDot1 /= norm;
  	hatDot2 /= norm;
  	hatDot3 /= norm;
  	hatDot4 /= norm;

  	// Compute estimated gyroscope biases
  	gerrx = _2q1 * hatDot2 - _2q2 * hatDot1 - _2q3 * hatDot4 + _2q4 * hatDot3;
  	gerry = _2q1 * hatDot3 + _2q2 * hatDot4 - _2q3 * hatDot1 - _2q4 * hatDot2;
  	gerrz = _2q1 * hatDot4 - _2q2 * hatDot3 + _2q3 * hatDot2 - _2q4 * hatDot1;

  	// Compute and remove gyroscope biases
  	gbiasx += gerrx * this->deltat * this->zeta;
  	gbiasy += gerry * this->deltat * this->zeta;
  	gbiasz += gerrz * this->deltat * this->zeta;
  	gx -= gbiasx;
  	gy -= gbiasy;
  	gz -= gbiasz;

  	// Compute the quaternion derivative
  	qDot1 = -_halfq2 * gx - _halfq3 * gy - _halfq4 * gz;
  	qDot2 =  _halfq1 * gx + _halfq3 * gz - _halfq4 * gy;
  	qDot3 =  _halfq1 * gy - _halfq2 * gz + _halfq4 * gx;
  	qDot4 =  _halfq1 * gz + _halfq2 * gy - _halfq3 * gx;

  	// Compute then integrate estimated quaternion derivative
  	q1 += (qDot1 -(this->beta * hatDot1)) * this->deltat;
  	q2 += (qDot2 -(this->beta * hatDot2)) * this->deltat;
  	q3 += (qDot3 -(this->beta * hatDot3)) * this->deltat;
  	q4 += (qDot4 -(this->beta * hatDot4)) * this->deltat;

  	// Normalize the quaternion
  	norm = sqrt(q1 * q1 + q2 * q2 + q3 * q3 + q4 * q4);    // normalise quaternion
  	norm = 1.0f/norm;
  	this->q[0] = q1 * norm;
  	this->q[1] = q2 * norm;
  	this->q[2] = q3 * norm;
  	this->q[3] = q4 * norm;
}
