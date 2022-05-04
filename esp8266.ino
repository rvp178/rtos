#define BLYNK_TEMPLATE_ID "TMPLXXwBUeHp"
#define BLYNK_DEVICE_NAME "BTL"
#define BLYNK_AUTH_TOKEN "HTp6zViCYt7X0ogyWjzfpw0XDVL_1X4X"

#define BLYNK_PRINT Serial
#define LivingroomLight           D0
#define LivingroomAirConditioner  D1
#define Television                D2   
#define BedroomLight              D3
#define BedroomAirConditioner     D6
#define airVent                   D7
int button;

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include<SoftwareSerial.h>
#include "ArduinoJson.h"
SoftwareSerial s(D5, D4);

char auth[] = "HTp6zViCYt7X0ogyWjzfpw0XDVL_1X4X";


char ssid[] = "dtt";
char pass[] = "12345678";
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);
  pinMode(LivingroomLight,OUTPUT);
  pinMode(LivingroomAirConditioner,OUTPUT);
  pinMode(Television,OUTPUT);
  pinMode(BedroomLight,OUTPUT);
  pinMode(BedroomAirConditioner,OUTPUT);
  pinMode(airVent,OUTPUT);
  s.begin(9600);
}
String Data = "";
void loop() {
  // put your main code here, to run repeatedly:
   Blynk.run();
  while(s.available())
  {
    const size_t capacity = JSON_OBJECT_SIZE(2)+256;
    DynamicJsonDocument JSON(capacity);
    DeserializationError error = deserializeJson(JSON,s);
    if(error)
    {
      return;
    }
    else
    {
      serializeJsonPretty(JSON,Serial);
      Serial.println();
      int t = JSON["Temperature"];
      int h = JSON["Humidity"];
      int l = JSON["Light"];
      int g = JSON["Gas"];
      if(t>40 || h<40 || g >1000)
  {
    Blynk.virtualWrite(V9, "1");
  }
  else{
    Blynk.virtualWrite(V9, "0");
  }
  Blynk.virtualWrite(V3, h);
  Blynk.virtualWrite(V2, t);
  Blynk.virtualWrite(V4, g);
  Blynk.virtualWrite(V10, l);
    }
  } 
}
BLYNK_WRITE(V0)
{
  button = param.asInt(); 
    digitalWrite(LivingroomLight,button);
}
BLYNK_WRITE(V1)
{
  button = param.asInt();
    digitalWrite(LivingroomAirConditioner,button);
}
BLYNK_WRITE(V5)
{
  button = param.asInt();
    digitalWrite(Television,button);
}
BLYNK_WRITE(V6)
{
  button = param.asInt();
    digitalWrite(BedroomLight,button);
}
BLYNK_WRITE(V7)
{
  button = param.asInt();
    digitalWrite(BedroomAirConditioner,button);
}
BLYNK_WRITE(V8)
{
  button = param.asInt();
    digitalWrite(airVent,button);
}
