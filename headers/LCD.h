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

	/**
	 * Define direção dos pinos e inicializa LCD
	 */
	void initPins();

	/**
	 * Modifica RS e RW
	 * @param rs TRUE se RS = 1, FALSE se RS = 0
	 @ @param rw TRUE se RW = 1, FALSE se RW = 0
	 */
	void setRsAndRw(bool rs, bool rw);

	/**
	 * Modifica o sentido do barramento. Como saída, zera os valores nos pinos.
	 * Como entrada, coloca um resistor de pull up.
	 * @param direction OUTPUT para deixar pinos como saída e INPUT para deixar pinos
	 * como entrada
	 */
	void setBusMode(byte direction);

	/**
	 * Lê um byte do LCD
	 * @return byte que foi lido
	 */
	byte readByte();

	/**
	 * Lê um nibble do LCD (xxxx 0000)
	 * @return nibble que foi lido
	 */
	byte readNibble();

	/**
	 * Envia um byte para o lcd
	 * @param dado byte que vai ser enviado
	 */
	void writeByte(byte dado);

	/**
	 * Envia um nibble e pulsa E
	 * @param nib nibble que vai ser enviado
	 */
	void writeNibble(byte nib);

public:

	byte lcd_buffer[4][20];  /**< Buffer para o LCD [lin][col] terminar cada linha com Zero */
	volatile bool lcd_flag0;  /**< Flag para indicar que a linha 0 deve atualizar */
	volatile bool lcd_flag1; /**< Flag para indicar que a linha 1 deve atualizar */

	/**
	 * Construtor da classe.
	 * Inicia os pinos.
	 */
	LCD();

	/**
	 * Configura o LCD para começar o uso
	 */
	void startConfig();

	/**
	 * Cria um caracterer especial
	 * @param code código do caracter
	 * @param vet vetor de 8 posições com o mapa de pontos
	 */
	void caracter(byte code, byte *vet);

	/**
	 * Imprime um decimal de 16 bits
	 * @param nr
	 * @param lin linha de início
	 * @param col coluna de início
	 */
	void printDec16(word nr, byte lin, byte col);

	/**
	 * Imprime um decimal de 8 bits
	 * @param nr
	 * @param lin linha de início
	 * @param col coluna de início
	 */
	void printDec8(byte nr, byte lin, byte col);

	/**
	 * Imprime um hexadecimal de 16 bits
	 * @param nr
	 * @param lin linha de início
	 * @param col coluna de início
	 */
	void printHex16(word nr, byte lin, byte col);

	/**
	 * Imprime um hexadecimal de 8 bits
	 * @param nr
	 * @param lin linha de início
	 * @param col coluna de início
	 */
	void printHex8(byte nr, byte lin, byte col);

	/**
	 * Imprime uma letra em 8 bits
	 * @param nr
	 * @param lin linha de início
	 * @param col coluna de início
	 */
	void printChar8(byte nr, byte lin, byte col);

	/**
	 * Trasferir uma string para o lcd_buffer.
	 * Transfere até encontrar o "\0"
	 * @param vet ponteiro para string
	 * @param lin linha de início
	 * @param col coluna de início
	 */
	void loadBuffer(byte *vet, byte lin, byte col);

	/**
	 * Trasferir do lcd_buffer para o LCD.
	 * Só transfere uma linha
	 * @param lin linha de início
	 * @param coli coluna de início
	 * @param colf coluna final (inclusa)
	 */
	void loadLCD(byte lin, byte coli, byte colf);

	/**
	 * Colocar branco (0x20) em todo o buffer do LCD
	 */
	void loadBufferWhite();

	/**
	 * Liga e desliga o cursor piscante
	 * @param on ON para ligar o cursor e OFF para desligar o cursor
	 */
	void cursor(byte on);

	/**
	 * Escrever um comando (instrução) no LCD
	 * @param dado instrução
	 */
	void command(byte dado);

	/**
	 * Escrever uma caracter no LCD
	 * @param dado caracter
	 */
	void printChar(byte dado);

	/**
	 * Posicionar o cursor
	 * @param pos posição
	 */
	void setCursor(byte pos);

	/**
	 * Ler contador de endereços
	 * @return Contador de endereços
	 */
	byte readAddressCounter();

	/**
	 * Verifica se o LCD está ocupado
	 * @return TRUE se tiver ocupado e FALSE se estiver livre
	 */
	bool isBusy();

	/**
	 * Acende e apaga o Back Light
	 * @param on TRUE para ligar o back light e FALSE para desligar o back light
	 */
	void backLight(bool on);

};

#endif
