#include <Arduino_FreeRTOS.h>
#include <queue.h>
#include "dht11.h"
#include "ArduinoJson.h"
#define VIN 3.3 // V power voltage, 3.3v in case of NodeMCU
#define R 10000 // Voltage devider resistor value
dht11 dht;
uint8_t t;
const int Analog_Pin = A1; // Analog pin A0
int LDR_Val; // Analog value from the LDR
int luminance; //Lux value
const int         MQ_PIN = A0;     // xác định kênh đầu vào analog sử dụng
#define         RL_VALUE                     (5)     // xác định điện trở tải trên bảng, tính bằng kilo ohms
float           LPGCurve[3]  =  {2.3,0.21,-0.47};    //hai điểm được lấy từ đường cong. 
                                                      //Định dạng dữ liệu: {x, y, angle}; point1: (lg200, 0,21), point2: (lg10000, -0,59)                                                                                                                                                                                                       
int           Ro = 22; 

void Taskdht(void *pvParameters);
void TaskLight(void *pvParameters);
void TaskSerial(void *pvParameters);
QueueHandle_t arrayTempQueue;
QueueHandle_t arrayLightQueue;
QueueHandle_t arrayHumiQueue;
QueueHandle_t arrayGasQueue;
String DataSend = "";
void setup() {
   Serial.begin(9600);
   arrayTempQueue=xQueueCreate(60, sizeof(uint8_t));
   arrayHumiQueue=xQueueCreate(60, sizeof(uint8_t));
   arrayLightQueue = xQueueCreate(30,sizeof(uint8_t));
   arrayGasQueue = xQueueCreate(30,sizeof(uint16_t));
   xTaskCreate(TaskSerial,// Task function
              "PrintSerial",// Task name
              128,// Stack size 
              NULL,
              3,// Priority
              NULL);
  xTaskCreate(Taskdht, // Task function
              "dht",// Task name
              128,// Stack size 
              NULL,
              2,// Priority
              NULL);
  xTaskCreate(TaskLight, // Task function
              "Light",// Task name
              128,// Stack size 
              NULL,
              1,// Priority
              NULL);
}

void loop() {}
void Taskdht(void *pvParameters)
{
  uint8_t dhtTemp;
  uint8_t dhtHumi;
  for(;;)
  {
    dht.read(2);
    dhtTemp = dht.temperature;
    dhtHumi = dht.humidity;
    delay(500);
    xQueueSend(arrayTempQueue,&dhtTemp,10);
    xQueueSend(arrayHumiQueue,&dhtHumi,10);
    vTaskDelay(15);
  }
}
void TaskLight(void *pvParameters)
{
    uint16_t MQ_val = analogRead(A0);
    uint16_t g = MQRead(MQ_val);
//    Serial.println(g);
    LDR_Val = analogRead(Analog_Pin);
    luminance = conversion(LDR_Val);
    xQueueSend(arrayLightQueue,&luminance,10);
    xQueueSend(arrayGasQueue,&g,10);
    vTaskDelay(15);
}
void TaskSerial(void *pvParameters)
{
  uint8_t Temp;
  uint8_t Humi;
  uint8_t Light;
  uint16_t Gas1;
  for(;;)
  {
      xQueueReceive(arrayTempQueue,&Temp,portMAX_DELAY);
      xQueueReceive(arrayLightQueue,&Light,portMAX_DELAY);
      xQueueReceive(arrayHumiQueue,&Humi,portMAX_DELAY);
      xQueueReceive(arrayGasQueue,&Gas1,portMAX_DELAY);
      DataSend = "";
      DataSend  = "{\"Temperature\":\"" + String(Temp) + "\"," +
                  "\"Humidity\":\"" + String(Humi) + "\"," +
                  "\"Light\":\"" + String(Light) + "\"," +
                  "\"Gas\":\"" + String(Gas1) +  "\"}";
       Serial.println(DataSend);
       Serial.flush();
       vTaskDelay(1000/portTICK_PERIOD_MS);
  }
}
int conversion(int raw_val){
  // Conversion rule
  float Vout = float(raw_val) * (VIN / float(1023));// Conversion analog to voltage
  float RLDR = (R * (VIN - Vout))/Vout; // Conversion voltage to resistance
  int lux = 500/(RLDR/1000); // Conversion resitance to lumen
  return lux;
}
int MQRead(uint16_t Val)
{
    float rs = RL_VALUE*(1023-Val)/Val;
    float rs_ro_ratio = rs/Ro;
    int ppm = pow(10,( ((log(rs_ro_ratio)-0.21)/(-0.47)) + 2.3));
  return ppm;  
}
