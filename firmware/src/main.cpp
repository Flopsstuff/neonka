#include <Arduino.h>

const int LED_PIN = 13;
const int INPUT_PINS[] = {6, 7, 8, 9};
const int NUM_PINS = sizeof(INPUT_PINS) / sizeof(INPUT_PINS[0]);

static int prevState[4];
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
    for (int i = 0; i < NUM_PINS; i++) {
        pinMode(INPUT_PINS[i], INPUT_PULLUP);
        prevState[i] = digitalRead(INPUT_PINS[i]);
    }

    while (!Serial.available()) {
        delay(100);
    }
    for (int i = 10; i > 0; i--) {
        Serial.print("TKB starting in ");
        Serial.println(i);
        delay(200);
    }
    Serial.println("TKB ready, monitoring pins 6-9");
}

void loop() {
    checkSerial();

    for (int i = 0; i < NUM_PINS; i++) {
        int state = digitalRead(INPUT_PINS[i]);
        if (state != prevState[i]) {
            prevState[i] = state;
            Serial.print("Pin ");
            Serial.print(INPUT_PINS[i]);
            Serial.print(": ");
            Serial.println(state == HIGH ? "HIGH" : "LOW");
        }
    }

    delay(10);
}
