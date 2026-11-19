#include <SoftwareSerial.h>  // Librería que facilitará la comunicación
#include <DHT.h>             // Librería para leer el sensor DHT
#define DHTPIN 2             
#define DHTTYPE DHT11        
#include <Servo.h>           // Librería para controlar servomotores

DHT dht(DHTPIN, DHTTYPE);    

Servo miServo; // Llamamos al servomotor miServo

SoftwareSerial mySerial(10, 11);  // RX, TX

long nextMillis1;             // Marca de tiempo (en ms) para la próxima lectura de temperatura y humedad
const long interval1 = 3000;  // 3 segundos para el sensor de H y T

const int ledtr = 13;  // LED en el pin 13

long nextMillisLed;         // Momento (en ms) en que se debe apagar el LED
bool ledEncendido = false;  // Indica si el LED está encendido esperando a ser apagado

bool esperandoTimeout = false;  // Indica si estamos esperando a confirmar que el sensor DHT11 falla
long nextTimeoutHT;             // Momento (en ms) en que se confirma el fallo del DHT11

bool esperandoTimeoutDist = false;  // Indica si el ultrasónico lleva fallando o fuera de rango
long nextTimeoutDist = 0;           // Marca de tiempo (en ms) límite antes de lanzar la alarma (código 13)

long nextMillisHB;             // Momento (en ms) en que toca enviar el siguiente latido
const long intervalHB = 2000;  // Un latido cada 2 segundos

// Variable para controlar si el satélite tiene que enviar datos o no
bool enviarDatos = true;

// Variables para media móvil de 10 valores y alarma por límite
bool calcularEnSatelite = false;  // Por defecto calcula en tierra (false)
float ultimasTemp[10];            // Vector con las últimas 10 temperaturas
int numLecturas = 0;              // Contador de las lecturas que llevamos (hasta 10)
int indiceTemp = 0;               // Posición circular donde guardar la siguiente
float limiteTemp = 30.0;          // Valor límite por defecto
int consecutivasSup = 0;          // Contador de medias consecutivas > límite

// Pin de señal PWM para el servomotor
#define SERVOPIN 6
// Pines del sensor ultrasónico HC-SR04
#define TRIGPIN 8  // Pin de disparo (emite el pulso)
#define ECHOPIN 9  // Pin de eco (recibe el rebote)

// Variables de control del barrido del servo
int anguloActual = 0;            // Ángulo actual del servo (0 a 180 grados)
int pasoAngulo = 5;              // Grados que avanza en cada paso (positivo = ida, negativo = vuelta)
long nextMillisServo = 0;        // Marca de tiempo para el próximo paso
const long intervalServo = 100;  // Tiempo (en ms) entre cada paso del servo

// Variables de control del disparo ultrasónico
long nextMillisSensorU = 0;         // Marca de tiempo para la próxima lectura
const long intervalSensorU = 1000;  // Intervalo de 1 segundo (1000 ms) entre lecturas
int estadoUltrasonico = 0;          // Controla en qué fase del disparo estamos (0, 1 o 2)
long nextMicrosSensorU = 0;         // Marca de tiempo en microsegundos para el pulso

// Función que envía datos a la estación de tierra garantizando la estabilidad y fluidez del movimiento del servomotor
void enviarMensaje(String msg) {
  miServo.detach();             // Desvincula temporalmente el servo para que SoftwareSerial no interfiera su señal PWM
  mySerial.println(msg);        // Envía el string de datos hacia la estación de tierra
  miServo.attach(SERVOPIN);     // Vuelve a conectar el servo al pin PWM
  miServo.write(anguloActual);  // Restaura inmediatamente la posición en la que estaba el servo
}

void setup() {
  Serial.begin(9600);    // Inicia la comunicación serie con el PC 
  mySerial.begin(9600);  // Inicia la comunicación serie con la estación de tierra

  dht.begin();               // Inicializa el sensor de temperatura y humedad DHT11
  miServo.attach(SERVOPIN);  // Asocia el servo al pin 6
  miServo.write(0);          // Posiciona el servo en 0 grados al arrancar
  
  pinMode(ledtr, OUTPUT);      
  pinMode(TRIGPIN, OUTPUT);    
  pinMode(ECHOPIN, INPUT);    
  digitalWrite(TRIGPIN, LOW);  
  
  nextMillis1 = millis() + interval1;              // Programa la primera lectura del DHT11 dentro de 3 segundos
  nextMillisHB = millis() + intervalHB;            // Programa el primer envío de latido dentro de 2 segundos
  nextMillisSensorU = millis() + intervalSensorU;  // Programa la primera medición ultrasónica dentro de 1 segundo
  nextMillisServo = millis() + intervalServo;      // Programa el primer paso de giro del servo dentro de 100 ms
}

