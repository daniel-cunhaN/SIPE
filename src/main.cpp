#include <Arduino.h>
#include "HX711.h"

// Definição dos pinos de ligação
const int LOADCELL_DOUT_PIN = 3;
const int LOADCELL_SCK_PIN = 2;

HX711 scale;

// Fator de calibração (ESTE VALOR DEVE SER AJUSTADO)
// Se o fator for muito pequeno, o ruído natural do sensor será amplificado, causando instabilidade.
float fator_calibracao = 10000.0; 

void setup() {
  Serial.begin(9600);
  Serial.println("Inicializando a balança...");

  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

  // 1. Aguarda o chip HX711 estabilizar após ligar
  Serial.println("Aguardando estabilização do sensor (2 segundos)...");
  delay(3000);

  if (scale.is_ready()) {
    scale.set_scale(fator_calibracao);
    
    // 2. Descarta as primeiras leituras para evitar lixo de memória/ruído inicial
    Serial.println("Limpando leituras iniciais...");
    scale.read_average(10); 
    
    // 3. Zera a balança (Tara)
    Serial.println("Zerando a balança (Tara)...");
    scale.tare(); 
    Serial.println("Balança zerada. Coloque um peso conhecido sobre ela.");
  } else {
    Serial.println("Erro: HX711 não encontrado. Verifique as conexões.");
  }
}

void loop() {
  if (scale.is_ready()) {
    Serial.print("Peso: ");
    // Faz uma média de 10 leituras e mostra com 3 casas decimais
    Serial.print(scale.get_units(10), 3); 
    Serial.println(" kg");
  } else {
    Serial.println("HX711 não está pronto.");
  }
  
  delay(500);
}
