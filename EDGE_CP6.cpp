#include <ArduinoJson.h>
#include <ESP32Servo.h>
#include "EspMQTTClient.h"


#define SERVO 12
#define pir 32
#define pirsaida 25


int entradaPIR = 0;
int saidaPIR = 0;
int quant_open = 0;
int quant_close = 0;

Servo s; // Variável Servo
int pos; // Posição Servo

char  DoorsJson[100];

EspMQTTClient client
(
  "André Luis",                                         //nome da sua rede Wi-Fi
  "ainda2742",                                          //senha da sua rede Wi-Fi
  "mqtt.tago.io",                                       //Endereço do servidor MQTT
  "Default",                                            //User é sempre default pois vamos usar token
  "7e41d1da-830e-4eac-b772-26c3840966bd",               // Código do Token
  "esp32",                                              //Nome do device
  1883                                                  //Porta de comunicação padrao
);




void setup()
{
  Serial.begin(9600);

  pinMode(pir , INPUT);
  pinMode(pirsaida, INPUT);
  s.attach(SERVO);


  s.write(0);

  Serial.println("Conectando WiFi");
  while (!client.isWifiConnected()) {
    Serial.println('.');
    client.loop();
    delay(1000);
  }
  Serial.println("Conectado!");

  Serial.println("Conectando com o broker MQTT");
  while (!client.isMqttConnected()) {
    Serial.println('.');
    client.loop();
    delay(1000);
  }
  Serial.println("Conectado!");

}

void onConnectionEstablished()
{}


void loop()
{

  if (Serial.available()) {
    String inputString = Serial.readStringUntil('\n');
    if (inputString == "OPEN") {
      pos =90;
      s.write(pos);
      delay(3000);
    }
  }

  entradaPIR = digitalRead(pir) ;
  saidaPIR = digitalRead(pirsaida);
if (entradaPIR == LOW)
  {
    pos = 0;
    s.write(pos);
  }
  if (saidaPIR == LOW)
  {
    pos = 0;
    s.write(pos);

  }

  
  delay(600);
  while ((entradaPIR == HIGH) && (saidaPIR == HIGH))
  {
    pos = 90;
    s.write(pos);
    entradaPIR = digitalRead(pir) ;
    saidaPIR = digitalRead(pirsaida);
    Serial.println("the doors are open");
    delay(600);
  }

  if (entradaPIR == HIGH)
  {
    quant_open += 1;
    Serial.print("quantidade de entradas");
    Serial.println(quant_open);

  }
  while (entradaPIR == HIGH)
  {
    pos = 90;
    s.write(pos);
    delay(1000);
    entradaPIR = digitalRead(pir) ;
  }



  if (saidaPIR == HIGH)
  {
    quant_close += 1;
    Serial.print("quantidade de saidas");
    Serial.println(quant_close);

    while (saidaPIR == HIGH)
    {
      pos = 90;
      s.write(pos);
      delay(1000);
      saidaPIR = digitalRead(pirsaida) ;
    }
  }



  StaticJsonDocument<300> documentoJson;
  documentoJson["variable"] = "entradas";
  documentoJson["value"] = quant_open;
  serializeJson(documentoJson, DoorsJson);
  Serial.println("Enviando dados de entradas de pessoas");
  Serial.println(DoorsJson);

  client.publish("Door", DoorsJson);

  delay(1000);

  StaticJsonDocument<300> documentoJsonOut;
  documentoJsonOut["variable"] = "saidas";
  documentoJsonOut["value"] = quant_close;
  serializeJson(documentoJsonOut, DoorsJson);
  Serial.println("Enviando dados de saida de pessoas");
  Serial.println(DoorsJson);

  client.publish("Door", DoorsJson);

  client.loop();


}