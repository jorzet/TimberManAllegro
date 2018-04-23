# TimberMan with Allegro

<br>
  <img height="300" src="https://github.com/jorzet/TimberManAllegro/blob/master/unnamed.png">
</br>

Some screenshots

<table border="0">
  <tr>
    <td>
      <a href="https://github.com/jorzet/TimberManAllegro/blob/master/screenshots/timberman_screenshot_1.png" 
         target="_blank" title="menu play">
        <img height=250 
             src="https://github.com/jorzet/TimberManAllegro/blob/master/screenshots/timberman_screenshot_1.png" 
             alt="pic name" 
             style="border:none;"/>
      </a>
    </td>
    <td>
      <a href="https://github.com/jorzet/TimberManAllegro/blob/master/screenshots/timberman_screenshot_2.png" 
         target="_blank" 
         title="start game">
        <img height=250 
             src="https://github.com/jorzet/TimberManAllegro/blob/master/screenshots/timberman_screenshot_2.png" 
             alt="pic name" 
             style="border:none;"/>
      </a>
    </td>
    <td>
      <a href="https://github.com/jorzet/TimberManAllegro/blob/master/screenshots/timberman_screenshot_3.png" 
         target="_blank" 
         title="playing">
        <img height=250 
             src="https://github.com/jorzet/TimberManAllegro/blob/master/screenshots/timberman_screenshot_3.png" 
             alt="pic name" 
             style="border:none;"/>
      </a>
    </td>
  </tr>
</table>

This project recreates the TimberMan game in Dev-C++ with Allegro librarie

Firstable you need to download the Allegro devpaks from http://devpaks.org and install it with Dev-C++ IDE go to "Tools->Package Manager" here you need to install the next packs:
  * Allegro
  * The Allegro GUI 
  * Allegro Font
  * AllegroDGG
  * algif
  * AllegroAVI
  * AllegroGL
  * Allegro supplement
  * AllegroMP3
  * AllegroPNG
  * JPGalleg
  * DUMB with Allegro support

<strong>Note 1:</strong>
  this project is a compilation of 3 projects:
   * TimberManAllegro 
        - from: https://github.com/jorzet/TimberManAllegro.git
   * TimberManTxControl
        - from: https://github.com/jorzet/TimberManTxControl.git
   * TimberManRxControl
        - from: https://github.com/jorzet/TimberManRxControl.git
        
<strong>Note 2:</strong>
  The current project runs as it. Is not necessary to use "TimberManTxControl" and "TimberManRxControl"
  But if you want to run the game with your own control uncomment the next lines from "main.cpp":
  
  <blockquote>
    <pre>
      <code>
        /*if (SP->IsConnected()) {
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
          }*/
          
   </code>
  </pre>
</blockquote>

<blockquote>
  <pre>
    <code>
        /*if (SP->IsConnected()) {
        readResult = SP->ReadData(incomingData, dataLength);
        // printf("Bytes read: (0 means no data available) %i\n",readResult);
              incomingData[readResult] = 0;
        if (strncmp(incomingData, "f", 1) == 0 || strncmp(incomingData, "d" , 1) == 0) {
          valor = FALSE;
          //printf("%s",incomingData);	
        }
        }*/
    </code>
  </pre>
</blockquote>

<blockquote>
  <pre>
    <code>
        /*SP = new Serial("\\\\.\\COM6");
         if (!SP->IsConnected()) {
           set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
              allegro_message(
                  "No puede leer puerto serie\n%s\n",
                  allegro_error);
              return 1; 
         }*/
    </code>
  </pre>
</blockquote>

