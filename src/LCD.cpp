#include "../headers/LCD.h"

/**
 * Construtor da classe.
 * Inicia os pinos.
 */
LCD::LCD() {
	initPins();
}

/**
 * Configura o LCD para começar o uso
 */
void LCD::startConfig() {
	PORTC &= ~( (1<<LCD_BIT_RW)|(1<<LCD_BIT_RS) );  //RW=0 e RS=0
  	delay(15);

  	writeNibble(3);
  	delay(5);

  	writeNibble(3);
  	delay(1);

  	writeNibble(3);
  	delay(1);

  	writeNibble(2);

  	// Entrou modo 4 bits
  	writeByte(0x28);
  	delay(1);

  	writeByte(8);
  	delay(1);

  	writeByte(1);
  	delay(1);

  	writeByte(6);
  	delay(1);

  	writeByte(0xf);
  	delay(1);

	// limpa lcd
	loadBufferWhite();
}

/**
 * Cria um caracterer especial
 * @param code código do caracter
 * @param vet vetor de 8 posições com o mapa de pontos
 */
void LCD::caracter(byte code, byte *vet) {
	int j, i;

  	command(0x40+(code<<3));

  	j = 0x40+(code<<3);

  	for (i = 0; i < 8; i++)
    	printChar(vet[i]);
  	setCursor(10);
}

/**
 * Imprime um decimal de 16 bits
 * @param nr
 * @param lin linha de início
 * @param col coluna de início
 */
void LCD::printDec16(word nr, byte lin, byte col) {
  	byte msg[6],aux,i;
  	word dv=10000;

  	for (i = 0; i < 5; i++){
    	aux = nr/dv;
    	msg[i] = 0x30+aux;
    	nr = nr-(dv*aux);
    	dv = dv/10;
  	}

  	msg[i] = 0;
  	loadBuffer(msg, lin, col);
}

/**
 * Imprime um decimal de 8 bits
 * @param nr
 * @param lin linha de início
 * @param col coluna de início
 */
void LCD::printDec8(byte nr, byte lin, byte col) {
  	byte msg[4],aux,i;
  	word dv = 100;

  	for (i = 0; i < 3; i++){
    	aux = nr/dv;
    	msg[i] = 0x30+aux;
    	nr = nr-(dv*aux);
    	dv = dv/10;
  	}

  	msg[i] = 0;
  	loadBuffer(msg, lin, col);
}

/**
 * Imprime um hexadecimal de 16 bits
 * @param nr
 * @param lin linha de início
 * @param col coluna de início
 */
void LCD::printHex16(word nr, byte lin, byte col) {
  	byte msg[3],aux,i;

  	for (i = 0; i < 4; i++){
    	aux = nr&0xf;
    	if (aux < 10)
			msg[3-i] = aux + 0x30;
    	else
			msg[3-i] = aux + 0x37;
    	nr = nr>>4;
  	}

  	msg[i] = 0;
  	loadBuffer(msg, lin, col);
}


/**
 * Imprime um hexadecimal de 8 bits
 * @param nr
 * @param lin linha de início
 * @param col coluna de início
 */
void LCD::printHex8(byte nr, byte lin, byte col) {
  	byte msg[3],aux,i;
  	for (i = 0; i < 2; i++){
    	aux = nr&0xf;
    	if (aux < 10)
			msg[1-i] = aux + 0x30;
    	else
			msg[1-i] = aux + 0x37;
    	nr = nr>>4;
  	}

  	msg[i] = 0;
  	loadBuffer(msg, lin, col);
}

/**
 * Imprime uma letra em 8 bits
 * @param nr
 * @param lin linha de início
 * @param col coluna de início
 */
void LCD::printChar8(byte nr, byte lin, byte col) {
  	byte msg[2];
  	msg[0] = nr;
  	msg[1] = 0;
  	loadBuffer(msg, lin, col);
}

/**
 * Trasferir uma string para o lcd_buffer.
 * Transfere até encontrar o "\0"
 * @param vet ponteiro para string
 * @param lin linha de início
 * @param col coluna de início
 */
