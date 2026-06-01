#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

// --- Pin Definitions for Arduino Mega 2560 ---
// RC522 RFID Module Pins (SPI)
#define RST_PIN 49   // Reset pin
#define SS_PIN 48  // SDA / SS pin (Standard for Mega)
// Note: Other SPI pins on the Mega 2560 are:
// SCK  = 52
// MOSI = 51
// MISO = 50

// SG90 Servo Motor Pin
#define SERVO_PIN 9 // Can be any PWM-capable pin

// --- Object Instantiation ---
MFRC522 rfid(SS_PIN, RST_PIN); 
Servo myServo;

void setup() {
  Serial.begin(9600);   // Initialize serial communications
  while (!Serial);      // Wait for serial port to connect

  SPI.begin();          // Initialize SPI bus
  rfid.PCD_Init();      // Initialize MFRC522 reader

  // Initialize Servo
  myServo.attach(SERVO_PIN);
  myServo.write(0);     // Set servo to initial position (0 degrees)

  Serial.println(F("System Initialized."));
  Serial.println(F("Please scan an RFID tag to activate the servo."));
}

void loop() {
  // Look for new RFID cards
  if (!rfid.PICC_IsNewCardPresent()) {
    return;
  }

  // Verify if the NUID has been read
  if (!rfid.PICC_ReadCardSerial()) {
    return;
  }

  // Print the UID of the tag to the Serial Monitor
  Serial.print(F("Tag UID detected:"));
  for (byte i = 0; i < rfid.uid.size; i++) {
    Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(rfid.uid.uidByte[i], HEX);
  }
  Serial.println();

  // --- Servo Action ---
  Serial.println(F("Activating servo..."));
  myServo.write(90);    // Move servo to 90 degrees
  
  delay(3000);          // Keep it open for 3 seconds
  
  myServo.write(0);     // Return servo to 0 degrees
  Serial.println(F("Servo returned to initial position."));
  Serial.println(F("Ready for next tag...\n"));

  // Halt PICC to stop reading the same card over and over
  rfid.PICC_HaltA();
  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
}
