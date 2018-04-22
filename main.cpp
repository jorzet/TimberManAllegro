#include <allegro.h>
#include "winalleg.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>
using namespace std;

#define ANCHOPANTALLA 350
#define ALTOPANTALLA 500
#include "SerialClass.h"

/* -------------- Global variables -------------- */

/* 
 * UI accesors 
 */
PALETTE pal;
BITMAP *personaje[10];
BITMAP *estado[10];
BITMAP *presentacion,*menu[10];
BITMAP *barra;
BITMAP *carga;
BITMAP *tap[2];
BITMAP *pause[2];
BITMAP *fragmentoSuelo;
BITMAP *pantallaOculta;
BITMAP *cursor;
BITMAP *tronco;
BITMAP *rama[4];
BITMAP *arbol[6][6];
BITMAP *shop;
BITMAP *selectt[2];
BITMAP *before[2];
BITMAP *after[2];
BITMAP *gameOver;
BITMAP *rip;

//HANDLE t_personaje;

int x_menu = 415;
int valor;
int flag;
int var = 0;
int partidaTerminada;
int x_izq = 15;
int x_der = 240;
int x_arb = 15;
int y = 300;
int a;//variable rand
int pos_x[5];
int pos_y[5];
int incrX = 4;
int incrY = 4;
int tecla;
int xEnemigo = 500;
int incr = 0;
int yEnemigo = 200;
int ySuelo = 232;
int pedazos = 0;
int angulo = 0;
int h = 0;
int h2 = 0;
int corte_i = FALSE;
int corte_d = FALSE;
int lugarRama;
int lugarPersonaje;
int tiempo = 50;
int timee = 0;
int opcion = 0;
int aa = 1;
int GO = 0;
char str[10];

/* 
 * Matrix to defines tree 
 */
int arb[][6]={{0,1,2,1,0,1},
			  {0,1,2,2,1,1},
			  {0,1,2,2,2,2},
		  	  {0,0,1,0,1,2},
			  {0,1,1,1,0,1},
			  {2,2,2,1,1,1}};
 
/*
 * Prototipes
 */
void comprobarTeclas();
void moverElementos();
void comprobarColisiones();
void dibujarElementos(int);
void pausaFotograma(int);
void moverDerecha();
void moverIzquierda();
void lanzarPresentacion();
void moverEnemigo();
void dibujarFondo();
int inicializa();
void buclePrincipal();
void talar(int);
void hilo();
void pintarArbol();
void cargarBarra();
void store(int);
int muerte();

/* 
*  Interface variables 
*/
Serial* SP;
char incomingData[256] = "";
int dataLength = 1;
int readResult = 0;


/* 
 * Serial port methods 
 */
Serial :: Serial(char *portName) {
    //We're not yet connected
    this->connected = false;

    //Try to connect to the given port throuh CreateFile
    this->hSerial = CreateFile(portName,
            GENERIC_READ | GENERIC_WRITE,
            0,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL);

    // Check if the connection was successfull
    if (this->hSerial == INVALID_HANDLE_VALUE) {
        // If not success full display an Error
        if (GetLastError() == ERROR_FILE_NOT_FOUND) {
            // Print Error if neccessary
            //printf("ERROR: Handle was not attached. Reason: %s not available.\n", portName);
        } else {
            //printf("ERROR!!!");
        }
    } else {
        // If connected we try to set the comm parameters
        DCB dcbSerialParams = {0};

        // Try to get the current
        if (!GetCommState(this->hSerial, &dcbSerialParams)) {
            // If impossible, show an error
            //printf("failed to get current serial parameters!");
        } else {
            // Define serial connection parameters for the arduino board
            dcbSerialParams.BaudRate = CBR_9600;
            dcbSerialParams.ByteSize = 8;
            dcbSerialParams.StopBits = ONESTOPBIT;
            dcbSerialParams.Parity = NOPARITY;
            // Setting the DTR to Control_Enable ensures that the Arduino is properly
            // reset upon establishing a connection
            dcbSerialParams.fDtrControl = DTR_CONTROL_ENABLE;

            // Set the parameters and check for their proper application
            if (!SetCommState(hSerial, &dcbSerialParams)) {
                //printf("ALERT: Could not set Serial Port parameters");
            } else {
	            // If everything went fine we're connected
	            this->connected = true;
	            // Flush any remaining characters in the buffers 
	            PurgeComm(this->hSerial, PURGE_RXCLEAR | PURGE_TXCLEAR);
	            // We wait 2s as the arduino board will be reseting
	        	Sleep(ARDUINO_WAIT_TIME);
            }
        }
    }
}

Serial :: ~Serial() {
    // Check if we are connected before trying to disconnect
    if (this->connected) {
        // We're no longer connected
        this->connected = false;
        // Close the serial handler
        CloseHandle(this->hSerial);
    }
}

int Serial :: ReadData(char *buffer, unsigned int nbChar) {
    // Number of bytes we'll have read
    DWORD bytesRead;
    // Number of bytes we'll really ask to read
    unsigned int toRead;

    //Use the ClearCommError function to get status info on the Serial port
    ClearCommError(this->hSerial, &this->errors, &this->status);

    // Check if there is something to read
    if (this->status.cbInQue > 0) {
        /*
		* If there is we check if there is enough data to read the required number
        * of characters, if not we'll read only the available characters to prevent
        * locking of the application.
        */
        if(this->status.cbInQue>nbChar) {
            toRead = nbChar;
        } else {
            toRead = this->status.cbInQue;
        }

        // Try to read the require number of chars, and return the number of read bytes on success
        if (ReadFile(this->hSerial, buffer, toRead, &bytesRead, NULL)) {
            return bytesRead;
        }
    }

    // If nothing has been read, or that an error was detected return 0
    return 0;
}