void loop() {
  // Apagar el LED cuando haya pasado el tiempo, sin bloquear el programa
  if (ledEncendido && millis() >= nextMillisLed) {
    digitalWrite(ledtr, LOW);  // Apaga el LED indicador de transmisión
    ledEncendido = false;      // Actualiza el estado del LED a apagado
  }
  
  // Recibir y procesar las órdenes de tierra
  if (mySerial.available()) {                               // Comprueba si han llegado datos desde la estación de tierra
    String comando = mySerial.readStringUntil('\n');        // Lee el mensaje completo hasta el salto de línea
    comando.trim();                                         // Elimina espacios en blanco o saltos de línea sobrantes
    int fin = comando.indexOf(':', 0);                      // Busca la posición del carácter ':' que separa código y valor
    if (fin != -1) {                                        // Verifica que el mensaje recibido tenga un formato válido con ':'
      int codigo = comando.substring(0, fin).toInt();       // Extrae y convierte a entero el código de la orden recibida
      if (codigo == 3) {          // Código 3: detener envío de datos de temperatura y humedad
        enviarDatos = false;      // Desactiva el envío de lecturas del DHT11
        Serial.println(comando);  // Muestra la orden recibida en el Monitor Serie del PC
      } else if (codigo == 4) {   // Código 4: reanudar envío de datos
        enviarDatos = true;       // Reactiva el envío de lecturas del DHT11
        Serial.println(comando);  // Muestra la orden recibida en el Monitor Serie del PC
      } else if (codigo == 9) {                          // Código 9: elegir dónde se calcula la media (1=satélite, 0=tierra)
        int modo = comando.substring(fin + 1).toInt();   // Extrae el parámetro tras ':' (1 o 0)
        calcularEnSatelite = (modo == 1);                // Activa el cálculo a bordo si modo es 1, o lo delega a tierra si es 0
        consecutivasSup = 0;                             // Reinicia el contador de alertas consecutivas al cambiar de modo
        Serial.println(comando);                         // Muestra la orden recibida en el Monitor Serie del PC
      } else if (codigo == 10) {                            // Código 10: actualizar temperatura límite
        limiteTemp = comando.substring(fin + 1).toFloat();  // Extrae el nuevo umbral de temperatura tras ':'
        consecutivasSup = 0;                                // Reinicia el contador de alertas consecutivas con el nuevo límite
        Serial.println(comando);                            // Muestra la orden recibida en el Monitor Serie del PC
      }
    }
  }

  // Latido cada 2 segundos
  if (millis() >= nextMillisHB) {
    enviarMensaje("4:");                   // Envía trama de latido ("4:") para confirmar que el satélite sigue operativo
    nextMillisHB = millis() + intervalHB;  // Programa el siguiente latido dentro de 2 segundos
  }

  // Lectura del sensor de humedad y temperatura (DHT11)
  if (millis() >= nextMillis1) {
    if (enviarDatos == true) {              // Solo lee y transmite si tierra no ha pausado el envío (código 3)
      float h = dht.readHumidity();         // Lee la humedad relativa (%) desde el sensor DHT11
      float t = dht.readTemperature();      // Lee la temperatura (°C) desde el sensor DHT11
     
      if (isnan(h) || isnan(t)) {           // Comprueba si el sensor devolvió un error de lectura (Not a Number)
        if (!esperandoTimeout) {            // Si es el primer fallo detectado, inicia el temporizador de confirmación
          esperandoTimeout = true;          // Marca que estamos esperando 5 segundos para confirmar avería permanente
          nextTimeoutHT = millis() + 5000;  // Fija el instante límite (5 segundos después del primer error)
        }
        enviarMensaje("5:Error al leer el sensor DHT11");  // Notifica a tierra el error puntual de lectura
      }
      else {
        esperandoTimeout = false;           // La lectura fue válida, por lo que cancela cualquier espera de fallo

        ultimasTemp[indiceTemp] = t;        // Almacena la temperatura actual en la lista circular
        indiceTemp = (indiceTemp + 1) % 10; // Avanza el índice circular (vuelve a 0 al llegar a 10)
        if (numLecturas < 10) {             // Incrementa el total de muestras válidas hasta llenar el buffer de 10
          numLecturas++;
        }

        if (calcularEnSatelite) {           // Si el modo de cálculo a bordo está activo (orden 9:1)
          float suma = 0;                   // Acumulador para sumar las temperaturas guardadas
          for (int i = 0; i < numLecturas; i++) {
            suma = suma + ultimasTemp[i];   // Suma cada muestra almacenada en el historial
          }
          float media = suma / numLecturas; // Calcula la media móvil con las muestras disponibles

          // Envía el string de código 1 con temperatura, humedad y media calculada a bordo
          enviarMensaje("1:" + String(t) + ":" + String(h) + ":" + String(media));

          if (media > limiteTemp) {         // Comprueba si la media móvil supera el umbral permitido
            consecutivasSup++;              // Suma una infracción consecutiva
            if (consecutivasSup >= 3) {     // Si se supera el límite 3 veces seguidas, dispara la alarma
              enviarMensaje("2:");          // Envía código 2 (alarma por sobretemperatura sostenida)
              consecutivasSup = 0;          // Reinicia el contador para poder volver a detectar futuras rachas
            }
          } else {
            consecutivasSup = 0;            // Si la media vuelve a valores normales, reinicia la racha
          }
        } else {                            // Si el cálculo de media se realiza en tierra (orden 9:0)
          enviarMensaje("1:" + String(t) + ":" + String(h));  // Envía trama estándar con temperatura y humedad

          digitalWrite(ledtr, HIGH);        // Enciende el LED para indicar visualmente el envío de datos
          ledEncendido = true;              // Marca el LED como encendido
          nextMillisLed = millis() + 100;   // Programa el apagado del LED tras 100 ms
        }

      }
    }
    if (esperandoTimeout && (millis() >= nextTimeoutHT)) {  // Si han pasado 5 s desde el primer error sin recuperarse
      enviarMensaje("3:");                                  // Envía código 3 para activar alarma de fallo del DHT11 en tierra
    }
    nextMillis1 = millis() + interval1;                     // Programa el siguiente ciclo de lectura dentro de 3 segundos
  }

  // Movimiento constante del servomotor (Barrido de 0° a 180° y vuelta)
  if (millis() >= nextMillisServo) {
    anguloActual = anguloActual + pasoAngulo;       // Suma o resta el paso de grados según el sentido actual de giro
    if (anguloActual >= 180) {                      // Si alcanza o supera el tope superior de 180°
      anguloActual = 180;                           // Asegura que no exceda el límite físico del motor
      pasoAngulo = pasoAngulo * -1;                 // Invierte el signo del paso para comenzar a girar hacia 0°
    } else if (anguloActual <= 0) {                 // Si alcanza o baja del tope inferior de 0°
      anguloActual = 0;                             // Asegura que no baje de 0°
      pasoAngulo = pasoAngulo * -1;                 // Invierte el signo del paso para comenzar a girar hacia 180°
    }

    miServo.write(anguloActual);                    // Ordena al servomotor moverse al nuevo ángulo calculado
    nextMillisServo = millis() + intervalServo;     // Programa el siguiente incremento de ángulo dentro de 100 ms
  }

  // Lectura del sensor ultrasónico HC-SR04
  if (estadoUltrasonico == 0 && millis() >= nextMillisSensorU) {
    digitalWrite(TRIGPIN, LOW);         // Pone el pin Trig en bajo para asegurar un pulso limpio
    nextMicrosSensorU = micros() + 2;   // Programa una espera de 2 microsegundos
    estadoUltrasonico = 1;              // Avanza a la fase 1 de disparo

  } else if (estadoUltrasonico == 1 && micros() >= nextMicrosSensorU) {
    digitalWrite(TRIGPIN, HIGH);        // Activa el pin Trig para emitir la ráfaga ultrasónica
    nextMicrosSensorU = micros() + 10;  // Programa mantener el pulso activo durante 10 microsegundos
    estadoUltrasonico = 2;              // Avanza a la fase 2 de lectura del eco

  } else if (estadoUltrasonico == 2 && micros() >= nextMicrosSensorU) {
    digitalWrite(TRIGPIN, LOW);         // Finaliza el pulso ultrasónico poniendo Trig en bajo

    long duracion = pulseIn(ECHOPIN, HIGH, 30000);  // Mide el tiempo (en us) que tarda en volver el eco (máx. 30 ms)
    float d = (duracion * 0.0343) / 2.0;            // Calcula la distancia en cm (velocidad sonido 0.0343 cm/us, ida y vuelta)

    if (duracion == 0 || d < 2.0 || d > 400.0) {    // Comprueba si no hubo eco (0) o está fuera de rango (2-400 cm)
      if (!esperandoTimeoutDist) {                  // Si es el primer fallo seguido, arranca el contador de 5 segundos
        esperandoTimeoutDist = true;
        nextTimeoutDist = millis() + 5000;          // 5 segundos de margen igual que el DHT11
      }
      if (millis() >= nextTimeoutDist) {            // Si lleva 5 segundos o más sin lecturas válidas
        enviarMensaje("13:");                       // Código 13: alarma de fallo del sensor de distancia
      } else {                                      // Mientras no pasen los 5 segundos, envía distancia 0 en el ángulo actual
        enviarMensaje("11:" + String(anguloActual) + ":0");
      }
    } else {                                        // Si la medición de distancia es válida
      esperandoTimeoutDist = false;                 // Cancela cualquier cuenta atrás de fallo del ultrasónico
      enviarMensaje("11:" + String(anguloActual) + ":" + String(d));  // Envía código 11 con ángulo y distancia en cm

      digitalWrite(ledtr, HIGH);                    // Enciende el LED para indicar que se transmitió un dato válido
      ledEncendido = true;                          // Marca el LED como encendido
      nextMillisLed = millis() + 100;               // Programa el apagado del LED tras 100 ms
    }

    estadoUltrasonico = 0;                           // Reinicia la máquina de estados del sensor ultrasónico
    nextMillisSensorU = millis() + intervalSensorU;  // Programa la próxima medición dentro de 1 segundo
  }
}