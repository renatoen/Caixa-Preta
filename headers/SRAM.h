// #ifndef SRAM_h
// #define SRAM_h
//
// #include <Arduino.h>
// #include <SPI.h>
//
// /*---------------------- SRAM -----------------------*/
// //Conjunto de instrucoes para a memoria 23LC1024
// #define RDMR 5
// #define WRMR 1
// #define BYMD 0x00           // Byte mode
// #define PGMD 0x80           // Page mode
// #define SQMD 0x40           // Sequential mode
// #define READ 3
// #define WRTE 2
//
// #define MODO_SEQ 0x40 // Registrador de modo - Modo sequencial
// #define CSpin 48
//
// class SRAM {
//     public:
//       void setMode(uint8_t mode);
//       uint8_t getMode();
//       void writeSeq(uint32_t addr, int16_t *buff, uint16_t qtd);
//       void readSeq(uint32_t addr, int16_t *buff, uint32_t numBytes);
//
//     private:
//
// };
//
// #endif
