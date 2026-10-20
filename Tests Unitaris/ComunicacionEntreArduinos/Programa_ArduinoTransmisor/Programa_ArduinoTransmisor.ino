// Programa más simple para transmitir información del Arduino transmisor (por ejemplo, device 1) al receptor (device 2) se muestra a continuación.
#include <SoftwareSerial.h> // Librería que facilitará la comunicación
int i;
//El programa transmisor crea un canal de comunicación serie que llama mySerial y le indica qué pines debe usar como RX (pin 10) y como TX (pin 11).
SoftwareSerial mySerial(10, 11); // RX, TX 
void setup() { 
   mySerial.begin(9600);
   mySerial.println("Empezamos");
   i=1;
   // Setup establece la velocidad de comunicación de ese canal (9600 baudios) y envía ya un primer mensaje de aviso
}
// La función loop enviará con ese canal, cada 3 segundos, serie la frase “Envío n”, siendo n un número del 1 en adelante
void loop() {
   delay (3000);
   mySerial.print("Envío: ");
   mySerial.println(i);
   i=i+1;

  /*Por el canal de comunicación viajarán bit a bit los códigos ASCII de cada uno de los caracteres de la frase, incluido el carácter de fin de 
  línea que se añade automáticamente por el hecho de haber usado la función println.*/
}