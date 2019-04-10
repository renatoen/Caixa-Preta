#include "../headers/Timer5.h"
#include "../headers/Leds.h"

LCD screen;
Leds leds;
Button key;

void setup() {
	Serial.begin(115200);
	// LCD
	screen.startConfig();
	screen.cursor(OFF);

	// Timer
	Timer5 timer = Timer5(&screen, &key);
}

void loop() {
	byte msg[50], tecla;
	int cont = 0;
	delay(1000);

	while(1) {
		sprintf(msg,"-%03d-",cont);
		screen.loadBuffer(msg, 1, 0);
		if (cont++ == 999)
			cont = 0;
		delay(1000);
		leds.green(TOGGLE);
		leds.red(TOGGLE);
		leds.yellow(TOGGLE);
	}

	// while(TRUE){
   	// 	if (key.readBuffer(&tecla) == TRUE){
    //  		// cont++;
    //  		// lcd_dec16(cont,1,0);
    //  		// lcd_hex8(tecla,1,10);
	// 		screen.loadBufferWhite();
    //  		switch(tecla){
    //    			case SW_SEL:
	// 				screen.loadBuffer("Seleciona",0,0);
	// 				break;
    //    			case SW_ESQ:
	// 				screen.loadBuffer("Esquerda",0,0);
	// 				break;
    //    			case SW_DIR:
	// 				screen.loadBuffer("Direita",0,0);
	// 				break;
    //    			case SW_CIMA:
	// 				screen.loadBuffer("Cima",0,0);
	// 				break;
    //    			case SW_BAIXO:
	// 				screen.loadBuffer("Baixo",0,0);
	// 				break;
    //  		}
   	// 	}
 	// }
}
