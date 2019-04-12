#include "../headers/MPU.h"

/**
 * Init da classe. Coloca o MPU num estado conhecido. Algumas operações
 * podem ser redundantes.
 * @param freq frequência utilizada
 */
MPU::MPU(int freq) {

	this->frequencia = freq;

	// Despertar MPU, Relógio = PLL do Giro-x
	writeRegister(PWR_MGMT_1, 0x01);
	delay(200);       //200ms - Esperar PLL estabilizar

	// 6 ==> Taxa = 1 kHz, Banda: Acel=5 Hz e Giro=5 Hz e delay=19 mseg
	writeRegister(CONFIG, 6);

	// Taxa de amostragem =  taxa/(1+SMPLRT_DIV) = 1k/10 = 100Hz
	writeRegister(SMPLRT_DIV, 9);  //Taxa = 100 Hz
	//mpu_wr(SMPLRT_DIV, 4);  //Taxa = 1k/5 = 500 Hz
}

/**
 * Ler um registrador do MPU
 * @param reg registrador
 * @return dado que foi lido
 */
byte MPU::readRegister(byte reg) {
	uint8_t dado = 0;
	TWI leitura(this->frequencia);

	if (leitura.sendStart()) {
		// enviou start
		if (leitura.sendWriteAddress(MPU_EWR)) {
			// enviou endereço de escrita
			if (leitura.sendData(reg)) {
				// enviou reg de leitura
				if (leitura.sendRepeatedStart()) {
					// enviou start repetido
					if (leitura.sendReadAddress(MPU_ERD)) {
						// enviou endereço de leitura

						// receber dado com NACK
						dado = leitura.readData(false);

						if (dado != 0) {
							// recebeu o dado
							leitura.sendStop();
						}
					} else {
						// erro no envio de endereço de leitura
					}
				}else {
					// erro no envio do start repetido
				}
			} else {
				// erro no envio do reg de leitura
			}
		}else {
			// erro no envio de end de escrita
		}
	}else {
		// erro no envio do start
	}

	return dado;
}

/**
 * Escrever em um registrador do MPU
 * @param reg registrador
 * @param dado dado que vai ser escrito
 * @return TRUE se dado foi escrito, FALSE caso contrário
 */
bool MPU::writeRegister(byte reg, byte dado) {
	TWI escrita(this->frequencia);

	if (escrita.sendStart()) {
		// mandou start
		if (escrita.sendWriteAddress(MPU_EWR)) {
			// mandou endereço
			if (escrita.sendData(reg)) {
				// mandou primeiro dado
				if (escrita.sendData(dado)) {
					// mandou segundo dado
					escrita.sendStop();
					return true;
				} else {
					// erro para mandar segundo dado
				}
			} else {
				// erro para mandar primeiro dado
			}
		} else {
			// erro para mandar endereço
		}
	}else {
		// erro no start
	}

	return false;
}

/**
 * Escrever um bloco de dados no MPU a partir de um registrador
 * @param reg registrador
 * @param dado bloco de dados que vai ser escrito
 * @param qtd tamanho do bloco de dados
 * @return TRUE se dados foram escritos, FALSE caso contrário
 */
bool MPU::writeBlockData(byte reg, byte* dado, byte qtd) {
	TWI escrita(this->frequencia);

	if (escrita.sendStart()) {
		// mandou start
		if (escrita.sendWriteAddress(MPU_EWR)) {
			// mandou endereço
			if (escrita.sendData(reg)) {
				// mandou primeiro dado
				for(uint8_t i = 0; i < qtd; i++) {
					if (!escrita.sendData(dado[i])) {
						// erro na escrita
						return false;
					}
				}
				escrita.sendStop();
				return true;
			} else {
				// erro para mandar primeiro dado
			}
		} else {
			// erro para mandar endereço
		}
	}else {
		// erro no start
	}

	return false;
}

/**
 * Lê um bloco de dados no MPU a partir de um registrador
 * @param reg registrador
 * @param dado bloco de dados em que os dados da leitura serão armazenados
 * @param qtd tamanho do bloco de dados
 */
