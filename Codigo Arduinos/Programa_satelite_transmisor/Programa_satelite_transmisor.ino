#include <SoftwareSerial.h>  // Librería que facilitará la comunicación
#include <DHT.h>             //Librería para leer el sensor DHT
#define DHTPIN 2
#define DHTTYPE DHT11
#define TRIGPIN 8            // Pin Trig del sensor ultrasónico
#define ECHOPIN 9            // Pin Echo del sensor ultrasónico

DHT dht(DHTPIN, DHTTYPE);

SoftwareSerial mySerial(10, 11);  // RX, TX

// Configuración para el sensor de humedad y temperatura
long nextMillis1;
const long interval1 = 3000;  // 3 segundos para el sensor de H y T

// Configuración para el LED
const int ledtr = 13;  // LED en el pin 13
long nextMillisLed;  // Momento (en ms) en que se debe apagar el LED
bool ledEncendido = false;  // Indica si el LED está encendido esperando a ser apagado
bool esperandoTimeout = false;  // Indica si estamos esperando a confirmar que el sensor falla
long nextTimeoutHT;  // Momento (en ms) en que se confirma el fallo del sensor si sigue sin leerse bien

// Variable para controlar si el satélite tiene que enviar datos o no
bool enviarDatos = true;

// Configuración para el latido
long nextMillisHB;  // Momento (en ms) en que toca enviar el siguiente latido
const long intervalHB = 2000;  // Un latido cada 2 segundos (menor que el timeout de 5 s de tierra)

// Configuración para el sensor ultrasónico
long nextMillis2;
const long interval2 = 1000;  // 1 segundos para el sensor ultrasónico

// Variables para controlar el disparo del ultrasónico sin ningún delay (máquina de estados)
int estadoUltrasonico = 0;          // 0: esperando intervalo, 1: limpiando Trig, 2: enviando pulso de 10 microsegundos
unsigned long nextMicrosTrig = 0;   // Marca de tiempo en microsegundos para controlar la duración del pulso

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
  dht.begin();

  nextMillis1 = millis() + interval1;  // Configuración para el tiempo del sensor
  nextMillis2 = millis() + interval2;  // Configuración para el tiempo del sensor ultrasónico
  nextMillisHB = millis() + intervalHB;  // Programa el primer latido

  pinMode(ledtr, OUTPUT); 
  pinMode(TRIGPIN, OUTPUT);
  pinMode(ECHOPIN, INPUT);
}

void loop() {
  // Apagar el LED cuando haya pasado el tiempo, sin bloquear el programa
  if (ledEncendido && millis() >= nextMillisLed) {
    digitalWrite(ledtr, LOW);  // Apaga el LED
    ledEncendido = false;  // Marca el LED como apagado
  }
  
  // Recibir y procesar las órdenes de tierra
  if (mySerial.available()) {
    String mensajePC = mySerial.readString();
    if (mensajePC == "Parar") {
      enviarDatos = false;
      Serial.println(mensajePC); // Imprime la orden recibida
    } else if (mensajePC == "Reanudar") {
      enviarDatos = true;
      Serial.println(mensajePC); // Imprime la orden recibida
    }
  }

  // Latido: se envía siempre, aunque enviarDatos sea false, para que tierra sepa que el satélite sigue vivo
  if (millis() >= nextMillisHB) {
    mySerial.println("Latido");  // Mensaje de latido
    nextMillisHB = millis() + intervalHB;  // Programa el siguiente latido
  }

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
        digitalWrite(ledtr, HIGH);
        ledEncendido = true;  // Marca el LED como encendido para que el loop lo apague después
        nextMillisLed = millis() + 100;  // Programa el apagado del LED dentro de 100 ms
      }
    }
    
    // Si han pasado 5 segundos con el sensor fallando, avisamos a tierra con "Fallo"
    if (esperandoTimeout && (millis() >= nextTimeoutHT)){
      mySerial.println ("Fallo");
    }
    
    nextMillis1 = millis() + interval1;
  }

  // Configuración del sensor ultrasónico
  // PASO 1 (Estado 0): Esperar a que pase 1 segundo para iniciar una nueva medición
  if (estadoUltrasonico == 0 && millis() >= nextMillis2) {
    digitalWrite(TRIGPIN, LOW);        // Asegura que el pin Trig empiece apagado para evitar ruido
    nextMicrosTrig = micros() + 2;     // Programa esperar 2 microsegundos antes de disparar para asegurar un "punto cero" limpio antes de lanzar el pulso
    estadoUltrasonico = 1;             // Avanza al siguiente estado
  
  // PASO 2 (Estado 1): Pasados los 2 microsegundos, activar el disparo ultrasónico
  } else if (estadoUltrasonico == 1 && micros() >= nextMicrosTrig) {
    digitalWrite(TRIGPIN, HIGH);       // Enciende el pin Trig para empezar a emitir ondas ultrasónicas
    nextMicrosTrig = micros() + 10;    // El sensor requiere que el pulso dure exactamente 10 microsegundos
    estadoUltrasonico = 2;             // Avanza al estado final de lectura
  
  // PASO 3 (Estado 2): Pasados los 10 microsegundos, cortar el pulso y medir el rebote
  } else if (estadoUltrasonico == 2 && micros() >= nextMicrosTrig) {
    digitalWrite(TRIGPIN, LOW);        // Corta el pulso ultrasónico

    long duracion = pulseIn(ECHOPIN, HIGH, 30000);  // Mide cuánto tiempo (en microsegundos) tarda en volver el sonido al pin Echo
    // El parámetro 30000 es el tiempo máximo de espera (timeout) si no hay ningún objeto delante

    // Fórmula física: distancia = (tiempo * velocidad del sonido en cm/us) / 2 (porque el sonido va y vuelve)
    float d = (duracion * 0.0343) / 2.0;

    // Validar si la lectura está fuera de los límites físicos del sensor HC-SR04 (2 cm a 400 cm)
    if (duracion == 0 || d < 2.0 || d > 400.0) {
      mySerial.println("Error al leer el sensor ultrasonico");
    } else {
      // Enviar la distancia hacia la estación de tierra
      mySerial.print(":D:");
      mySerial.println(d);
      digitalWrite(ledtr, HIGH);
      ledEncendido = true;  // Marca el LED como encendido para que el loop lo apague después
      nextMillisLed = millis() + 100;  // Programa el apagado del LED dentro de 100 ms
    }

    // Reiniciar la máquina de estados y programar la próxima medición dentro de 1 segundo
    estadoUltrasonico = 0;
    nextMillis2 = millis() + interval2;
  }
}