bool Serial :: WriteData(char *buffer, unsigned int nbChar) {
    DWORD bytesSend;

    // Try to write the buffer on the Serial port
    if (!WriteFile(this->hSerial, (void *)buffer, nbChar, &bytesSend, 0)) {
        //In case it don't work get comm error and return false
        ClearCommError(this->hSerial, &this->errors, &this->status);
        return false;
    }
    
    return true;
}

bool Serial :: IsConnected() {
    //Simply return the connection status
    return this->connected;
}


/*Main*/
int main() {
    int i,j;
    
	/* DWORD ThreadID;
  	*  t_personaje=CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)hilo,NULL,CREATE_SUSPENDED,&ThreadID);
    */
    
    // try to init
    if (inicializa() != 0)
        exit(1);
        
	while(1) {
	    lanzarPresentacion();
	    //ResumeThread(t_personaje);
	    buclePrincipal();
	}
	
    destroy_bitmap(personaje[0]);
    destroy_bitmap(personaje[1]);
    destroy_bitmap(fragmentoSuelo);
    destroy_bitmap(presentacion);
    destroy_bitmap(pantallaOculta);
 
    rest(1000);
    return 0;
}
 
END_OF_MAIN();

// --- Bucle principal del juego -----
void buclePrincipal() {
  partidaTerminada = FALSE;
  int i = 0;
  flag = 0;
  
  do {
    comprobarTeclas();
    //moverElementos();
    
    dibujarElementos(i);
    //pausaFotograma(200);
    if (flag == 15){
		i++;
	    if (i == 2)
	    	i = 0;
		flag = 0;	
    }
    flag++;
	comprobarColisiones();
  } while (partidaTerminada != TRUE);
}
 
 
// function to chech keys to move or exit
void comprobarTeclas(){   
	
 /*Movimientos con teclas*/
 
	if (keypressed()) {
		
	    tecla = readkey() >> 8;
	    if (tecla == KEY_ESC ) {
	      partidaTerminada = TRUE;
		  exit(0); 
		}
	    if (tecla == KEY_RIGHT) {
			var = 1;
			moverDerecha();
			corte_d = TRUE;
			pedazos++;
			
			int i = 2;
			
			if (tiempo < 118)
				tiempo += 2;
			
			while (i < 4) {
		       talar(i);
		       pausaFotograma(20);
		       i++;
		   	}
		   	
		   	lugarPersonaje = 2;
			i = 2;
	    }
	          
	    if (tecla==KEY_LEFT) {
			var = 0;
			moverIzquierda();
			corte_i = TRUE;
			pedazos++;
			
			int i = 2;
			
			if(tiempo < 118)
				tiempo += 2;
			
			while (i < 4) {
		       talar(i);
		       pausaFotograma(20);
		       i++;
		   	}
		   	
			lugarPersonaje = 0;
			i = 2;
		}
	}
    clear_keybuf();
	
	
	
	
	/* 
	* Movimiento con interface serial 
	* Comment this if seccion if you want to run program without 
	* control interface
	*/
	/*
	if (SP->IsConnected()) {

		readResult = SP->ReadData(incomingData,dataLength);
		// printf("Bytes read: (0 means no data available) %i\n",readResult);
        incomingData[readResult] = 0;
		if (strncmp(incomingData, "a", 1) == 0){
			
		//printf("%s",incomingData);	
		} else if (strncmp(incomingData, "b", 1) == 0){
			
		//printf("%s",incomingData);	
    	}
    	else if (strncmp(incomingData, "c", 1) == 0){
			
		//printf("%s",incomingData);	
    	}
    	else if (strncmp(incomingData, "d", 1) == 0){
			var = 1;
			moverDerecha();
			corte_d = TRUE;
			pedazos++;
			int i = 2;
			if(tiempo < 118)
			tiempo += 2;
			while (i < 4) {
		       talar(i);
		       pausaFotograma(20);
		       i++;
		   	}
		   	lugarPersonaje = 2;
			i = 2;
			//printf("%s",incomingData);	
    	} else if (strncmp(incomingData, "e", 1) == 0) {
			//printf("%s",incomingData);	
    	}
    	else if (strncmp(incomingData, "f", 1) == 0){
			var = 0;
			moverIzquierda();
			corte_i = TRUE;
			pedazos++;
			int i = 2;
			if (tiempo < 118)
				tiempo += 2;
			while (i < 4) {
		       talar(i);
		       pausaFotograma(20);
		       i++;
		   	}
		   	lugarPersonaje = 0;
			i = 2;
			//printf("%s",incomingData);	
    	} else if (strncmp(incomingData, "g", 1) == 0){
			//printf("%s",incomingData);	
    	}else{
			//printf("%s",incomingData);	
    	} 
	}
	*/
	 
   /*Movimiento con mouse*/
   
    if ((mouse_x > 0 && mouse_x < 175) && mouse_b & 1) {
		var = 0;
		moverIzquierda();
		corte_i = TRUE;
		pedazos++;
		int i = 2;
		if (tiempo < 118)
			tiempo += 2;
		while (i < 4) {
	       talar(i);
	       pausaFotograma(60);
	       i++;
	   	}
	   	lugarPersonaje = 0;
		i = 2;
   } else if ((mouse_x > 175 && mouse_x < 350) && mouse_b & 1) {
		var = 1;
		moverDerecha();
		corte_d = TRUE;
		pedazos++;
		int i = 2;
		if (tiempo < 118)
			tiempo += 2;
		while (i < 4) {
	       talar(i);
	       pausaFotograma(60);
	       i++;
	   	}
	   	lugarPersonaje = 2;
		i = 2;	
   } else {
		if ( timee == 5) {
   			if (tiempo >= 0)
				tiempo--;
			timee = 0;
		} else {
			timee++;
		}
	}
		
}
 
 
// -- Intenta mover el personaje hacia la derecha
void moverDerecha() {
	estado[0] = load_bmp("assets/lenador2.bmp", pal);
	estado[1] = load_bmp("assets/lenador2_2.bmp", pal);
	estado[2] = load_bmp("assets/lenador_tala2_1.bmp", pal);
	estado[3] = load_bmp("assets/lenador_tala2_2.bmp", pal);
}
 
 
// -- Intenta mover el personaje hacia la izquierda
void moverIzquierda() {
	estado[0] = load_bmp("assets/lenador1.bmp", pal);
	estado[1] = load_bmp("assets/lenador1_2.bmp", pal);
	estado[2] = load_bmp("assets/lenador_tala1_1.bmp", pal);
	estado[3] = load_bmp("assets/lenador_tala1_2.bmp", pal);
}
 

 
// -- Mover otros elementos del juego 
void moverElementos() {
	moverEnemigo();
}
 
 
// -- Comprobar colisiones de nuestro elemento con otros, o disparos con enemigos, etc
void comprobarColisiones() {
	
    // Por ahora solo se comprueban las colisiones
    if (lugarRama == lugarPersonaje) {
		partidaTerminada = TRUE;
  		opcion = 3;
  		pedazos = 0;
  		tiempo = 50;
    }
    if (tiempo == 0) {
		opcion = 3;
		partidaTerminada = TRUE;
		pedazos = 0;
		tiempo = 50;
    }
}

