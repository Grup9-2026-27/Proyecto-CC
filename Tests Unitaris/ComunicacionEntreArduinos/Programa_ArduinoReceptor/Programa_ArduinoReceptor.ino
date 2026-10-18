// Programa que debe ejecutar el Arduino receptor
#include <SoftwareSerial.h> // Librería que facilitará la comunicación
// El programa transmisor crea un canal de comunicación serie que llama mySerial y le indica qué pines debe usar como RX (pin 10) y como TX (pin 11).
SoftwareSerial mySerial(10, 11); // RX, TX (azul, naranja)
unsigned long nextMillis = 500;
void setup() {
   mySerial.begin(9600);
   Serial.begin(9600);
   Serial.println("Empezamos la recepción");
   
  // Setup establece la velocidad de comunicación, que debe coincidir con la establecida por el transmisor.
}
void loop() {
   if (mySerial.available()) { //mySerial.available() indica si se han recibido nuevos bytes por el canal serie
      String data = mySerial.readString(); // En caso afirmativo se leen todos los bytes hasta el carácter de fin de línea, usando la función mySerial.readString()
      Serial.print(data);
   }
  /*Puesto que queremos que la frase recibida se pueda ver en el monitor serie del IDE conectado al arduino receptor, el programa usa 
  también el canal Serial, tal y como se vio en el paso 3.
  Fíjate que para escribir en el monitor serie la frase recibida el programa receptor usa la función Serial.print(data) y no Serial.println(data), que es lo que cabría esperar*/ 
}