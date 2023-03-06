#include "heltec.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

#define BAND                915E6
#define TARA_LAGO           244

String packSize         = "--";
String packet;
String distancia;
String nivel;

const char* ssid        = "AP-SMART";
const char* password    = "smart@2022";
const char* mqttServer  = "34.234.193.23";
const int mqttPort      = 1883;

const char* topic1      = "smart/distancia";
const char* topic2      = "smart/nivel";

WiFiClient                espClient;
PubSubClient              client(espClient);

void callback(char* topic, byte* payload, unsigned int length);
void reconnect();


void setup() 
{
  Heltec.begin(true ,true, true, true, BAND);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.println("Conectando ao WiFi...");
  }
  
  Serial.println("Conectado ao WiFi");

  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
}

void loop() {
if (!client.connected()) {
    reconnect();
  }
  else{

  client.loop();

  float packetSize = LoRa.parsePacket();

  packet = "";
  packSize = String(packetSize, DEC);
  for (int i = 0; i < packetSize; i++) {
    packet += (char) LoRa.read();
  }
  
  Heltec.display -> clear();
  Heltec.display -> setTextAlignment(TEXT_ALIGN_LEFT);
  Heltec.display -> setFont(ArialMT_Plain_10);
  Heltec.display -> setFont(ArialMT_Plain_16);
  Heltec.display -> drawString(0, 40, "Dist:");
  Heltec.display -> drawString(40, 40, packet);
  Heltec.display -> drawString(90, 40, " cm");
  Heltec.display -> display();
  
  nivel = packet.toFloat() - TARA_LAGO;

  if (packetSize) {
    Serial.print("Received packet '");
    while (LoRa.available()) {
      Serial.print(packet);
    }
    Serial.print("' with RSSI ");
    Serial.println(LoRa.packetRssi());

    client.publish("/smart/lago/dist", packet.c_str());
    client.publish("/smart/lago/nivel", nivel.c_str());

  }
}
}
void reconnect() {
  while (!client.connected()) {
    Serial.println("Connecting to MQTT broker...");
    if (client.connect("ESP32Client")) {
      Serial.println("Connected to MQTT broker");
    }
    else {
      
      Serial.print("Failed to connect to MQTT broker, rc=");
      Serial.print(client.state());
      Serial.println(" retrying in 5 seconds");
      delay(5000);
      
    }
  }
}
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received on topic: ");
  Serial.println(topic);
  Serial.print("Message content: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}