void talar(int i) {
	
    clear_bitmap(pantallaOculta);
    dibujarFondo();
    pintarArbol();
  
    // 
    switch(var) {	
		case 0:
			if (i == 2) {
				draw_sprite(pantallaOculta, estado[i], x_izq - 47, y);
			} else if (i == 3) {
				draw_sprite(pantallaOculta, estado[i], x_izq + 12, y);	
			}
		break;
		case 1:
			if (i == 2) {
				draw_sprite(pantallaOculta, estado[i], x_der + 12, y);
			} else if (i == 3) {
				draw_sprite(pantallaOculta, estado[i], x_der - 47, y);	
			}
		break;
    }
  
  
    draw_sprite(pantallaOculta, tronco,110+x_arb, 400);
    draw_sprite(pantallaOculta, pause[0], 255, 10);
    //draw_sprite(pantallaOculta, barra, 106, 10);
    cargarBarra();
    masked_blit(cursor, pantallaOculta, 0, 0, mouse_x, mouse_y, 13, 22);
    //draw_sprite(pantallaOculta, enemigo, xEnemigo, yEnemigo);

    // Sincronizo con el barrido para evitar parpadeos
    // y vuelco la pantalla oculta
    vsync();
    blit(pantallaOculta, screen, 0, 0, 0, 0, ANCHOPANTALLA, ALTOPANTALLA); 
 }
 
void cargarBarra() {
	draw_sprite(pantallaOculta, barra, 106, 10);
	stretch_blit(carga, pantallaOculta, 0, 0, 2, 23, 116, 19, tiempo, 20);
}
 
// Draw elements on the screen
void dibujarElementos(int i) {
 
  // Borro la pantalla
  clear_bitmap(pantallaOculta);
 
  // Dibujo el "fondo", con los trozos de piezas anteriores
  dibujarFondo();
 
  // Dibujo el personaje y el enemigo
  draw_sprite(pantallaOculta, tronco, 110 + x_arb, 400);
  pintarArbol();
  
  if(var == 0) {
		draw_sprite(pantallaOculta, estado[i], x_izq, y);		
  } else if(var==1) {
		draw_sprite(pantallaOculta, estado[i], x_der, y);		
  }
  	
  draw_sprite(pantallaOculta, pause[0], 255, 10);
  cargarBarra();
  masked_blit(cursor, pantallaOculta, 0, 0, mouse_x, mouse_y, 13, 22);
  //draw_sprite(pantallaOculta, enemigo, xEnemigo, yEnemigo);
 
  // Sincronizo con el barrido para evitar parpadeos
  // y vuelco la pantalla oculta
  vsync();
  blit(pantallaOculta, screen, 0, 0, 0, 0, ANCHOPANTALLA, ALTOPANTALLA);
}
 