void MPU::readBlockData(byte reg, byte* dado, byte qtd) {

	TWI leitura(this->frequencia);

	if (leitura.sendStart()) {
		// enviou start
		if (leitura.sendWriteAddress(MPU_EWR)) {
			// enviou endereço de escrita
			if (leitura.sendData(reg)) {
				// enviou reg de leitura
				if (leitura.sendRepeatedStart()) {
					// enviou start repetido
					if (leitura.sendReadAddress(MPU_ERD)) {
						// enviou endereço de leitura

						for (uint8_t i = 0; i < qtd; i++) {
							if ((i + 1) < qtd) {
								dado[i] = leitura.readData(true); // Receber dados e gerar ACK
							}else {
								dado[i] = leitura.readData(false); // Receber último dado e gerar NACK
							}
						}
						//TODO: arrumar um jeito de reportar erro na leitura do bloco
						leitura.sendStop();

					} else {
						// erro no envio de endereço de leitura
					}
				}else {
					// erro no envio do start repetido
				}
			} else {
				// erro no envio do reg de leitura
			}
		}else {
			// erro no envio de end de escrita
		}
	}else {
		// erro no envio do start
	}

}
/**
 * Lê o registrador WHO_AM_I
 * @return valor do registrador
 */
byte MPU::whoAmI() {

	return readRegister(WHO_AM_I);
}

/**
 * Acordar o MPU e programar para usar relógio Giro X
 */
void MPU::wakeUp() {
  	writeRegister(PWR_MGMT_1, 1);
}

/**
 * Dormir o MPU e programar para usar relógio Giro X
 */
void MPU::sleep() {
  	writeRegister(PWR_MGMT_1, 0x21);  //SLEEP=1 e PLL com Giro X
}

/**
 * Seleciona Fundo de Escalas para o MPU
 * @param gfs FS do giroscópio
 * @param afs FS do acelerômetro
 */
void MPU::setScale(byte gfs, byte afs) {
	writeRegister(GYRO_CONFIG, gfs << 3); //FS do Giro

	writeRegister(ACCEL_CONFIG, afs << 3); //FS do Acel

	// Calcular resoluçao do Giroscópio
    switch (gfs) {
    	case GIRO_FS_250:
			this->giro_res =  250.0 / 32768.0;
			break;
      	case GIRO_FS_500:
			this->giro_res =  500.0 / 32768.0;
			break;
      	case GIRO_FS_1000:
			this->giro_res = 1000.0 / 32768.0;
			break;
      	case GIRO_FS_2000:
			this->giro_res = 2000.0 / 32768.0;
			break;
    }

	// Calcular resoluçao do Acelerômetro
	switch (afs) {
		case ACEL_FS_2G:
			this->acel_res =  2.0 / 32768.0;
			break;
	  	case ACEL_FS_4G:
			this->acel_res =  4.0 / 32768.0;
			break;
	  	case ACEL_FS_8G:
			this->acel_res =  8.0 / 32768.0;
			break;
	  	case ACEL_FS_16G:
			this->acel_res = 16.0 / 32768.0;
			break;
	}

}

/**
 * Lê a média da aceleração, temperatura e giroscópio
 * @param rpt número de leituras que vai ser tirada a média
 */
void MPU::readAverageAccelTempGyros(word rpt) {
  	byte vet[14];
  	long lax,lay,laz,ltp,lgx,lgy,lgz;
  	lax = lay = laz = ltp = lgx = lgy = lgz = 0;

  	for (word i = 0; i < rpt; i++){
    	readBlockData(ACCEL_XOUT_H, vet, 14);
    	lax += (int)((vet[ 0] << 8) | vet[ 1]);    //Montar Acel X
    	lay += (int)((vet[ 2] << 8) | vet[ 3]);    //Montar Acel Y
    	laz += (int)((vet[ 4] << 8) | vet[ 5]);    //Montar Acel Z
    	ltp += (int)((vet[ 6] << 8) | vet[ 7]);    //Montar Temp
    	lgx += (int)((vet[ 8] << 8) | vet[ 9]);    //Montar Giro X
    	lgy += (int)((vet[10] << 8) | vet[11]);    //Montar Giro Y
    	lgz += (int)((vet[12] << 8) | vet[13]);    //Montar Giro Z
  	}

  	axi = lax/rpt;
  	ayi = lay/rpt;
  	azi = laz/rpt;
  	tpi = ltp/rpt;
  	gxi = lgx/rpt;
  	gyi = lgy/rpt;
  	gzi = lgz/rpt;
}

