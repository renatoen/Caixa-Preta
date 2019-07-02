#include "../headers/Firmware.h"

LCD screen;
Button key;
Firmware firm;

int menuSelection = 0;
byte tecla;
bool changeScreen = true;

void setup() {
	// LCD
	screen.startConfig();
	screen.cursor(OFF);

	// Timer
	Timer5 timer = Timer5(&screen, &key);

	//Firmware
	firm = Firmware(&screen, &key);
}

void loop() {

	if (changeScreen) {
		screen.loadBufferWhite();

		switch (menuSelection) {
			case 0:
				screen.loadBuffer("Testar MPU",0,0);
				break;
			case 1:
				screen.loadBuffer("Testar GPS",0,0);
				break;
			case 2:
				screen.loadBuffer("Rodar Rotina",0,0);
				break;
			case 3:
				screen.loadBuffer("Ler dados brutos",0,0);
				break;
			case 4:
				screen.loadBuffer("Tratar dados",0,0);
				break;
			// case 5:
			// 	screen.loadBuffer("Ler dados",0,0);
			// 	screen.loadBuffer("tratados",1,0);
			// 	break;
			default:
				break;
		}
		changeScreen = false;
	}

	if (key.readBuffer(&tecla) == true) {
	 	switch(tecla){
	   		case SW_SEL:
				switch (menuSelection) {
					case 0: // Testar MPU
						screen.loadBufferWhite();
						screen.loadBuffer("Testando MPU...",0,0);
						firm.testMPU();
						break;
					case 1: // Testar GPS
						screen.loadBufferWhite();
						screen.loadBuffer("Testando GPS...",0,0);
						firm.testGPS();
						break;
					case 2: // Rodar Rotina
						firm.rotina();
						break;
					case 3: // Ler dados brutos
						screen.loadBufferWhite();
						screen.loadBuffer("Lendo dados",0,0);
						screen.loadBuffer("brutos...",1,0);
						firm.readData();
						break;
					case 4: // Tratar dados
						screen.loadBufferWhite();
						screen.loadBuffer("Tratando Dados",0,0);
						firm.processData();
						break;
					// case 5: // Ler dados tratados
					// 	screen.loadBufferWhite();
					// 	screen.loadBuffer("Lendo dados",0,0);
					// 	screen.loadBuffer("tratados...",1,0);
					// 	firm.readProcessedData();
					// 	break;
					default:
						break;
				}
				changeScreen = true;
				break;
	   		case SW_CIMA:
				if (--menuSelection == -1) {
					menuSelection = 4;
				}
				changeScreen = true;
				break;
	   		case SW_BAIXO:
				if (++menuSelection == 5) {
					menuSelection = 0;
				}
				changeScreen = true;
				break;
			default:
				break;
	 	}
	}
}
