#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include "HX711.h"

// --- Pin Definitions for Arduino Mega 2560 ---

// RC522 RFID Module Pins (SPI)
#define RST_PIN 5
#define SS_PIN 53
// SCK=52, MOSI=51, MISO=50

// SG90 Servo Motor Pin
#define SERVO_PIN 9

// HX711 Load Cell Pins (Charge Sensor)
const int LOADCELL_DOUT_PIN = 3;
const int LOADCELL_SCK_PIN = 2;

// --- Object Instantiation ---
MFRC522 rfid(SS_PIN, RST_PIN); 
Servo myServo;
HX711 scale;

// --- Load Cell Variables ---
// IMPORTANT: Adjust this calibration factor so your scale returns weight in GRAMS.
// You will need to calibrate this with a known weight.
float fator_calibracao = 20.0; 
float previousWeight = 0.0;
const float VARIATION_THRESHOLD = 100.0; // 100 grams variation threshold

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // --- RFID & Servo Setup ---
  SPI.begin();
  rfid.PCD_Init();
  myServo.attach(SERVO_PIN);
  myServo.write(0); // Initial position

  // --- HX711 Setup ---
  Serial.println(F("Initializing Load Cell (HX711)..."));
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(fator_calibracao);
  scale.tare(); // Reset the scale to 0

  Serial.println(F("System Ready."));
  Serial.println(F("Scale tared. Place an object to measure weight."));
  Serial.println(F("Scan an RFID tag to activate the servo."));
  
  // Set initial weight reference
  if (scale.is_ready()) {
      previousWeight = scale.get_units(10);
  }
}

void loop() {
  static unsigned long lastPrintTime = 0;
  
  // --- 1. Load Cell (HX711) Logic ---
  if (scale.is_ready()) {
    // Reduced average to 3. Taking 15 samples takes 1.5 seconds and can cause timeout/garbage data!
    float currentWeight = scale.get_units(3); 
    long rawValue = scale.read(); // Get raw data to check for hardware errors
    
    // Print current weight every 2 seconds for monitoring
    if (millis() - lastPrintTime > 2000) {
      Serial.print(F("RAW: "));
      Serial.print(rawValue);
      Serial.print(F(" | PESO: "));
      Serial.print(currentWeight);
      Serial.println(F(" g"));
      lastPrintTime = millis();
    }

    // Calculate the absolute difference between the new reading and the previous weight
    float variation = abs(currentWeight - previousWeight);

    // If the variation is greater than or equal to 100g, trigger the alert
    if (variation >= VARIATION_THRESHOLD) {
      Serial.print(F("Variacao detectada: "));
      Serial.print(variation);
      Serial.println(F(" g"));
      Serial.println(F("ENVIAR_PEDIDO"));
      
      // Update the previous weight so it only alerts once per 100g change
      previousWeight = currentWeight;
    }
  }

  // --- 2. RFID & Servo Logic ---
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    // ... rest of RFID logic ...
    Serial.print(F("Card UID:"));
    for (byte i = 0; i < rfid.uid.size; i++) {
      // The python script expects "Card UID: AA BB CC DD", so ensure there is a space
      Serial.print(F(" "));
      if (rfid.uid.uidByte[i] < 0x10) {
        Serial.print(F("0"));
      }
      Serial.print(rfid.uid.uidByte[i], HEX);
    }
    Serial.println();

    Serial.println(F("Activating servo..."));
    myServo.write(90);
    delay(3000);
    myServo.write(0);
    Serial.println(F("Servo returned to initial position.\n"));

    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
  }
}