void pintarArbol() {
	
	int posicion_y;
	if (corte_i == FALSE && corte_d == FALSE) {
		for(int i = 0; i < 6; i++) {
			if (arb[a][i] == 0) {
				draw_sprite(pantallaOculta, rama[arb[a][i]], 29 + x_arb, i * 75 - 50);
			} else if (arb[a][i] == 1 || arb[a][i] == 2) {
				draw_sprite(pantallaOculta, rama[arb[a][i]], 117 + x_arb, i * 75 - 50);
			}
			textout_centre_ex(pantallaOculta, font, itoa(pedazos,str,10), SCREEN_W/2, 80, palette_color[15], -1);		
			if (i == 5) {
				if (arb[a][i] == 0) {
					lugarRama = 0;
				} else if (arb[a][i] == 2) {
					lugarRama = 2;
				} else {
					lugarRama = 1;
				}
			}
		}
	}
	
	if (corte_i == TRUE){
		for(int i = 0; i < 6; i++) {
			if (arb[a][i] == 0) {
				if (i == 5)
					pivot_sprite(pantallaOculta, rama[arb[a][i]], 117 + x_arb + (h += 12), 325 + (incr += 4), 0, 0, itofix(angulo += 4));
				else
					draw_sprite(pantallaOculta, rama[arb[a][i]], 29 + x_arb, i * 75 - 50 + h2);
			} else if (arb[a][i]==1 || arb[a][i]==2) {
				if (i == 5)
					pivot_sprite(pantallaOculta, rama[arb[a][i]], 117 + x_arb + (h += 12), 325 + (incr += 4), 0, 0, itofix(angulo += 4));
				else
					draw_sprite(pantallaOculta, rama[arb[a][i]], 117 + x_arb, i * 75 - 50 + h2);
			}
			textout_centre_ex(pantallaOculta, font, itoa(pedazos, str, 10), SCREEN_W/2, 80, palette_color[15], -1);		
		}
		
		if (250 + h2 <= 325)
			h2+=10;
			
		if (h2 >= 75) {
			arb[a][5] = arb[a][4];
			arb[a][4] = arb[a][3];
			arb[a][3] = arb[a][2];
			arb[a][2] = arb[a][1];
			arb[a][1] = arb[a][0];
			
			if(arb[a][0] == 0 || arb[a][0] == 2) {
				arb[a][0] = 1;
			} else {
				arb[a][0] = rand() % 3;
			}
			
			//arb[a][0] = rand() % 3;
			h2 = 0;
			corte_i = FALSE;
			angulo = 0;
			incr = 0;
			h = 0;
  		}	
	}
	
	if (corte_d == TRUE) {
		for(int i = 0; i < 6; i++) {
			if (arb[a][i] == 0) {
				if (i == 5)
					pivot_sprite(pantallaOculta,rama[arb[a][i]], 117 + x_arb - (h += 12), 325 + (incr += 4), 0, 0, itofix(angulo += 4));
				else
					draw_sprite(pantallaOculta, rama[arb[a][i]], 29 + x_arb, i * 75 - 50 + h2);
			} else if(arb[a][i] == 1 || arb[a][i] == 2) {
				if (i == 5)
					pivot_sprite(pantallaOculta, rama[arb[a][i]], 117 + x_arb - (h += 12), 325 + (incr += 4), 0, 0, itofix(angulo += 4));
				else
					draw_sprite(pantallaOculta, rama[arb[a][i]], 117 + x_arb, i * 75 - 50 + h2);
			}
			textout_centre_ex(pantallaOculta, font, itoa(pedazos, str, 10), SCREEN_W/2, 80, palette_color[15], -1);		
		}
		
		if (250 + h2 <= 325)
			h2 += 10;
		
		if (h2 >= 75) {
			arb[a][5] = arb[a][4];
			arb[a][4] = arb[a][3];
			arb[a][3] = arb[a][2];
			arb[a][2] = arb[a][1];
			arb[a][1] = arb[a][0];
			
			if(arb[a][0] == 0 || arb[a][0] == 2)
				arb[a][0] = 1;
			else
				arb[a][0] = rand() % 3;
			
			h2 = 0;
			corte_d = FALSE;
			angulo = 0;
			incr = 0;
			h = 0;
		}	
	}
}
 
 
// -- Pausa hasta el siguiente fotograma
void pausaFotograma(int i) {
  // Para 25 fps: 1000/25 = 40 milisegundos de pausa
  //rest(40);
  rest(i);
}

