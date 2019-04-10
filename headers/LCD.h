#ifndef LCD_H
#define LCD_H

// Includes
#include <Arduino.h>

// Defines

#define ON 1
#define OFF 0

#define  LCD_LIN  2  // LCD com 2 linhas
#define  LCD_COL  20 // LCD com 20 colunas

// Conexão do barramento do LCD com o Arduito
#define LCD_BIT_E   0  //PC0 - Pino 37
#define LCD_BIT_RS  1  //PC1 - Pino 36
#define LCD_BIT_RW  2  //PC2 - Pino 35
#define LCD_BIT_BL  3  //PC3 - Pino 34
#define LCD_BIT_D4  4  //PA4 - Pino 26
#define LCD_BIT_D5  5  //PA5 - Pino 27
#define LCD_BIT_D6  6  //PA6 - Pino 28
#define LCD_BIT_D7  7  //PA7 - Pino 29


#define  BRANCO  BR;  // Caracter branco

#define CH  1     //Cheio
#define MS  2     //Meio Superior
#define MI  3     //Meio Inferiior
#define QS  4     //Meio Superior
#define QI  5     //Meio Inferiior
#define VG  6     //Vírgula
#define BR  0x20  //Branco

class LCD {
private:

	// métodos
	void initPins();
	void setRsAndRw(bool rs, bool rw);
	void setBusMode(byte direction);
	byte readByte();
	byte readNibble();
	void writeByte(byte dado);
	void writeNibble(byte nib);

public:

	// variáveis
	byte lcd_buffer[4][20];  //Buffer para o LCD [lin][col] terminar cada linha com Zero
	volatile bool lcd_flag0,lcd_flag1,lcd_flag2,lcd_flag3;  //Flags para indicar qual linha atualizar

	// init
	LCD();

	// métodos
	void startConfig();
	void caracter(byte code, byte *vet);
	void printDec16(word nr, byte lin, byte col);
	void printDec8(byte nr, byte lin, byte col);
	void printHex16(word nr, byte lin, byte col);
	void printHex8(byte nr, byte lin, byte col);
	void printChar8(byte nr, byte lin, byte col);
	void loadBuffer(byte *vet, byte lin, byte col);
	void loadLCD(byte lin, byte coli, byte colf);
	void loadBufferWhite();
	void cursor(byte on);
	void command(byte dado);
	void printChar(byte dado);
	void setCursor(byte pos);
	byte readAddressCounter();
	bool isBusy();
	void backLight(bool on);

};

#endif
