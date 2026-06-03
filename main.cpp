#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

#define RST_PIN 5
#define SS_PIN 53
#define SERVO_PIN 9

MFRC522 rfid(SS_PIN, RST_PIN); 
Servo myServo;

bool servoAberto = false; 

void setup() {
  // ATENÇÃO: A velocidade do Serial deve ser a mesma do Python (9600)
  Serial.begin(9600);
  while (!Serial);

  SPI.begin();
  rfid.PCD_Init();

  myServo.attach(SERVO_PIN);
  myServo.write(0);
}

void loop() {
  if (!rfid.PICC_IsNewCardPresent()) {
    return;
  }

  if (!rfid.PICC_ReadCardSerial()) {
    return;
  }

  // --- MENSAGEM PARA O PYTHON ---
  // Imprime o texto exato que o Python está esperando ("Card UID: ")
  Serial.print(F("Card UID: "));
  
  for (byte i = 0; i < rfid.uid.size; i++) {
    // Adiciona um zero à esquerda para manter a formatação bonita, mas removemos 
    // os espaços extras para o Python ler a TAG como um bloco contínuo (ex: 0A1B2C3D)
    Serial.print(rfid.uid.uidByte[i] < 0x10 ? "0" : "");
    Serial.print(rfid.uid.uidByte[i], HEX);
  }
  Serial.println(); // Pula a linha (isso dispara a leitura no Python)

  // --- LÓGICA DO SERVO ---
  if (!servoAberto) {
    myServo.write(90);
    servoAberto = true;
  } else {
    myServo.write(0);
    servoAberto = false;
  }

  delay(1000); 

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}