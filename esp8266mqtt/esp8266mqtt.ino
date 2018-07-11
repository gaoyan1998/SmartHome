// This example uses an Adafruit Huzzah ESP8266
// to connect to shiftr.io.
//
// You can check on your device after a successful
// connection here: https://shiftr.io/try.
//
// by Joël Gähwiler
// https://github.com/256dpi/arduino-mqtt

#include <ESP8266WiFi.h>
#include <MQTT.h>
String comdata = "";
char *tempdata ;
const char *m = "#";
char *p;
char *con[3];
int liveTime = 30000;
int isSub = 0;

const char ssid[] = "ZUK";
const char pass[] = "147258369";
WiFiClient net;
MQTTClient client;

unsigned long lastMillis = 0;

void connect() {
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".0");
    delay(1000);
  }

  Serial.print("\nconnecting...");
  while (!client.connect("sensor", "admin", "public")) {
    Serial.print(".1");
    delay(1000);
  }

  Serial.println("\nconnected!");

  client.subscribe("/heart");
  client.subscribe("/sensor");
  client.subscribe("/EqCt");
}
void submqtt(char *s){
  Serial.print("注册:");
  Serial.println(s);
   if (!client.connected()) {
    connect();
  }
    client.subscribe(s);
}
void unsubmqtt(char *s){
  Serial.print("反注册:");
  Serial.println(s);
   if (!client.connected()) {
    connect();
  }
  client.unsubscribe(s);
}
void messageReceived(String &topic, String &payload) {
  if(topic == "/EqCt")
      Serial.println("recv#" + topic + "#" + payload);
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, pass);

  // Note: Local domain names (e.g. "Computer.local" on OSX) are not supported by Arduino.
  // You need to set the IP address directly.
  client.begin("123.206.55.114", net);
  client.onMessage(messageReceived);

  connect();
}

void loop() {
  client.loop();
  delay(10);  // <- fixes some issues with WiFi stability

  if (!client.connected()) {
    connect();
  }
  if (millis() - lastMillis > liveTime) {
    lastMillis = millis();
    client.publish("/heart", "keeplive");
  }
  comdata = "";
  while (Serial.available() > 0)  
    {
        comdata += char(Serial.read());
        delay(2);
    }
    if (comdata != "")
    {
      tempdata = new char[comdata.length()];strcpy(tempdata,comdata.c_str());
      p = strtok(tempdata,m);
      int i = 0;
      while(p){
        con[i] = p;
       p = strtok(NULL,m);
       i++;
       }
       if(!strcmp(con[1],"submqtt")){
          submqtt(con[2]);
        }else if(!strcmp(con[1],"unsubmqtt")){
          unsubmqtt(con[2]);
        }else if(!strcmp(con[0],"send")){
          //Serial.println("发送mqtt:");
          client.publish(con[1],con[2]);
        }
     }
}
