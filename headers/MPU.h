#ifndef MPU_H
#define MPU_H

// Includes
#include <Arduino.h>

// Defines

//////////// 24LC512 ou 24LC1025 - Flash externa
// Mudar nomes para FLASH ...
#define M24LC_ADR 0x50    //Endereço da 24lC512 ou 24LC1025 (Limitar em 64 KB)
#define M24LC_EWR 0xA0    //24LC para escrita (0x50<<1)
#define M24LC_ERD 0xA1    //24LC para leitura (0x50<<1 + 1)
#define FLASH_ADR_LIM   65536L  //Limitar uso em 64 KB
#define FLASH_PAG   128   //Tamanho da págnia para gravação


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

/////////////// TWI - Códigos de Status
#define TWI_START_OK      8     // Start OK
#define TWI_START_REP_OK  0x10  // Start Repetido OK
#define TWI_SLA_WR_ACK    0x18  // EET enviado e ACK recebido
#define TWI_SLA_WR_NACK   0x20  // EET enviado e NACK recebido
#define TWI_TX_DATA_ACK   0x28  // Dado enviado e ACK recebido
#define TWI_SLA_RD_ACK    0x40  // EER enviado e ACK recebido
#define TWI_SLA_RD_NACK   0x48  // EER enviar e NACK recebido
#define TWI_RX_DATA_NACK  0x58  // Dado recebido e NACK gerado
#define TWI_RX_DATA_ACK   0x50  // Dado recebido e ACK gerado
#define TWI_TMI_OUT       10000 // Time out

#define TWI_ERRO_1  1   // Erro ao gerar START
#define TWI_ERRO_2  2   // Erro ao gerar START Repetido
#define TWI_ERRO_3  3   // Erro Escravo Receptor endereçado (ER) não enviou ACK
#define TWI_ERRO_4  4   // Erro Escravo Transmissor endereçado (ET) não enviou ACK
#define TWI_ERRO_5  5   // Erro Escravo Receptor (ER) não enviou ACK após envio do dado
#define TWI_ERRO_6  6   // Erro ao receber um dado do Escravo Transmissor (ET) e gerar um ACK
#define TWI_ERRO_7  7   // Erro ao receber um dado do Escravo Transmissor (ET) e gerar um NACK
#define TWI_ERRO_8  8   // Erro ao esperar TWINT - Timeout esperando TWINT ir para 1

class MPU {
private:

public:
	MPU();

};

#endif
