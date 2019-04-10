// #include "../headers/SRAM.h"
//
// /*---------------- SRAM ----------------*/
// // Ler registrador de modo
// uint8_t SRAM::getMode()
// {
//     uint8_t mode;
//     digitalWrite(CSpin, LOW);
//     delay(5);
//     SPI.transfer(RDMR);
//     mode = SPI.transfer(0x00);
//     digitalWrite(CSpin, HIGH);
//     return mode;
// }
//
// void SRAM::setMode(uint8_t mode)
// {
//     digitalWrite(CSpin, LOW); // Selecionar SRAM
//     SPI.transfer(WRMR);       // Indicar escrita no Reg de modo
//     SPI.transfer(mode);       // Escrever o modo
//     digitalWrite(CSpin, HIGH);
// }
//
// void SRAM::writeSeq(uint32_t addr, int16_t *buff, uint16_t qtd)
// {
//     setMode(SQMD);
//     digitalWrite(CSpin, LOW); // #CS = 0 antes de cada transação
//     SPI.transfer(WRTE);
//     SPI.transfer((uint8_t)(addr >> 16));
//     SPI.transfer((uint8_t)(addr >> 8));
//     SPI.transfer((uint8_t)addr);
//
//     for (uint16_t i = 0; i < qtd; i++)
//     {
//         SPI.transfer((uint8_t)(buff[i] >> 8));
//         SPI.transfer((uint8_t)buff[i]);
//     }
//
//     digitalWrite(CSpin, HIGH); // #CS = 1 ao final de cada transacao
// }
//
// void SRAM::readSeq(uint32_t addr, int16_t *buff, uint32_t numBytes)
// {
//     setMode(SQMD);
//     digitalWrite(CSpin, LOW);
//     SPI.transfer(READ);
//     SPI.transfer((uint8_t)(addr >> 16));
//     SPI.transfer((uint8_t)(addr >> 8));
//     SPI.transfer((uint8_t)addr);
//
//     for (uint16_t i = 0; i < numBytes; i++)
//     {
//         buff[i] = (int16_t)(SPI.transfer(0x00) << 8) | SPI.transfer(0x00);
//         // reads two bytes sequentially to form a signed 16bit number
//     }
//
//     digitalWrite(CSpin, HIGH);
// }