/**
 * Lê a aceleração, temperatura e giroscópio
 */
void MPU::readAccelTempGyros() {
  	byte vet[14];
  	readBlockData(ACCEL_XOUT_H, vet, 14);
  	axi = (int)((vet[ 0] << 8) | vet[ 1]);    //Montar Acel X
  	ayi = (int)((vet[ 2] << 8) | vet[ 3]);    //Montar Acel Y
  	azi = (int)((vet[ 4] << 8) | vet[ 5]);    //Montar Acel Z
  	tpi = (int)((vet[ 6] << 8) | vet[ 7]);    //Montar Temp
  	gxi = (int)((vet[ 8] << 8) | vet[ 9]);    //Montar Giro X
  	gyi = (int)((vet[10] << 8) | vet[11]);    //Montar Giro Y
  	gzi = (int)((vet[12] << 8) | vet[13]);    //Montar Giro Z
}

/**
 * Faz um self test do MPU
 * @return TRUE se passar do teste, FALSE caso contrário
 */
bool MPU::selfTest() {
	// char msg[100]; 						// 	Descomentar se for imprimir os resultados
  	uint8_t  aux[14];						//	Auxiliar na leitura dos registradores
  	int16_t  gx1, gy1, gz1, ax1, ay1, az1;	//	Valores ST desabilitado
  	int16_t  gx2, gy2, gz2, ax2, ay2, az2;	//	Valores ST habilitado
  	uint8_t  gx3, gy3, gz3, ax3, ay3, az3;	//	Valores reg de Self-Test
  	float    gxf, gyf, gzf, axf, ayf, azf;	//	Factory Trim
  	float    gxr, gyr, gzr, axr, ayr, azr;	//	Alteração em %

 	// Desabilitar Self_Test
	writeRegister(ACCEL_CONFIG, ACEL_FS_8G << 3);	//Self-test desab
	writeRegister(GYRO_CONFIG, GIRO_FS_250 << 3);	//Self-test desab

  	delay(250);  //Aguardar cofiguração estabilizar

	// Ler valores com Self-Test desabilitado.
	readBlockData(ACCEL_XOUT_H, aux, 14);

	ax1 = (int16_t)((aux[0] << 8) | aux[1]);
	ay1 = (int16_t)((aux[2] << 8) | aux[3]);
	az1 = (int16_t)((aux[4] << 8) | aux[5]);
	gx1 = (int16_t)((aux[8] << 8) | aux[9]);
	gy1 = (int16_t)((aux[10] << 8) | aux[11]);
	gz1 = (int16_t)((aux[12] << 8) | aux[13]);

  	// Habilitar Self_Test
	writeRegister(ACCEL_CONFIG, (0xE0 | ACEL_FS_8G << 3)); //Acel hab.
	writeRegister(GYRO_CONFIG, (0xE0 | GIRO_FS_250 << 3)); //Giro hab.

  	delay(250);  //Aguardar cofiguração estabilizar

  	// Ler valores com Self-Test habilitado.
	readBlockData(ACCEL_XOUT_H, aux, 14);

	ax2 = (int16_t)((aux[0] << 8) | aux[1]);
  	ay2 = (int16_t)((aux[2] << 8) | aux[3]);
  	az2 = (int16_t)((aux[4] << 8) | aux[5]);
  	gx2 = (int16_t)((aux[8] << 8) | aux[9]);
  	gy2 = (int16_t)((aux[10] << 8) | aux[11]);
  	gz2 = (int16_t)((aux[12] << 8) | aux[13]);

  	// Leitura dos resultados do self-test
	aux[0] = readRegister(SELF_TEST_X);	//Eixo X: resultado self-test
  	aux[1] = readRegister(SELF_TEST_Y);	//Eixo Y: resultado self-test
	aux[2] = readRegister(SELF_TEST_Z);	//Eixo Z: resultado self-test
	aux[3] = readRegister(SELF_TEST_A);	//Restante dos resultados


  	// Extrair dados do registradores de self-test
  	ax3 = (aux[0] >> 3) | ((aux[3] >> 4) & 3) ; // XA_TEST
  	ay3 = (aux[1] >> 3) | ((aux[3] >> 2) & 3) ; // YA_TEST
  	az3 = (aux[2] >> 3) | (aux[3] & 3);         // ZA_TEST
  	gx3 = aux[0]  & 0x1F ;  					// XG_TEST
  	gy3 = aux[1]  & 0x1F ;  					// YG_TEST
  	gz3 = aux[2]  & 0x1F ;  					// ZG_TEST

  	// Calcular os Factory Trim
  	axf = (4096.0 * 0.34) * (pow((0.92 / 0.34) , (((float)ax3 - 1.0) / 30.0)));
  	ayf = (4096.0 * 0.34) * (pow((0.92 / 0.34) , (((float)ay3 - 1.0) / 30.0)));
  	azf = (4096.0 * 0.34) * (pow((0.92 / 0.34) , (((float)az3 - 1.0) / 30.0)));
  	gxf = ( 25.0 * 131.0) * (pow( 1.046 , ((float)gx3 - 1.0) ));
  	gyf = (-25.0 * 131.0) * (pow( 1.046 , ((float)gy3 - 1.0) ));
  	gzf = ( 25.0 * 131.0) * (pow( 1.046 , ((float)gz3 - 1.0) ));

  	// Se registrador = 0 --> Factory Trim = 0
  	if (ax3 == 0)
		axf = 0;
  	if (ay3 == 0)
		ayf = 0;
  	if (az3 == 0)
		azf = 0;
  	if (gx3 == 0)
		gxf = 0;
  	if (gy3 == 0)
		gyf = 0;
  	if (gz3 == 0)
		gzf = 0;

  	// Calcular as Percentagens de Alteração
  	axr = 100.0 * ((float)(ax2 - ax1) - axf ) / axf;
  	ayr = 100.0 * ((float)(ay2 - ay1) - ayf ) / ayf;
  	azr = 100.0 * ((float)(az2 - az1) - azf ) / azf;
  	gxr = 100.0 * ((float)(gx2 - gx1) - gxf ) / gxf;
  	gyr = 100.0 * ((float)(gy2 - gy1) - gyf ) / gyf;
  	gzr = 100.0 * ((float)(gz2 - gz1) - gzf ) / gzf;

  	// Remova os comentários para imprimir os resultados
   	// Serial.println("Self Test:   OFF      ON    TEST      FT      %");
    // sprintf(msg,"Acel X   :  %+06d  %+06d  %+04d   ",ax1, ax2, ax3);
    // Serial.print(msg);  Serial.print(axf,2);  Serial.print("  ");
    // Serial.println(axr,2);
    // sprintf(msg,"Acel Y   :  %+06d  %+06d  %+04d   ",ay1, ay2, ay3);
    // Serial.print(msg);  Serial.print(ayf,2);  Serial.print("  ");
    // Serial.println(ayr,2);
    // sprintf(msg,"Acel Z   :  %+06d  %+06d  %+04d   ",az1, az2, az3);
    // Serial.print(msg);  Serial.print(azf,2);  Serial.print("  ");
    // Serial.println(azr,2);
    // sprintf(msg,"Giro X   :  %+06d  %+06d  %+04d   ",gx1, gx2, gx3);
    // Serial.print(msg);  Serial.print(gxf,2);  Serial.print("  ");
    // Serial.println(gxr,2);
    // sprintf(msg,"Giro Y   :  %+06d  %+06d  %+04d   ",gy1, gy2, gy3);
    // Serial.print(msg);  Serial.print(gyf,2);  Serial.print("  ");
    // Serial.println(gyr,2);
    // sprintf(msg,"Giro Z   :  %+06d  %+06d  %+04d   ",gz1, gz2, gz3);
    // Serial.print(msg);  Serial.print(gzf,2);  Serial.print("  ");
    // Serial.println(gzr,2);

  	// Tolerância de +/- 14%
  	if (axr < 14 && ayr < 14 && azr < 14 && gxr < 14 && gyr < 14 && gzr < 14)
    	return true;  //Passou no Self-Test
  	else
		return false;  //Falhou no Self-Test
}