void LCD::loadBuffer(byte *vet, byte lin, byte col) {
  	byte j;
  	switch(lin){
    	case 0: lcd_flag0 = true;  break;
    	case 1: lcd_flag1 = true;  break;
    	default:  break;
  	}
  	j = 0;
  	while(vet[j] != 0x0){
    	lcd_buffer[lin][col+j] = vet[j];
    	j++;
  	}
}

/**
 * Trasferir do lcd_buffer para o LCD.
 * Só transfere uma linha
 * @param lin linha de início
 * @param coli coluna de início
 * @param colf coluna final (inclusa)
 */
void LCD::loadLCD(byte lin, byte coli, byte colf) {
  	byte j;
  	switch(lin){
    	case 0: setCursor(coli);          break;
    	case 1: setCursor(coli+0x40);     break;
    	case 2: setCursor(coli+20);       break;
    	case 3: setCursor(coli+0x40+20);  break;
    	default:  break;
  	}
  	for (j = coli; j < colf+1; j++)
    	printChar(lcd_buffer[lin][j]);
}

/**
 * Colocar branco (0x20) em todo o buffer do LCD
 */
void LCD::loadBufferWhite() {
  	byte lin,col;
  	for (lin = 0; lin < LCD_LIN; lin++)
    	for (col = 0; col < LCD_COL; col++)
      		lcd_buffer[lin][col] = 0x20;

  	lcd_flag0 = lcd_flag1 = true;
}

/**
 * Liga e desliga o cursor piscante
 * @param on ON para ligar o cursor e OFF para desligar o cursor
 */
void LCD::cursor(byte on) {
	if (on == ON) {
		command(0xf);
	} else if (on == OFF) {
		command(0xc);
	}
}

/**
 * Escrever um comando (instruçao) no LCD
 * @param dado instrução
 */
void LCD::command(byte dado) {
  	while (isBusy() == true);  //LCD Ocupado?
	setRsAndRw(false, false);
  	writeByte(dado);
}

/**
 * Escrever uma caracter no LCD
 * @param dado caracter
 */
void LCD::printChar(byte dado) {
  	while (isBusy() == true);  //LCD Ocupado?
	setRsAndRw(true, false);
  	writeByte(dado);
}

/**
 * Posicionar o cursor
 * @param pos posição
 */
void LCD::setCursor(byte pos) {
  	while (isBusy() == true);  //LCD Ocupado?
  	setRsAndRw(false, false);
  	writeByte(0x80|pos);
}

/**
 * Ler contador de endereços
 * @return Contador de endereços
 */
byte LCD::readAddressCounter() {
  	byte dado;
	setRsAndRw(false, true);
  	setBusMode(INPUT);      //LCD Bus para entrada
  	dado = readByte();
  	setBusMode(OUTPUT);     //LCD Bus para saída
  	return dado&0x7F;
}

/**
 * Verifica se o LCD está ocupado
 * @return TRUE se tiver ocupado e FALSE se estiver livre
 */
bool LCD::isBusy() {
  	byte dado;
  	setRsAndRw(false, true);     //RS=0 e RW=1
  	setBusMode(INPUT);      //LCD Bus para entrada
  	dado = readByte();
  	setBusMode(OUTPUT);     //LCD Bus para saída
  	if ( (dado&0x80) == 0)
		return false;
  	else
		return true;
}

/**
 * Lê um byte do LCD
 * @return byte que foi lido
 */
byte LCD::readByte() {
  	byte aux;
  	aux = readNibble();
  	aux |= readNibble()>>4;
  	return aux;
}

/**
 * Lê um nibble do LCD (xxxx 0000)
 * @return nibble que foi lido
 */
byte LCD::readNibble() {
  	byte aux;
  	PORTC |= 1<<LCD_BIT_E;   //E=1
  	delayMicroseconds(2);    //Precisa desse delay
  	aux = PINA&0xF0;
  	PORTC &= ~(1<<LCD_BIT_E);  //E=0
  	return aux;
}

/**
 * Envia um byte para o lcd
 * @param dado byte que vai ser enviado
 */