int muerte() {
	valor = FALSE;
	int flag = 0;
	int increce = 0;
	int decrece = 0;
	int sube = 0;
	int i = 0;
	int select = 0;
	do {
		//if (sube == 0) {
			if (increce <= 465) {
				draw_sprite(pantallaOculta, presentacion, 0, 0);
				pintarArbol();
				textout_centre_ex(pantallaOculta, font, itoa(increce, str, 10), SCREEN_W/2, 280, palette_color[15], -1);
				draw_sprite(pantallaOculta, tronco, 110 + x_arb, 400);
				draw_sprite(pantallaOculta, gameOver, 55, 0 + increce - 465);
				draw_sprite(pantallaOculta, menu[0], 40, x_menu + increce - 465);
		  		draw_sprite(pantallaOculta, menu[1], 130, x_menu + increce - 465);
		  		draw_sprite(pantallaOculta, menu[2], 250, x_menu + increce - 465);
			}	
			if (increce >= 465 && decrece >= 480) {
				draw_sprite(pantallaOculta, presentacion, 0, 0);
				if (mouse_x > 40 && mouse_x < 116 && mouse_y > 415 && mouse_y < 475) {
					draw_sprite(pantallaOculta, presentacion, 0, 0);
					pintarArbol();
					draw_sprite(pantallaOculta, rip, x_izq, y + 50);
					
					draw_sprite(pantallaOculta, tronco, 110 + x_arb, 400);
					draw_sprite(pantallaOculta, gameOver, 55, 0 + increce - 465);
			  		draw_sprite(pantallaOculta, menu[3], 40, x_menu + increce - 465);
					draw_sprite(pantallaOculta, menu[1], 130, x_menu + increce - 465);
					draw_sprite(pantallaOculta, menu[2], 250, x_menu + increce - 465);
					masked_blit(cursor, pantallaOculta, 0, 0, mouse_x, mouse_y, 13, 22);
			  		blit(pantallaOculta, screen, 0, 0, 0, 0, ANCHOPANTALLA, ALTOPANTALLA);
			  		
					if (mouse_b & 1) {
					}
					//valor = TRUE;
				} else if (mouse_x > 130 && mouse_x < 226 && mouse_y > 415 && mouse_y < 475) {
					draw_sprite(pantallaOculta, presentacion, 0, 0);
					pintarArbol();
					draw_sprite(pantallaOculta, rip, x_izq, y + 50);
					
					draw_sprite(pantallaOculta, tronco, 110 + x_arb, 400);
					draw_sprite(pantallaOculta, gameOver, 55, 0 + increce - 465);
			  		draw_sprite(pantallaOculta, menu[0], 40, x_menu+increce-465);
					draw_sprite(pantallaOculta, menu[4], 130, x_menu+increce-465);
					draw_sprite(pantallaOculta, menu[2], 250, x_menu+increce-465);
					masked_blit(cursor, pantallaOculta, 0, 0, mouse_x, mouse_y, 13, 22);
			  		blit(pantallaOculta, screen, 0, 0, 0, 0, ANCHOPANTALLA, ALTOPANTALLA);
					
					if((mouse_b & 1) && aa == 1) {
						valor = TRUE;
						opcion = 2;
						sube = 1;
					}
				} else if (mouse_x > 250 && mouse_x < 316 && mouse_y > 415 && mouse_y < 475) {
					draw_sprite(pantallaOculta, presentacion, 0, 0);
					pintarArbol();
			  		draw_sprite(pantallaOculta, rip, x_izq, y + 50);
			  		
					draw_sprite(pantallaOculta, tronco, 110 + x_arb, 400);
					draw_sprite(pantallaOculta, gameOver, 55, 0 + increce - 465);
			  		draw_sprite(pantallaOculta, menu[0], 40, x_menu + increce - 465);
					draw_sprite(pantallaOculta, menu[1], 130, x_menu + increce - 465);
					draw_sprite(pantallaOculta, menu[5], 250, x_menu + increce - 465);
					masked_blit(cursor, pantallaOculta, 0, 0, mouse_x, mouse_y, 13, 22);
			  		blit(pantallaOculta, screen, 0, 0, 0, 0, ANCHOPANTALLA, ALTOPANTALLA);
					
					if (mouse_b & 1) {
						//CloseHandle(t_personaje); 
						//select=1;
						valor = TRUE;
						opcion = 1;
						return 1;
						//exit(0);
					}
					//valor = TRUE;
				} else {
					//draw_sprite(pantallaOculta, personaje[0],x, y);
					pintarArbol();
					draw_sprite(pantallaOculta, tronco, 110 + x_arb, 400);
					draw_sprite(pantallaOculta, gameOver, 55, 0 + increce - 465);
			  		draw_sprite(pantallaOculta, menu[0], 40, x_menu + increce - 465);
			  		draw_sprite(pantallaOculta, menu[1], 130, x_menu + increce - 465);
			  		draw_sprite(pantallaOculta, menu[2], 250, x_menu + increce - 465);
			  		textout_centre_ex(pantallaOculta, font, itoa(increce, str, 10), SCREEN_W/2, 80, palette_color[15], -1);
			
			  		draw_sprite(pantallaOculta, rip, x_izq, y + 50);
			  		
			  		masked_blit(cursor, pantallaOculta, 0, 0, mouse_x, mouse_y, 13, 22);
			  		blit(pantallaOculta, screen, 0, 0, 0, 0, ANCHOPANTALLA, ALTOPANTALLA);
			  		aa = 1;
				}
			    //textout_centre_ex(pantallaOculta,font,"que onda esme",SCREEN_W/2,80, palette_color[15], -1);
			    //pausaFotograma(200);  
			}
			
			if (increce <= 465)
		  			increce += 10;
		  	if (decrece <= 480)
		  			decrece += 10;	
		//}		
	} while (valor != TRUE);
	
}

