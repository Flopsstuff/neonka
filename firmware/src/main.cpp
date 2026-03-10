#if defined(__clang__) && !defined(SERIAL_9BIT_SUPPORT)
#define SERIAL_9BIT_SUPPORT
#endif

#include <Arduino.h>

const int LED_PIN = 13;
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

// Test words: mix of bit8=0 (data) and bit8=1 (address)
const uint16_t testWords[] = {
    0x000,  // data: 0x00, bit8=0
    0x0FF,  // data: 0xFF, bit8=0
    0x055,  // data: 0x55, bit8=0
    0x0AA,  // data: 0xAA, bit8=0
    0x121,  // address: 0x21, bit8=1 (Motor Control Board address)
    0x1FF,  // address: 0xFF, bit8=1
    0x100,  // address: 0x00, bit8=1
};
const int numTests = sizeof(testWords) / sizeof(testWords[0]);

void setup() {
    pinMode(LED_PIN, OUTPUT);
    Serial.begin(115200);
    Serial2.begin(187500, SERIAL_9N1);

    while (!Serial) delay(10);
    delay(500);

    Serial.println("=== 9-bit UART loopback test ===");
    Serial.println("Serial2: TX=pin8, RX=pin7, 187500 baud, 9N1");
    Serial.println();

    int passed = 0;
    int failed = 0;

    for (int i = 0; i < numTests; i++) {
        uint16_t sent = testWords[i];

        // Clear RX buffer
        while (Serial2.available()) Serial2.read();

        // Send 9-bit word
        Serial2.write9bit(sent);

        // Wait for loopback
        unsigned long t0 = millis();
        while (!Serial2.available() && millis() - t0 < 50);

        if (!Serial2.available()) {
            Serial.print("FAIL #");
            Serial.print(i);
            Serial.print(": sent 0x");
            Serial.print(sent, HEX);
            Serial.println(" — no response (timeout)");
            failed++;
            continue;
        }

        uint16_t received = Serial2.read();

        if (received == sent) {
            Serial.print("OK   #");
            Serial.print(i);
            Serial.print(": 0x");
            Serial.print(sent, HEX);
            Serial.print(" -> 0x");
            Serial.print(received, HEX);
            Serial.print("  bit8=");
            Serial.println((sent >> 8) & 1);
            passed++;
        } else {
            Serial.print("FAIL #");
            Serial.print(i);
            Serial.print(": sent 0x");
            Serial.print(sent, HEX);
            Serial.print(" got 0x");
            Serial.println(received, HEX);
            failed++;
        }

        delay(5);
    }

    Serial.println();
    Serial.print("Results: ");
    Serial.print(passed);
    Serial.print(" passed, ");
    Serial.print(failed);
    Serial.println(" failed");
    Serial.println(failed == 0 ? "9-bit UART OK!" : "9-bit UART FAILED");
}

void loop() {
    checkSerial();
    delay(10);
}
