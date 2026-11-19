#include <SoftwareSerial.h> // Librería que facilitará la comunicación

SoftwareSerial mySerial(10, 11); // RX, TX 

const int ledre = 13;        // LED en el pin 13 
const int ledFalloDist = 8;  // LED de alarma para el sensor de distancia
const int ledFallo = 7;      // LED de alarma para el sensor DHT11
const int ledCom = 6;        // LED de alarma de comunicación

const int buzzer = 5;        // Zumbador activo en el pin 5

long nextMillisLed;          // Momento (en ms) en que se debe apagar el LED de recepción
bool ledEncendido = false;   // Indica si el LED de recepción está encendido esperando a apagarse

bool esperandoCom = false;   // Indica si estamos vigilando el tiempo máximo sin recibir tramas
long nextTimeoutCom;         // Momento (en ms) límite antes de declarar pérdida de comunicación (5 s)

bool alarmaSensor = false;  // Indica si la alarma del DHT11 ya está activada
bool alarmaDist = false;    // Indica si la alarma del sensor de distancia ya está activada
bool comCaida = false;      // Indica si la comunicación ha caído

void setup() {  
  mySerial.begin(9600);  // Inicia la comunicación serie con el satélite a 9600 baudios
  Serial.begin(9600);    // Inicia la comunicación serie con el PC / Python a 9600 baudios
 
  pinMode(ledre, OUTPUT);         
  pinMode(ledFallo, OUTPUT);     
  pinMode(ledCom, OUTPUT);        
  pinMode(buzzer, OUTPUT);        
  pinMode(ledFalloDist, OUTPUT);  
}

void loop() {
  // Apagar el LED indicador de recepción tras 100 ms sin bloquear la ejecución del programa
  if (ledEncendido && millis() >= nextMillisLed) {
    digitalWrite(ledre, LOW);  // Apaga el LED indicador de recepción
    ledEncendido = false;      // Actualiza el estado del LED a apagado
  }

  // Recepción y procesamiento de telemetría procedente del satélite
  if (mySerial.available()) {                                   // Comprueba si han llegado datos desde el satélite
    String data = mySerial.readStringUntil('\n');               // Lee el string completo hasta el salto de línea
    data.trim();                                                // Elimina espacios y saltos de línea sobrantes
    Serial.println(data);                                       // Reenvía el string al PC (Python)
    digitalWrite(ledre, HIGH);                                  // Enciende el LED para indicar visualmente la recepción
    ledEncendido = true;                                        // Marca el LED como encendido
    nextMillisLed = millis() + 100;                             // Programa el apagado del LED dentro de 100 ms

    // Corta la cabecera del mensaje recibido a un número entero para que los if / else if siguientes sepan qué LED deben activar.
    int fin = data.indexOf(':');                                   // Localiza el primer ':' que delimita el código del string
    int codigo = (fin > 0) ? data.substring(0, fin).toInt() : -1;  // Extrae el código numérico (o -1 si no hay ':')

    if (codigo == 3) {                 // Código 3: fallo confirmado del sensor DHT11
      digitalWrite(ledFallo, HIGH);    // Enciende el LED de alarma del sensor DHT11
      if (!alarmaSensor) {             // Si la alarma acústica aún no había saltado
        alarmaSensor = true;           // Registra que el sensor DHT11 está en estado de alarma
        digitalWrite(buzzer, HIGH);    // Activa el zumbador acústico de alerta
      }
    } else if (codigo == 1) {            // Código 1: datos correctos de temperatura y humedad
      digitalWrite(ledFallo, LOW);       // Apaga el LED de fallo del DHT11 al recibir datos válidos
      alarmaSensor = false;              // Restablece el estado de alarma del sensor DHT11
    } else if (codigo == 2) {            // Código 2: 3 medias consecutivas > límite en satélite
      digitalWrite(buzzer, HIGH);        // Activa el zumbador por alarma de sobretemperatura
    } else if (codigo == 13) {           // Código 13: fallo confirmado del sensor de distancia
      digitalWrite(ledFalloDist, HIGH);  // Enciende el LED de alarma del sensor ultrasónico
      if (!alarmaDist) {                 // Si la alarma acústica de distancia no estaba activa
        alarmaDist = true;               // Registra que el sensor de distancia está en alarma
        digitalWrite(buzzer, HIGH);      // Activa el zumbador acústico de alerta
      }
    } else if (codigo == 11) {                                  // Datos de distancia recibidos
      int segonDosPunts = data.indexOf(':', fin + 1);           // Busca el segundo ':' que separa el ángulo de la distancia
      if (segonDosPunts != -1 && data.substring(segonDosPunts + 1) != "0") { // Comprueba que la distancia sea válida (distinta de 0) y que ha recibido un segundo ":"
        digitalWrite(ledFalloDist, LOW);                        // Apaga el LED de fallo del sensor ultrasónico
        alarmaDist = false;                                     // Restablece el estado de alarma del sensor de distancia
      }
    }

    // Alarma de comunicación
    esperandoCom = true;                  // Activa el control de la comunicación tras recibir un string válido
    nextTimeoutCom = millis() + 5000;     // Reinicia la cuenta atrás de 5 segundos para el próximo mensaje
    digitalWrite(ledCom, LOW);            // Mantiene apagado el LED de fallo de comunicación
    if (comCaida) {                       // Si la comunicación había estado caída previamente
      comCaida = false;                   // Marca la comunicación como recuperada
      Serial.println("8:Comunicacion restablecida"); // Notifica a Python con un código 8 que vuelve a haber comunicación
    }
  }

  // Comprobación de pérdida de enlace (si pasan más de 5 segundos sin recibir nada del satélite)
  if (esperandoCom && (millis() >= nextTimeoutCom)) {
    digitalWrite(ledCom, HIGH);                     // Enciende el LED de alarma de pérdida de comunicación
    Serial.println("6:Fallo de comunicacion");      // Notifica a Python con código 6 que se ha perdido el enlace
    digitalWrite(buzzer, HIGH);                     // Activa el zumbador acústico de alerta
    comCaida = true;                                // Marca el estado de la comunicación como caída
    esperandoCom = false;                           // Detiene la comprobación hasta que vuelva a entrar un mensaje
  }

  // Reenviar órdenes de Python al satélite
  if (Serial.available()) {                          // Comprueba si Python ha enviado alguna orden 
    String mensajePC = Serial.readStringUntil('\n'); // Lee la orden recibida desde Python hasta el salto de línea
    mensajePC.trim();                                // Elimina espacios y saltos de línea sobrantes
    if (mensajePC.startsWith("7:")) {                // Código 7 desde PC: orden para silenciar el zumbador
      digitalWrite(buzzer, LOW);                     // Apaga el zumbador acústico
    } else if (mensajePC.startsWith("2:")) {         // Código 2 desde PC: orden para activar el zumbador (alarma en tierra)
      digitalWrite(buzzer, HIGH);                    // Enciende el zumbador acústico
    } else {                                         // Cualquier otra orden (ej. 3:, 4:, 9:, 10:) va dirigida al satélite
      mySerial.println(mensajePC);                   // Transmite la orden hacia el satélite por SoftwareSerial
      Serial.println("Se ha enviado la orden");       // Confirma por el puerto serie del PC que la orden ha sido enviada
    }
  }
}