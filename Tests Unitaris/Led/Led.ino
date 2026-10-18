const int led1 = 13;  // LED en el pin 13 

int nextMillis1;
const long interval1 = 2000; // 2 segundos para el LED 13

bool stateLed1 = LOW;

void setup() {
    pinMode(led1, OUTPUT);
    // primer instante en el que habrá que cambiar
    nextMillis1 = millis() + interval1;
}

void loop() {
    // Control del LED en el pin 13 (2 segundos)
    if (millis() >= nextMillis1) {
        // Ya toca cambiar
        if (stateLed1 == LOW)
             stateLed1 = HIGH;
        else
             stateLed1 = LOW;
        digitalWrite(led1, stateLed1);
        // Siguiente instante en el que habrá que cambiar
        nextMillis1 = millis() + interval1;
    }
}