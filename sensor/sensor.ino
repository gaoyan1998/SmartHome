#include <SimpleDHT.h>
#include <ArduinoJson.h>
int pinDHT11 = 3;
SimpleDHT11 dht11;
char *buf;
String comdata;
char *con[3];
char *tempdata ;
const char *m = "#";
char *p;
unsigned long sendlastMillis = 0;
int sendTime = 3000;
byte temperature;
byte humidity;
int err = SimpleDHTErrSuccess;
StaticJsonBuffer<110> jsonBuffer;
JsonObject& root = jsonBuffer.createObject();
String json  = "";

void setup() {
  Serial.begin(115200);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  delay(2000);
}
//void p(char *s, int a) {
//  buf = new char[strlen(s) + sizeof(a) + 1];
//  sprintf(buf, "%s%d", s, a);
//}
void sendSensor() {
  temperature = 0;
  humidity = 0;
  json = "";
  if ((err = dht11.read(pinDHT11, &temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
    Serial.print("Read DHT11 failed, err="); Serial.println(err); delay(1000);
  } else {
    root["temperature"] =(int)temperature;
    root["humidity"] = (int)humidity;
    root["com4"] = digitalRead(4);
    root["com5"] = digitalRead(5);
    root["com6"] = digitalRead(6);
    root.printTo(json);
    //Serial.print("发送数据");delay(100);
    Serial.println("send#/sensor#"+json);
  }
}
void dealAction(int action,int id){
  if(action == 2)
    {
      Serial.println("关");
      digitalWrite(id, LOW);
     }else if(action == 3){
      Serial.println("开");
      digitalWrite(id,HIGH);
     }
}
void recv() {
  comdata = "";
  while (Serial.available() > 0)
  {
    comdata += char(Serial.read());
    delay(10);
  }
  if (comdata == "")
    return;
  Serial.print("收到数据：");
  Serial.println(comdata);
  tempdata = new char[comdata.length()]; strcpy(tempdata, comdata.c_str());
  p = strtok(tempdata, m);
  int i = 0;
  while (p) {
    con[i] = p;
    p = strtok(NULL, m);
    i++;
  }
  if (!strcmp(con[1], "/EqCt")) {
    StaticJsonBuffer<100> jsonBuffer2;    
    JsonObject& root2 = jsonBuffer2.parseObject(con[2]);
    int type = root2["type"];
    int action = root2["action"];
    int id = root2["id"];
    switch(type){
      case 2:dealAction(action,id);
      break;
      case 3:
      break;
     }
  }
}
void loop() {
  if (millis() - sendlastMillis > sendTime) {
    sendlastMillis = millis();
    sendSensor();
  }
  recv();
  delay(500);
  //  }
}

