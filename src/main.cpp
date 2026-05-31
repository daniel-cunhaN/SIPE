#include <Arduino.h>
#include "HX711.h"

// Definição dos pinos de ligação (Balança -> Arduino)
const int LOADCELL_DOUT_PIN = 3;
const int LOADCELL_SCK_PIN = 2;

HX711 scale;

// Fator de calibração (Você precisará ajustar este valor depois)
float fator_calibracao = 20.0; 

void setup() {
  Serial.begin(9600);
  Serial.println("Inicializando a balança...");

  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

  // Define o fator de calibração
  scale.set_scale(fator_calibracao);
  
  // Zera a balança (Tara)
  scale.tare(); 

  Serial.println("Balança zerada. Coloque um peso ou pressione a celula.");
}

void loop() {
  Serial.print("Peso: ");
  // Faz uma média de 10 leituras e mostra com 3 casas decimais
  Serial.print(scale.get_units(10), 3); 
  Serial.println(" kg");
  
  delay(500);
}