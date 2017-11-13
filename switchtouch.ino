
/*Desenvolvido por Bruno Horta @ 2017*/

//Wi-Fi Manger library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>//https://github.com/tzapu/WiFiManager

//MQTT
#include <PubSubClient.h>

//ESP
#include <ESP8266WiFi.h>

#define MQTT_AUTHENTICATION false //colocar a true se o broker tiver autenticação

#define MQTT_USERNAME "user"

#define MQTT_PASSWORD "password"

#define LIGHT D1

#define BUTTON D2


//Constantes 

const String HOSTNAME  = "SWITCHTOUCH"; //Nome do dispositivo

const String MQTT_LIGHT_TOPIC = "sala/luz/set";

const String MQTT_LIGHT_STATE_TOPIC = "sala/luz";

const char* MQTT_SERVER = "ip_broker"; //IP do Broker


WiFiClient wclient;

PubSubClient client(MQTT_SERVER,1883,wclient);

void setup() {

  Serial.begin(115200);
  

  pinMode(LIGHT,OUTPUT); 

  pinMode(BUTTON, INPUT); 

  WiFiManager wifiManager;
  //reset saved settings
  //wifiManager.resetSettings();
  /*define o tempo limite até o portal de configuração ficar novamente inátivo,
   útil para quando alteramos a password do AP*/
  wifiManager.setTimeout(180);

 wifiManager.autoConnect(HOSTNAME.c_str());
  attachInterrupt(digitalPinToInterrupt(BUTTON), handleInterrupt, CHANGE);
  client.setCallback(callback);
 
}
void handleInterrupt() {
  Serial.println("toogle");
  if(digitalRead(LIGHT)){
   turnOff();
  }else{
   turnOn();
  }  
}


//Chamada de recepção de mensagem 

void callback(char* topic, byte* payload, unsigned int length) {

  String payloadStr = "";

  for (int i=0; i<length; i++) {

    payloadStr += (char)payload[i];

  }

  Serial.println(payloadStr);

  String topicStr = String(topic);

 if(topicStr.equals(MQTT_LIGHT_TOPIC)){
  Serial.println(payloadStr);
  if(payloadStr.equals("ON")){
  
      turnOn();

    }else if(payloadStr.equals("OFF")) {
   
      turnOff();

    }

  } 

} 

void turnOn(){

  digitalWrite(LIGHT,HIGH);
  Serial.println("ON");
  client.publish(MQTT_LIGHT_STATE_TOPIC.c_str(),"ON");

}



void turnOff(){
   digitalWrite(LIGHT,LOW);  
   Serial.println("OFF");
   client.publish(MQTT_LIGHT_STATE_TOPIC.c_str(),"OFF");

}



bool checkMqttConnection(){

  if (!client.connected()) {

    if (MQTT_AUTHENTICATION ? client.connect(HOSTNAME.c_str(),MQTT_USERNAME, MQTT_PASSWORD) : client.connect(HOSTNAME.c_str())) {

      //SUBSCRIÇÃO DE TOPICOS

      Serial.println("CONNECTED");

      client.subscribe(MQTT_LIGHT_TOPIC.c_str());

    }

  }

  return client.connected();

}




void loop() {
 if (WiFi.status() == WL_CONNECTED) {
    if(checkMqttConnection()){
      client.loop();
    }
  }
}


