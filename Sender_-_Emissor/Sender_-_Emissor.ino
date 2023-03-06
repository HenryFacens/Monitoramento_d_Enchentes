#include "heltec.h"

#define BAND    915E6
#define MAX_DISTANCE 400 // Maximum distance we want to ping for (incentimeters).Maximum sensor distance is rated at 400 - 500 cm.
#define pinTrigger 17
#define pinEcho 13

float distancia;

void sonarBegin(byte trig, byte echo);
float calcularDistancia();
float leituraSimples();

void setup() {
  
  pinMode(LED, OUTPUT);
  Heltec.begin(true, true , true , true , BAND);
  Heltec.display -> init();
  Heltec.display -> flipScreenVertically();
  Heltec.display -> drawString(0, 0, "Setup concluído.");
  Heltec.display -> display();
  sonarBegin(pinTrigger, pinEcho);
  
}

void loop() {
  
  distancia = calcularDistancia();

  Heltec.display -> clear();
  Heltec.display -> setTextAlignment(TEXT_ALIGN_LEFT);
  Heltec.display -> setFont(ArialMT_Plain_16);
  Heltec.display -> drawString(30, 5, "Enviando");
  Heltec.display -> drawString(33, 30, (String) distancia);
  Heltec.display -> drawString(78, 30, "cm");
  Heltec.display -> display();
  
  Serial.print("Sending packet: ");
  Serial.println(distancia);
  LoRa.beginPacket();
  LoRa.setTxPower(14,RF_PACONFIG_PASELECT_PABOOST);
  LoRa.print(distancia);
  LoRa.endPacket();
  
  Serial.print("Distância: ");
  Serial.print(distancia);
  Serial.println(" cm");
  
}

void sonarBegin(byte trig, byte echo) {
  #define divisor 58
  #define intervaloMedida 35 // MÁXIMO 35 mS PARA ATÉ 6,0M MIN 5mS PARA ATÉ 0, 8 M
  #define qtdMedidas 750 // QUANTIDADE DE MEDIDAS QUE SERÃO FEITAS (7500 para 2, 5 min)

  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);

  digitalWrite(trig, LOW); // DESLIGA O TRIGGER E ESPERA 500 uS
  delayMicroseconds(500);
}

float calcularDistancia() {
  float leituraSum = 0;
  float resultado = 0;
  for (int index = 0; index < qtdMedidas; index++) {
    delay(intervaloMedida);
    leituraSum += leituraSimples();
  }
  resultado = (float) leituraSum / qtdMedidas;
  resultado = resultado;

  return resultado;
}

float leituraSimples() {
  long duracao = 0;
  float resultado = 0;

  digitalWrite(pinTrigger, HIGH);
  delayMicroseconds(20);
  digitalWrite(pinTrigger, LOW);
  duracao = pulseIn(pinEcho, HIGH);
  resultado = ((float) duracao / divisor);
  return resultado;
}
