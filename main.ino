#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal.h>

// --- RFID Setup ---
#define SS_PIN 53 // SDA pin on Arduino Mega
#define RST_PIN 8 // Reset pin
MFRC522 mfrc522(SS_PIN, RST_PIN);

// --- LCD Setup ---
// RS, E, D4, D5, D6, D7
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

void setup() {
  Serial.begin(9600);
  // Initialize SPI bus for RFID
  SPI.begin();
  // Initialize RFID reader
  mfrc522.PCD_Init();
  
  // Initialize LCD
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Sistema Pronto!");
  lcd.setCursor(0, 1);
  lcd.print("Aproxime a tag!");
  
  Serial.println("Esperando a tag...");
}

void loop() {
  // Look for new RFID cards
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  // Clear LCD and show success message
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Tag Detectada!");

  // Print the UID (Unique ID) on the second row
  lcd.setCursor(0, 1);
  lcd.print("ID: ");

  String uidString = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    // Add a zero if the hex value is single digit
    if (mfrc522.uid.uidByte[i] < 0x10) {
      uidString += "0";
    }
    uidString += String(mfrc522.uid.uidByte[i], HEX);
  }

  // Convert to uppercase and print to LCD
  uidString.toUpperCase();
  lcd.print(uidString);
  Serial.println("Card UID: " + uidString);

  // Stop reading this specific card so we don't spam the screen
  mfrc522.PICC_HaltA();

  // Wait 2 seconds, then prompt to scan again
  delay(2000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Sistema Pronto!");
  lcd.setCursor(0, 1);
  lcd.print("Aproxime a tag!");
}
