#include <ArduinoJson.h>
#include <BluetoothSerial.h>

#if defined(ARDUINO_ARCH_AVR)
#define SERIAL  Serial
#elif defined(ARDUINO_ARCH_SAMD) ||  defined(ARDUINO_ARCH_SAM)
#define SERIAL  SerialUSB
#else
#define SERIAL  Serial
#endif

float temp;
float humid;
float ps;
int want;
char msgforcontrol[200];
String m;
String recievemsg;
String messageforcontrol;
String myRecieve = "+NNMI";
String myRecieve_1 = "chMaskCntl";
long lastmsg = 0;
int i = 1;
String topic_temp;
String topic_hr;
String topic_spo2;
BluetoothSerial ESP32BT;

/////////// Function อ่านค่าตอบกลับจาก ChirpStack ////////////

void Serial_2frommaxiiot(void *pvParameter){
  while(1){
    if(Serial2.available() > 0){
      String data_re = Serial2.readString();        
      Serial.println(data_re);
      ESP32BT.println(data_re);
      
      ///////// เช็คค่าในกรณีที่ตอบกลับเป็น Message จาก Python ///////////
      if(data_re.startsWith(myRecieve)){
        for(int i = 0; i < data_re.length(); i++){
          if(data_re[i] == ','){
            want = i + 1;
            break;
          }
         
        }for(int i = want; i < data_re.length(); i++){
          recievemsg += data_re[i];          
        }
        recievemsg.toCharArray(msgforcontrol,200);
        for(int i = 0; i < sizeof(msgforcontrol); i++){
          if(msgforcontrol[i] == '\n' || msgforcontrol[i] == '\t' || msgforcontrol[i] == '\r'){
            Serial.println(messageforcontrol);
            
            if(messageforcontrol == "48"){
              digitalWrite(2, 1);
              
            }else if(messageforcontrol == "45"){
              digitalWrite(2, 0);
             
            }
            messageforcontrol = "";
            recievemsg = "";
            want = 0;
            break;
          }else{
            messageforcontrol += msgforcontrol[i];
          }
        }
      }else if(data_re.startsWith(myRecieve_1)){
        for(int i = 0; i < data_re.length(); i++){
          if(data_re[i] == '\n'){
            want = i + 1;
            break;
          }
         
        }for(int i = want; i < data_re.length(); i++){
          recievemsg += data_re[i];          
        }
        recievemsg.toCharArray(msgforcontrol,200);
        for(int i = 0; i < sizeof(msgforcontrol); i++){
          if(msgforcontrol[i] == '\n' || msgforcontrol[i] == '\t' || msgforcontrol[i] == '\r'){
            Serial.println(messageforcontrol);
            ESP32BT.println(messageforcontrol);
            if(messageforcontrol == "+NNMI:1,48"){
              digitalWrite(2, 1);
              
              
            }else if(messageforcontrol == "+NNMI:1,45"){
              digitalWrite(2, 0);
              
            }
            messageforcontrol = "";
            recievemsg = "";
            want = 0;
            break;
          }else{
            messageforcontrol += msgforcontrol[i];
          }
        }
      }
    }
    if(Serial.available() > 0){
      m = Serial.readString();
      
      ////////// พิมพ์ reboot เพื่อ reboot MaxiIoT /////////
      
      if(m == "reboot"){
        send_NRB();
        m = "";
      }else if(m == "D"){
        send_DEVEUI();
        m = "";
      }else if(m == "A"){
        send_APPKEY();
        m = "";
      }else{
        Serial2.println(m);
      }
    }
    if(ESP32BT.available() > 0){
      m = ESP32BT.readString();
      
      ////////// พิมพ์ reboot เพื่อ reboot MaxiIoT /////////
      
      if(m == "reboot"){
        send_NRB();
        m = "";
      }else if(m == "D"){
        send_DEVEUI();
        m = "";
      }else if(m == "A"){
        send_APPKEY();
        m = "";
      }else{
        Serial2.println(m);
      }
    }
    vTaskDelay(20 / portTICK_RATE_MS);
  }
}

void setup() {
  
  Serial.begin(115200);
  Serial2.begin(115200);

  Serial.setTimeout(20);
  Serial2.setTimeout(20);

  pinMode(2, OUTPUT);

  ESP32BT.begin("ESP32_Serialmonitor");

  ////////// สร้าง Task สำหรับอ่านค่าตอบกลับจาก ChirpStack ///////////
  xTaskCreatePinnedToCore(Serial_2frommaxiiot, "Serial_2frommaxiiot", 100000, NULL, 1, NULL, 0);
}
void loop() {
  
      float Tempetature = random(3500,4000) / 100.0;
      float HR = random(6000,12000) / 100.0;
      int SpO2 = random(9000,10000) / 100.0;
      StaticJsonDocument<200> data_send;
      topic_temp = "Temp-" + String(i);
      topic_hr = "HeartRate-" + String(i);
      topic_spo2 = "SpO2-" + String(i);
      data_send[topic_temp] = Tempetature;
      data_send[topic_hr] = HR;
      data_send[topic_spo2] = SpO2;
      String jsonStr;
      serializeJson(data_send, jsonStr);
      int length_str = jsonStr.length();
      String AT_cmd = "AT+NCMGS=" + String(length_str) + ",";
      String msg_send = AT_cmd + jsonStr;
      Serial2.println(msg_send);
      delay(100);
      Serial.println(msg_send);
      ESP32BT.println(msg_send);
      if(i == 5){
        i = 0;
      }
      i += 1;
      delay(10000);
}

/////////// Reboot MaxiIoT //////////
void send_NRB(){
  Serial2.println("AT+NRB");
  //Serial.println("OK Reboot");
}

/////////// DEVEUI of MaxiIoT //////////
void  send_DEVEUI(){
  Serial2.println("AT+DEVEUI");
}

/////////// APPKEY of  MaxiIoT //////////
void send_APPKEY(){
  Serial2.println("AT+APPKEY");
}