void LCD::writeByte(byte dado) {
  	//Serial.print("wr_byte recebeu = ");
  	//Serial.println(dado,HEX);
  	writeNibble((dado>>4)&0xF);
  	writeNibble(dado&0xF);
}

/**
 * Envia um nibble e pulsa E
 * @param nib nibble que vai ser enviado
 */
void LCD::writeNibble(byte nib) {
  	PORTA &= ~((1<<LCD_BIT_D4)|(1<<LCD_BIT_D5)|(1<<LCD_BIT_D6)|(1<<LCD_BIT_D7)); //Zerar
  	PORTA |= nib<<4;
  	PORTC |= 1<<LCD_BIT_E;   //E=1
  	PORTC &= ~(1<<LCD_BIT_E);  //E=0
}

/**
 * Modifica RS e RW
 * @param rs TRUE se RS = 1, FALSE se RS = 0
 @ @param rw TRUE se RW = 1, FALSE se RW = 0
 */
void LCD::setRsAndRw(bool rs, bool rw) {
	if (rs == true) {
		if (rw == true) {
			PORTC |= (1<<LCD_BIT_RW) | (1<<LCD_BIT_RS);  //RS=RW=1
		} else if (rw == false) {
			PORTC |= (1<<LCD_BIT_RS);   //RS=1
		  	PORTC &= ~(1<<LCD_BIT_RW);  //RW=0
		}
	} else if (rs == false) {
		if (rw == true) {
			PORTC &= ~(1<<LCD_BIT_RS);  //RS=0
		  	PORTC |= (1<<LCD_BIT_RW);   //RW=1
		} else if (rw == false) {
			PORTC &= ~( (1<<LCD_BIT_RW) | (1<<LCD_BIT_RS) ) ;  //RS=RW=0
		}
	}
}

/**
 * Acende e apaga o Back Light
 * @param on TRUE para ligar o back light e FALSE para desligar o back light
 */
void LCD::backLight(bool on) {
	if (on == true) {
		PORTC |= (1<<LCD_BIT_BL);   //BL=1
	} else if (on == false) {
		PORTC &= ~(1<<LCD_BIT_BL);   //BL=0
	}
}

/**
 * Define direção dos pinos e inicializa LCD
 */
void LCD::initPins() {
  	DDRC |= (1<<LCD_BIT_BL)|(1<<LCD_BIT_RW)|(1<<LCD_BIT_RS)|(1<<LCD_BIT_E);      //Saídas
  	PORTC &= ~((1<<LCD_BIT_BL)|(1<<LCD_BIT_RW)|(1<<LCD_BIT_RS)|(1<<LCD_BIT_E));  //Zerar
  	DDRA |= (1<<LCD_BIT_D4)|(1<<LCD_BIT_D5)|(1<<LCD_BIT_D6)|(1<<LCD_BIT_D7);     //Saídas
  	PORTA &= ~((1<<LCD_BIT_D4)|(1<<LCD_BIT_D5)|(1<<LCD_BIT_D6)|(1<<LCD_BIT_D7)); //Zerar
}

/**
 * Modifica o sentido do barramento. Como saída, zera os valores nos pinos.
 * Como entrada, coloca um resistor de pull up.
 * @param direction OUTPUT para deixar pinos como saída e INPUT para deixar pinos
 * como entrada
 */
void LCD::setBusMode(byte direction) {
	if (direction == OUTPUT) {
		DDRA |= (1<<LCD_BIT_D4)|(1<<LCD_BIT_D5)|(1<<LCD_BIT_D6)|(1<<LCD_BIT_D7);     //Saídas
	  	PORTA &= ~((1<<LCD_BIT_D4)|(1<<LCD_BIT_D5)|(1<<LCD_BIT_D6)|(1<<LCD_BIT_D7)); //Zerar
	} else if (direction == INPUT) {
		DDRA &= ~((1<<LCD_BIT_D4)|(1<<LCD_BIT_D5)|(1<<LCD_BIT_D6)|(1<<LCD_BIT_D7));  //Entradas
	  	PORTA |= (1<<LCD_BIT_D4)|(1<<LCD_BIT_D5)|(1<<LCD_BIT_D6)|(1<<LCD_BIT_D7);    //Pull up
	}

}
