#include "../headers/Timer5.h"
#include "../headers/Leds.h"
#include "../headers/MPU.h"
#include "../headers/USART.h"

LCD screen;
Leds leds;
Button key;

void setup() {
	int i;
	// Serial.begin(115200);
	// LCD
	screen.startConfig();
	screen.cursor(OFF);

	// Timer
	Timer5 timer = Timer5(&screen, &key);

	//USART
	USART monitor(0, 115200);

	// MPU
	MPU teste(55);
	teste.wakeUp();

	i = teste.whoAmI();

	if (i == 0x73) {
		screen.loadBuffer("MPU OK", 0, 0);
	} else {
		screen.loadBuffer("MPU NOK", 0, 0);
	}

	while (true) {
		teste.readAccelTempGyros();
		// Serial.print(teste.axi);      Serial.print("  ");
      // Serial.print(teste.ayi);      Serial.print("  ");
      // Serial.print(teste.azi);      Serial.print(" | ");
      // Serial.print(teste.gxi);      Serial.print("  ");
      // Serial.print(teste.gyi);      Serial.print("  ");
      // Serial.println(teste.gzi);
		monitor.print(teste.axi);
		monitor.print("  ");
      	monitor.print(teste.ayi);
		monitor.print("  ");
      	monitor.print(teste.azi);
		monitor.print(" | ");
      	monitor.print(teste.gxi);
		monitor.print("  ");
      	monitor.print(teste.gyi);
		monitor.print("  ");
      	monitor.println(teste.gzi);
      	delay(500);
	}
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
