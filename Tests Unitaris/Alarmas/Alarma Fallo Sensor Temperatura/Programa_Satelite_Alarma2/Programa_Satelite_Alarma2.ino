#include <SoftwareSerial.h>  // Librería que facilitará la comunicación
#include <DHT.h>             //Librería para leer el sensor DHT
#define DHTPIN 2
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

SoftwareSerial mySerial(10, 11);  // RX, TX

// Configuración para el sensor de humedad y temperatura
long nextMillis1;
const long interval1 = 3000;  // 3 segundos para el sensor de H y T

bool esperandoTimeout = false;  // * Indica si estamos esperando a confirmar que el sensor falla
long nextTimeoutHT;  // * Momento (en ms) en que se confirma el fallo del sensor si sigue sin leerse bien

bool enviarDatos = true;
void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
  dht.begin();

  nextMillis1 = millis() + interval1;  // Configuración para el tiempo del sensor

}

void loop() {
  // Configuración del sensor de humedad y temperatura
  if (millis() >= nextMillis1) {
    if (enviarDatos == true) {
      float h = dht.readHumidity();     //Devuelve la humedad en porcentaje
      float t = dht.readTemperature();  //devuelve la temperatura en grados Celsius
      // Si no se puede leer la humedad o la temperatura, las funciones devuelven NAN
      if (isnan(h) || isnan(t)) {  //isnan() comprueba si dan NAN | \t es un carácter de tabulación, que nos permite alinear los datos en el monitor serie |
        if (!esperandoTimeout) {  // Solo fijamos el tiempo límite la primera vez que falla la lectura
          esperandoTimeout = true;  // Empezamos a esperar para confirmar el fallo
          nextTimeoutHT = millis() + 5000; // 5 segundos
        }
        mySerial.println("Error al leer el sensor DHT11");
      }
       else {
        esperandoTimeout = false;
        mySerial.print("T:");
        mySerial.print(t);
        mySerial.print(":H:");
        mySerial.println(h);
      }
    }
    
    // Si han pasado 5 segundos con el sensor fallando, avisamos a tierra con "Fallo"
    if (esperandoTimeout && (millis() >= nextTimeoutHT)){
      mySerial.println ("Fallo");
    }
    
    nextMillis1 = millis() + interval1;
  }

}