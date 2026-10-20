// Definiciones necesarias
#include <DHT.h> //Librería para leer el sensor DHT
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
void setup() {
   Serial.begin(9600);
   // Inicialización del sensor
   dht.begin();
}
void loop() {
   delay(2000);
   float h = dht.readHumidity(); //Devuelve la humedad en porcentaje
   float t = dht.readTemperature(); //devuelve la temperatura en grados Celsius
   // Si no se puede leer la humedad o la temperatura, las funciones devuelven NAN
   if (isnan(h) || isnan(t)) //isnan() comprueba si dan NAN | \t es un carácter de tabulación, que nos permite alinear los datos en el monitor serie |
      Serial.println("Error al leer el sensor DHT11");
   else {
      Serial.print("Humedad: ");
      Serial.print(h);
      Serial.print("%\t");
      Serial.print("Temperatura: ");
      Serial.print(t);
      Serial.println("°C"); // Introduce al final de la frase el caracter de fin de línea.
   }
}