/**
 * Calibra o acelerômetro e o giroscópio do MPU
 * @param bias
 * @param valor
 */
void MPU::calibrate(int16_t *bias, float  *valor) { //TODO:trocar nome
	// char msg[100]; 	//	Descomentar se for imprimir valores de offset
  	uint8_t data[14];   //	Receber dados do acel e giro
  	int32_t abx = 0, aby = 0, abz = 0, gbx = 0, gby = 0, gbz = 0; //	Somatório erro acel e giro


  	// Resetar MPU, o Reset se apaga sozinho
  	// Reset ==> registradores=0, exceto PWR_MGMT=0x40
  	// Seleciona automaticamente  escalas 2g e 250gr/s
	writeRegister(PWR_MGMT_1, 0x80);
  	delay(100); //Esperar terminar o Reset

	// Sair do modo sleep e selecionar Relógio = PLL do Giro-x
	writeRegister(PWR_MGMT_1, 0x01);
  	delay(200); //Esperar estabilizar

  	// Selecionar a escala desejada para a calibração
  	// Função atualiza as variáveis acel_res e giro_res
	setScale(GIRO_FS_250, ACEL_FS_2G);

  	// Preparar MPU para cálculo do erro do acelerômetro e do giroscópio
	writeRegister(CONFIG, 0x01);		//BW_Acel=184Hz e BW_Giro=188 Hz
	writeRegister(SMPLRT_DIV, 0x00);	//Taxa de amostragem em 1 kHz

  	// Habilitar bit DATA_RDY_INT para indicar dado pronto
	writeRegister(INT_ENABLE, 0x01);	//Será usado polling
	readRegister(INT_STATUS);			//Leitura para apagar o bit

  	// Laço principal: somatório de 256 leituras
  	for (uint16_t j = 0; j < 256; j++) {
    	while ((readRegister(INT_STATUS) & 1) ==  0)
			delay(1);

    	readBlockData(ACCEL_XOUT_H, data, 14);
    	abx += (int32_t) (((int16_t)data[0] << 8) | data[1]);
    	aby += (int32_t) (((int16_t)data[2] << 8) | data[3]);
    	abz += (int32_t) (((int16_t)data[4] << 8) | data[5]);
    	gbx += (int32_t) (((int16_t)data[8] << 8) | data[9]);
    	gby += (int32_t) (((int16_t)data[10] << 8) | data[11]);
    	gbz += (int32_t) (((int16_t)data[12] << 8) | data[13]);
  	}

  	// Calcular média (>>8 corresponde a dividir por 256) e transferir para a variável de saída
  	bias[0] = (int16_t) (abx >> 8); //acel X
  	bias[1] = (int16_t) (aby >> 8); //acel Y
  	bias[2] = (int16_t) (abz >> 8); //acel Z
  	bias[3] = (int16_t) (gbx >> 8); //giro X
  	bias[4] = (int16_t) (gby >> 8); //giro Y
  	bias[5] = (int16_t) (gbz >> 8); //giro Z

  	// Calcular o erro (bias) em valores de g e graus/s
  	valor[0] = bias[0] * acel_res; //acel X
  	valor[1] = bias[1] * acel_res; //acel Y
  	valor[2] = bias[2] * acel_res; //acel Z
  	valor[3] = bias[3] * giro_res; //giro X
  	valor[4] = bias[4] * giro_res; //giro Y
  	valor[5] = bias[5] * giro_res; //giro Z

  	// Remova os comentário para imprimir os valores de offset
  	// Serial.println("Resultados da Calibracao:");
    // sprintf(msg,"Acel X: %+06X --> ",bias[0]);  Serial.print(msg);
    // Serial.print(valor[0],2); Serial.println(" g");
    // sprintf(msg,"Acel Y: %+06X --> ",bias[1]);  Serial.print(msg);
    // Serial.print(valor[1],2); Serial.println(" g");
    // sprintf(msg,"Acel Z: %+06X --> ",bias[2]);  Serial.print(msg);
    // Serial.print(valor[2],2); Serial.println(" g");
    // sprintf(msg,"Giro X: %+06X --> ",bias[3]);  Serial.print(msg);
    // Serial.print(valor[3],2); Serial.println(" graus/s");
    // sprintf(msg,"Giro Y: %+06X --> ",bias[4]);  Serial.print(msg);
    // Serial.print(valor[4],2); Serial.println(" graus/s");
    // sprintf(msg,"Giro Z: %+06X --> ",bias[5]);  Serial.print(msg);
    // Serial.print(valor[5],2); Serial.println(" graus/s");
}