void store(int bandera) {
	int flag = 0;
	int i = 0;
	int increce = 0;
	int decrece = 0;
	int sube = 0;
	
	do {
		if (sube == 0) {
			if (increce < 465 && decrece < 480) {
				draw_sprite(pantallaOculta, presentacion, 0, 0);
				pintarArbol();
				draw_sprite(pantallaOculta, tronco,110+x_arb, 400);
				
				draw_sprite(pantallaOculta, menu[0], 40, x_menu - decrece);
		  		draw_sprite(pantallaOculta, menu[1], 130, x_menu - decrece);
		  		draw_sprite(pantallaOculta, menu[2], 250, x_menu - decrece);
				
				draw_sprite(pantallaOculta, shop,30, 0 + increce - 465);
		  		draw_sprite(pantallaOculta, before[0], 20, x_menu+10+increce-465);
		  		draw_sprite(pantallaOculta, selectt[0], 120, x_menu+10+increce-465);
		  		draw_sprite(pantallaOculta, after[0], 240, x_menu+10+increce-465);
			}	
			if (increce >= 465 && decrece >= 480) {
				draw_sprite(pantallaOculta, presentacion, 0, 0);
				pintarArbol();
				draw_sprite(pantallaOculta, tronco, 110 + x_arb, 400);
				
				if (mouse_x > 20 && mouse_x < 116 && mouse_y > 425 && mouse_y < 485) {
					draw_sprite(pantallaOculta, shop, 30, 0 + increce - 465);
		  			draw_sprite(pantallaOculta, before[1],20, x_menu + 10 + increce - 465);
		  			draw_sprite(pantallaOculta, selectt[0], 120, x_menu+10+increce - 465);
		  			draw_sprite(pantallaOculta, after[0], 240, x_menu + 10 + increce - 465);
				} else if (mouse_x > 120 && mouse_x < 236 && mouse_y > 425 && mouse_y < 485) {
					draw_sprite(pantallaOculta, shop,30, 0 + increce - 465);
		  			draw_sprite(pantallaOculta, before[0], 20, x_menu + 10 + increce - 465);
		  			draw_sprite(pantallaOculta, selectt[1], 120, x_menu + 10 + increce - 465);
		  			draw_sprite(pantallaOculta, after[0], 240, x_menu + 10 + increce - 465);
		  			if (mouse_b & 1) {
						aa = 0;
						sube = 1;
					}
				} else if (mouse_x > 240 && mouse_x < 336 && mouse_y > 425 && mouse_y < 485) {
					draw_sprite(pantallaOculta, shop, 30, 0 + increce - 465);
		  			draw_sprite(pantallaOculta, before[0], 20, x_menu + 10 + increce - 465);
		  			draw_sprite(pantallaOculta, selectt[0],120, x_menu + 10 + increce - 465);
		  			draw_sprite(pantallaOculta, after[1], 240, x_menu + 10 + increce - 465);
				} else {
					draw_sprite(pantallaOculta, shop, 30, 0 + increce - 465);
		  			draw_sprite(pantallaOculta, before[0],20, x_menu + 10 + increce - 465);
		  			draw_sprite(pantallaOculta, selectt[0], 120, x_menu + 10 + increce - 465);
		  			draw_sprite(pantallaOculta, after[0], 240, x_menu + 10 + increce - 465);	
				}
			}
			
			if (increce <= 465)
	  			increce += 8;
	  		if (decrece <= 480)
	  			decrece += 8;
		}
		
		if (sube == 1) {
			if (increce >= 0 && decrece >= 0) {
				draw_sprite(pantallaOculta, presentacion, 0, 0);
				pintarArbol();
				draw_sprite(pantallaOculta, tronco, 110 + x_arb, 400);
				draw_sprite(pantallaOculta, menu[0], 40, x_menu - decrece);
		  		draw_sprite(pantallaOculta, menu[1], 130, x_menu - decrece);
		  		draw_sprite(pantallaOculta, menu[2], 250, x_menu - decrece);
				
				draw_sprite(pantallaOculta, shop, 30, 0 + increce - 465);
		  		draw_sprite(pantallaOculta, before[0], 20, x_menu + 10 + increce - 465);
		  		draw_sprite(pantallaOculta, selectt[0], 120, x_menu + 10 + increce - 465);
		  		draw_sprite(pantallaOculta, after[0], 240, x_menu + 10 + increce - 465);
			}
				
			if (increce <= 0 && decrece <= 0) {	
				opcion = 0;
				bandera = 0;
			}
			
			if (increce >= 0)
	  			increce -= 8;
	  		if (decrece >= 0)
	  			decrece -= 8;
		}
			
  		if (var == 0)
  			draw_sprite(pantallaOculta, estado[i], x_izq, y);
  		else if(var == 1)
  			draw_sprite(pantallaOculta, estado[i], x_der, y);
  		
  		masked_blit(cursor, pantallaOculta, 0, 0, mouse_x, mouse_y, 13, 22);
  		blit(pantallaOculta, screen, 0, 0, 0, 0, ANCHOPANTALLA, ALTOPANTALLA);
	  		
		//textout_centre_ex(pantallaOculta,font,"que onda esme",SCREEN_W/2,80, palette_color[15], -1);
		//pausaFotograma(200);
		
		if (flag == 80) {
			i++;
			if (i == 2)
				i = 0;
			flag = 0;
		}  
		
		flag++;
		
	} while (bandera != 0);
	  
} 
 
// -- Funciones que no son de la logica juego, sino de 
// funcionamiento interno de otros componentes
 
