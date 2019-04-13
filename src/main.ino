#include "../headers/Timer5.h"
#include "../headers/Leds.h"
#include "../headers/MPU.h"
#include "../headers/USART.h"
#include "../headers/TinyGPS++.h"

LCD screen;
Leds leds;
Button key;
TinyGPSPlus gps;
USART monitor, monitor3;
MPU mpu;

void setup() {
	int i;
	// LCD
	screen.startConfig();
	screen.cursor(OFF);

	// Timer
	Timer5 timer = Timer5(&screen, &key);

	//USART
	monitor = USART(0, 115200);
	monitor3 = USART(3, 9600);

	// MPU
	mpu = MPU(55);
	mpu.wakeUp();

	i = mpu.whoAmI();

	if (i == 0x73) {
		screen.loadBuffer("MPU OK", 0, 0);
	} else {
		screen.loadBuffer("MPU NOK", 0, 0);
	}
}

void loop() {
	byte msg[50], tecla;
	int cont = 0;

	while (true) {
		// mpu.readAccelTempGyros();
		// monitor.print(mpu.axi);
		// monitor.print("  ");
		// monitor.print(mpu.ayi);
		// monitor.print("  ");
		// monitor.print(mpu.azi);
		// monitor.print(" | ");
		// monitor.print(mpu.gxi);
		// monitor.print("  ");
		// monitor.print(mpu.gyi);
		// monitor.print("  ");
		// monitor.println(mpu.gzi);
		char c = monitor3.readByte();

		if(gps.encode(c)) {
	        if(gps.location.isValid() && gps.date.isValid() && gps.time.isValid() && gps.speed.isValid()) {
	        	monitor.print("Localização: "); monitor.print(gps.location.lat()); monitor.print(", "); monitor.println(gps.location.lng());
	          	monitor.print("Velocidade: "); monitor.print(gps.speed.kmph()); monitor.println("km/h");

	          	monitor.print("Data: "); monitor.print(gps.date.day()); monitor.print("/"); monitor.print(gps.date.month()); monitor.print("/"); monitor.println(gps.date.year());
	          	monitor.print("Hora: "); monitor.print(gps.time.hour()); monitor.print(":"); monitor.print(gps.time.minute()); monitor.print(":"); monitor.println(gps.time.second());
	        }
        	delay(500);
      	}
	}

	if(millis() > 5000 && gps.charsProcessed() < 10) {
      	monitor.println("No GPS detected: check wiring.");
      	while(true);
    }

	// while(1) {
	// 	sprintf(msg,"-%03d-",cont);
	// 	screen.loadBuffer(msg, 1, 0);
	// 	if (cont++ == 999)
	// 		cont = 0;
	// 	delay(1000);
	// 	leds.green(TOGGLE);
	// 	leds.red(TOGGLE);
	// 	leds.yellow(TOGGLE);
	// }

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
