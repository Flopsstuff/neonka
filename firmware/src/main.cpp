#include <Arduino.h>

const int LED_PIN = 13;
const unsigned long BLINK_INTERVAL = 1000;

void setup() {
    pinMode(LED_PIN, OUTPUT);
    Serial.begin(115200);
}

void loop() {
    digitalWrite(LED_PIN, HIGH);
    Serial.println("LED ON");
    delay(BLINK_INTERVAL);

    digitalWrite(LED_PIN, LOW);
    Serial.println("LED OFF");
    delay(BLINK_INTERVAL);
}