// -- Pantalla de presentacion
void lanzarPresentacion(){
	valor = FALSE;
	int i = 0, j = 0;
	int j1 = 0, j2 = 0;
	flag = 0;
	int flag2;
	int select = 0;
	srand(time(NULL));
	a = rand() % 5;
	bool ciclo = TRUE;
	//ResumeThread(t_personaje);
	
	do {
		switch(opcion) {
			case 0:
		  		do {
			  		draw_sprite(pantallaOculta, presentacion, 0, 0);
			  		
				  	if (mouse_x > 40 && mouse_x < 116 && mouse_y > 415 && mouse_y < 475) {
						draw_sprite(pantallaOculta, presentacion, 0, 0);
						
						if (var == 0)
				  			draw_sprite(pantallaOculta, estado[i], x_izq, y);
				  		else if(var == 1)
				  			draw_sprite(pantallaOculta, estado[i], x_der, y);
						
						pintarArbol();
						draw_sprite(pantallaOculta, tronco, 110 + x_arb, 400);
				  		draw_sprite(pantallaOculta, menu[3], 40, x_menu);
						draw_sprite(pantallaOculta, menu[1],130, x_menu);
						draw_sprite(pantallaOculta, menu[2],250, x_menu);
						masked_blit(cursor,pantallaOculta, 0, 0, mouse_x, mouse_y, 13, 22);
				  		blit(pantallaOculta, screen, 0, 0, 0, 0,ANCHOPANTALLA, ALTOPANTALLA);
						if (mouse_b & 1) {
						}
						//valor = TRUE;
			  		} else if (mouse_x > 130 && mouse_x < 226 && mouse_y > 415 && mouse_y < 475) {
						draw_sprite(pantallaOculta, presentacion, 0, 0);
					
						if (var == 0)
				  			draw_sprite(pantallaOculta, estado[i], x_izq, y);
				  		else if(var == 1)
				  			draw_sprite(pantallaOculta, estado[i], x_der, y);
				  			
						pintarArbol();
						draw_sprite(pantallaOculta, tronco, 110 + x_arb, 400);
				  		draw_sprite(pantallaOculta, menu[0], 40, x_menu);
						draw_sprite(pantallaOculta, menu[4], 130, x_menu);
						draw_sprite(pantallaOculta, menu[2], 250, x_menu);
						masked_blit(cursor,pantallaOculta, 0, 0, mouse_x, mouse_y, 13, 22);
				  		blit(pantallaOculta, screen, 0, 0, 0, 0, ANCHOPANTALLA, ALTOPANTALLA);
						if ((mouse_b & 1) && aa == 1) {
							valor = TRUE;
							opcion = 2;
						}
			  		} else if (mouse_x > 250 && mouse_x < 316 && mouse_y > 415 && mouse_y < 475) {
						draw_sprite(pantallaOculta, presentacion, 0, 0);
						
						if (var == 0)
				  			draw_sprite(pantallaOculta, estado[i], x_izq, y);
				  		else if(var == 1)
				  			draw_sprite(pantallaOculta, estado[i], x_der, y);
				  			
				  		pintarArbol();
						draw_sprite(pantallaOculta, tronco, 110 + x_arb, 400);
				  		draw_sprite(pantallaOculta, menu[0],40, x_menu);
						draw_sprite(pantallaOculta, menu[1],130, x_menu);
						draw_sprite(pantallaOculta, menu[5],250, x_menu);
						masked_blit(cursor,pantallaOculta,0,0,mouse_x,mouse_y,13,22);
				  		blit(pantallaOculta, screen, 0, 0, 0, 0, ANCHOPANTALLA, ALTOPANTALLA);
						
						if (mouse_b & 1) {
							//CloseHandle(t_personaje); 
							select = 1;
							valor = TRUE;
							opcion = 1;
							//exit(0);
						}
						//valor = TRUE;
			  		} else {
						//draw_sprite(pantallaOculta, personaje[0],x, y);
						pintarArbol();
						draw_sprite(pantallaOculta, tronco, 110 + x_arb, 400);
				  		draw_sprite(pantallaOculta, menu[0], 40, x_menu);
				  		draw_sprite(pantallaOculta, menu[1], 130, x_menu);
				  		draw_sprite(pantallaOculta, menu[2], 250,x_menu);
				  		
				  		if (var == 0)
				  			draw_sprite(pantallaOculta, estado[i], x_izq, y);
				  		else if(var == 1)
				  			draw_sprite(pantallaOculta, estado[i], x_der, y);
				  		
				  		masked_blit(cursor,pantallaOculta,0,0, mouse_x, mouse_y, 13, 22);
				  		blit(pantallaOculta, screen, 0, 0, 0, 0, ANCHOPANTALLA, ALTOPANTALLA);
				  		aa = 1;
				  	}
					//textout_centre_ex(pantallaOculta,font,"que onda esme",SCREEN_W/2,80, palette_color[15], -1);
					//pausaFotograma(200);
					if (flag == 80) {
						i++;
						if (i == 2)
							i = 0;
						flag = 0;
					}  
					
					flag++;
				} while (valor != TRUE);
			
				j1 = 60;
				j2 = 200;
				j = 0;
				flag2 = 0;
			break;
			
			case 1:
				store(select);
			break;
			case 2:
				do {
					draw_sprite(pantallaOculta, presentacion, 0, 0);
					pintarArbol();
					draw_sprite(pantallaOculta, tronco, 110 + x_arb, 400);
					
						draw_sprite(pantallaOculta, tap[0], j1, 430);
			  			draw_sprite(pantallaOculta, tap[1], j2, 430);
					
			  		if (j1 == 60)
			  			j = 1;
			  		else if(j1 == 30)
			  			j = 0;
			  			
			  		if (flag2 == 10) {
						if (j == 1) {
							j1--;
							j2++;
			  			}
			  			else if(j == 0) {
							j1++;
							j2--;
			  			}
			  			flag2 = 0;
			  		}
			  		flag2++;
				  			
			  		if (var == 0)
			  			draw_sprite(pantallaOculta, estado[i], x_izq, y);
			  		else if (var == 1)
			  			draw_sprite(pantallaOculta, estado[i], x_der, y);
			  		
			  		masked_blit(cursor,pantallaOculta,0,0, mouse_x, mouse_y, 13, 22);
			  		blit(pantallaOculta, screen, 0, 0, 0, 0, ANCHOPANTALLA, ALTOPANTALLA);
			  		
			  		if(key[KEY_ENTER] || key[KEY_RIGHT] || key[KEY_LEFT])
						valor = FALSE;
					
			    	if(((mouse_x > 0 && mouse_x < 120) && mouse_b & 1) || ((mouse_x > 230 && mouse_x < 350) && mouse_b & 1))
						valor = FALSE;
						
			  		ciclo = FALSE;
				
					/*
					if (SP->IsConnected()) {
						readResult = SP->ReadData(incomingData, dataLength);
						// printf("Bytes read: (0 means no data available) %i\n",readResult);
			            incomingData[readResult] = 0;
						if (strncmp(incomingData, "f", 1) == 0 || strncmp(incomingData, "d" , 1) == 0) {
							valor = FALSE;
							//printf("%s",incomingData);	
						}
					}*/
					
					if (flag == 80) {
				  		i++;
				  		if (i == 2)
				  			i = 0;
				  		flag = 0;
			  		}  
			  		flag++;	
				} while (valor != FALSE);
			break;
			case 3:
				select = muerte();
				j1 = 60;
				j2 = 200;
				j = 0;
				flag2 = 0;
			break;
		}
	} while (ciclo != FALSE);
  //readkey();
}

void hilo() {
	while (1) {
		masked_blit(cursor, pantallaOculta, 0, 0, mouse_x, mouse_y, 13, 22);
	}	
}
 
// -- Mover el enemigo a su siguiente posicion
void moverEnemigo() {
  	xEnemigo += incr;
  	// Da la vuelta si llega a un extremo
  	if ((xEnemigo > ANCHOPANTALLA-30) || (xEnemigo < 30))
		incr = -incr;
	for (int i = 0; i < 10; i++) {
		BITMAP *A = (BITMAP *)malloc(sizeof(fragmentoSuelo) * 5);
	}
}
 
