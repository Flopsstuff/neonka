#include <Arduino.h>

const int LED_PIN = 13;
const unsigned long BLINK_INTERVAL = 1000;

static String serialBuf;

static void checkSerial() {
    while (Serial.available()) {
        char c = Serial.read();
        if (c == '\n' || c == '\r') {
            serialBuf.trim();
            if (serialBuf == "REBOOT") {
                Serial.println("Entering bootloader...");
                Serial.flush();
                delay(100);
                _reboot_Teensyduino_();
            }
            serialBuf = "";
        } else {
            serialBuf += c;
        }
    }
}

void setup() {
    pinMode(LED_PIN, OUTPUT);
    Serial.begin(115200);
}

void loop() {
    checkSerial();

    digitalWrite(LED_PIN, HIGH);
    Serial.println("LED ON");
    delay(BLINK_INTERVAL);

    checkSerial();

    digitalWrite(LED_PIN, LOW);
    Serial.println("LED OFF");
    delay(BLINK_INTERVAL);
}
