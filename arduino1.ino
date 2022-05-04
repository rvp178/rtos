#include "ArduinoJson.h"
#include "DHT.h"
#define DHTPIN 2 // what digital pin we're connected to
#define DHTTYPE DHT11 // DHT 11
DHT dht(DHTPIN, DHTTYPE);

#define VIN 3.3 // V power voltage, 3.3v in case of NodeMCU
#define R 10000 // Voltage devider resistor value
const int Analog_Pin = A1; // Analog pin A0
int LDR_Val; // Analog value from the LDR
float luminance; //Lux value
String DataSend = "";
#define         MQ_PIN                       (A0)     // xác định kênh đầu vào analog sử dụng
#define         RL_VALUE                     (5)     // xác định điện trở tải trên bảng, tính bằng kilo ohms
float           LPGCurve[3]  =  {2.3,0.21,-0.47};    //hai điểm được lấy từ đường cong. 
                                                      //Định dạng dữ liệu: {x, y, angle}; point1: (lg200, 0,21), point2: (lg10000, -0,59)                                                                                                                                                                                                       
float           Ro; 
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Ro = MQCalibration(MQ_PIN);    // Hiệu chỉnh cảm biến. 
                                 //Hãy đảm bảo rằng cảm biến ở trong không khí sạch khi hiệu chỉnh
  Serial.print("R0: ");
  Serial.println(Ro);
  dht.begin();
}
unsigned long lastMsg = 0;
void loop() {
  // put your main code here, to run repeatedly:
  long now = millis();
    if (now - lastMsg > 2000)
    {
      lastMsg = now;
      float h = dht.readHumidity();       // đọc độ ẩm
      float t = dht.readTemperature();    // đọc nhiệt độ
      float g = MQGetPercentage(MQRead(MQ_PIN)/Ro,LPGCurve); // đọc giá trị mq2 (ppm)
      LDR_Val = analogRead(Analog_Pin);
      luminance = conversion(LDR_Val);
      if (isnan(h) || isnan(t)) {
        Serial.println("Failed to read from DHT sensor!");
        return;
        }

      DataSend = "";
      DataSend  = "{\"Temperature\":\"" + String(t) + "\"," +
                  "\"Humidity\":\"" + String(h) + "\"," +
                  "\"Light\":\"" + String(luminance) + "\"," +
                  "\"Gas\":\"" + String(g) +  "\"}";
       Serial.println(DataSend);
       Serial.flush();
      //char Tempstring[10];
      //sprintf(Tempstring,"%d",t);
      //char Humistring[10];
      //sprintf(Humistring, "%d", h);
      //char lightString[10];
      //sprintf(lightString, "%f", luminance);
      //char Gasstring[10];
      //sprintf(Gasstring, "%d",g);
      
//      StaticJsonDocument<50> doc;
//      doc["Temperature"] = t;
//      doc["Humidity"] = h;
//      doc["Light"] = luminance;
//      doc["Gas"] = g;
//      char buffer[64];
//      serializeJson(doc, buffer);
//       Serial.write(buffer);
//       Serial.println();
    }
}
// Đầu ra điện trở của cảm biến trong không khí
float MQResistanceCalculation(int raw_adc)
{
  return ( ((float)RL_VALUE*(1023-raw_adc)/raw_adc));
}

// Đầu ra điện trở của cảm biến trong không khí sạch sau tính toán
float MQCalibration(int mq_pin)
{
  int i;
  float val=0;
  for (i=0;i<50;i++) {           
    val += MQResistanceCalculation(analogRead(mq_pin));
    delay(300);
  }
  val = val/50;    // lấy giá trị trung bình               
  val = val/9.83;  // 9.83 là điện trở cảm biến trong không khí/RO được lấy từ biểu đồ                                                                           
  return val; 
}
// Đầu ra là điện trở của cảm biến trong không khí
float MQRead(int mq_pin)
{
  int i;
  float rs=0;
  for (i=0;i<5;i++) {
    rs += MQResistanceCalculation(analogRead(mq_pin));
    delay(50);
  }
  rs = rs/5;
  return rs;  
}
// Tính toán giá trị ppm
int  MQGetPercentage(float rs_ro_ratio, float *pcurve)
{
  return (pow(10,( ((log(rs_ro_ratio)-pcurve[1])/pcurve[2]) + pcurve[0])));
}
int conversion(int raw_val){
  // Conversion rule
  float Vout = float(raw_val) * (VIN / float(1023));// Conversion analog to voltage
  float RLDR = (R * (VIN - Vout))/Vout; // Conversion voltage to resistance
  int lux = 500/(RLDR/1000); // Conversion resitance to lumen
  return lux;
}