// -- Dibuja el fondo (por ahora, apenas un fragmento de suelo)
void dibujarFondo(){
    draw_sprite(pantallaOculta, fragmentoSuelo,0, 0);
}
 
/* -------------- Rutina de inicializaci?n -------- */
int inicializa() {
    int i,j;
 
    allegro_init();        // Inicializamos Allegro
    install_keyboard();
    install_timer();
    install_mouse();
                           // Intentamos entrar a modo grafico
    set_color_depth(32);
    if (set_gfx_mode(GFX_SAFE,ANCHOPANTALLA, ALTOPANTALLA, 0, 0) != 0) {
        set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
        allegro_message(
            "Incapaz de entrar a modo grafico\n%s\n",
            allegro_error);
        return 1;
    }
    
    /*
    * TODO - changes this logic to auto-detect current port 
	* where arduino is connected, is needed changes manually
	* and recompile all project
	*
	* Comment this seccion if you want to run program without
	* control interface
    */
    /*
	SP = new Serial("\\\\.\\COM6");
	if (!SP->IsConnected()) {
		set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
        allegro_message(
            "No puede leer puerto serie\n%s\n",
            allegro_error);
        return 1; 
	}*/
	
	// e intentamos abrir im?genes
    cursor = load_bitmap("assets/cursor.bmp", NULL);       
	tronco = load_bitmap("assets/tronco_base.bmp", pal);                 
    rama[0] = load_bitmap("assets/rama_izq.bmp", pal);
	rama[1] = load_bitmap("assets/sin_rama.bmp", pal);
	rama[2] = load_bitmap("assets/rama_der.bmp", pal);
	
	estado[0] = load_bmp("assets/lenador1.bmp", pal);
    estado[1] = load_bmp("assets/lenador1_2.bmp", pal);
    estado[2] = load_bmp("assets/lenador_tala1_1.bmp", pal);
 	estado[3] = load_bmp("assets/lenador_tala1_2.bmp", pal);
 
    fragmentoSuelo = load_bmp("assets/fondop.bmp", pal);
    if (!fragmentoSuelo) {
        set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
        allegro_message("No se ha podido abrir la imagen\n");
        return 1;
    }
 
    presentacion = load_bmp("assets/fondo.bmp", pal);
    if (!presentacion) {
        set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
        allegro_message("No se ha podido abrir la imagen\n");
        return 1;
    }
 	menu[0] = load_bmp("assets/record.bmp", pal);
    if (!presentacion) {
        set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
        allegro_message("No se ha podido abrir la imagen\n");
        return 1;
    }
    menu[1] = load_bmp("assets/play.bmp", pal);
    if (!presentacion) {
        set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
        allegro_message("No se ha podido abrir la imagen\n");
        return 1;
    }
    menu[2] = load_bmp("assets/personaje.bmp", pal);
    if (!presentacion) {
        set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
        allegro_message("No se ha podido abrir la imagen\n");
        return 1;
    }
    menu[3] = load_bmp("assets/record2.bmp", pal);
    if (!presentacion) {
        set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
        allegro_message("No se ha podido abrir la imagen\n");
        return 1;
    }
    menu[4] = load_bmp("assets/play2.bmp", pal);
    if (!presentacion) {
        set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
        allegro_message("No se ha podido abrir la imagen\n");
        return 1;
    }
    menu[5] = load_bmp("assets/personaje2.bmp", pal);
    if (!presentacion) {
        set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
        allegro_message("No se ha podido abrir la imagen\n");
        return 1;
    }
    
    pause[0] = load_bmp("assets/pause.bmp", pal);
    barra = load_bitmap("assets/barra.bmp", pal);
    tap[0] = load_bitmap("assets/tap_der.bmp", pal);
    tap[1] = load_bitmap("assets/tap_izq.bmp", pal);
    carga = load_bitmap("assets/carga.bmp", pal);
    
    shop = load_bitmap("assets/shop.bmp", pal);
    if (!shop) {
        set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
        allegro_message("No se ha podido abrir la imagen\n");
        return 1;
    }
	selectt[0] = load_bitmap("assets/select.bmp", pal);
	if (!selectt[0]) {
        set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
        allegro_message("No se ha podido abrir la imagen\n");
        return 1;
    }
	before[0] = load_bitmap("assets/before.bmp", pal);
	if (!before[0]) {
        set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
        allegro_message("No se ha podido abrir la imagen\n");
        return 1;
    }
	after[0] = load_bitmap("assets/after.bmp", pal);
	if (!after[0]) {
        set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
        allegro_message("No se ha podido abrir la imagen\n");
        return 1;
    }
    selectt[1] = load_bitmap("assets/select2.bmp", pal);
	if (!selectt[1]) {
        set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
        allegro_message("No se ha podido abrir la imagen\n");
        return 1;
    }
	before[1] = load_bitmap("assets/before2.bmp", pal);
	if (!before[1]) {
        set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
        allegro_message("No se ha podido abrir la imagen\n");
        return 1;
    }
	after[1] = load_bitmap("assets/after2.bmp", pal);
	if (!after[1]) {
        set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
        allegro_message("No se ha podido abrir la imagen\n");
        return 1;
    }
    gameOver = load_bitmap("assets/gameOver.bmp", pal);
	if (!gameOver) {
        set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
        allegro_message("No se ha podido abrir la imagen\n");
        return 1;
    }
	rip = load_bitmap("assets/rip.bmp", pal);
	if (!rip) {
        set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
        allegro_message("No se ha podido abrir la imagen\n");
        return 1;
    }
    // Pantalla oculta para evitar parpadeos
    // (doble buffer)
    pantallaOculta = create_bitmap(ANCHOPANTALLA, ALTOPANTALLA);
 
   // Y termino indicando que no ha habido errores
   return 0;
}
 
