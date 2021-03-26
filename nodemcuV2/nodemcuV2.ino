#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <Time_irrigacao.h>

// Altere estes dados pelos obtidos ao cadastrar o novo dispopsitivo no Watson Iot
const String ORG = "967k08";
const String DEVICE_TYPE = "esp01-rele";
const String DEVICE_ID = "esp-01-rele01";
#define DEVICE_TOKEN "*MVOPHVA2leXeYeg4I"
Time_irrigacao pinRele1(2,1,1);
// Pinos dos Relés

#define pinRele2 13
#define pinLED   2  //LED para mostrar conexão



const String CLIENT_ID = "d:" + ORG + ":" + DEVICE_TYPE + ":" + DEVICE_ID;
const String MQTT_SERVER = ORG + ".messaging.internetofthings.ibmcloud.com";

#define COMMAND_TOPIC_R1 "iot-2/cmd/commandR1/fmt/json"
#define COMMAND_TOPIC_R2 "iot-2/cmd/commandR2/fmt/json"

WiFiClient wifiClient;
PubSubClient client(MQTT_SERVER.c_str(), 1883, wifiClient);

void setup() {
  Serial.begin(115200);


  pinMode(pinRele2, OUTPUT);
  pinMode(pinLED, OUTPUT);

  digitalWrite(pinLED, LOW);
  digitalWrite(12, HIGH);


  WiFiManager wifiManager;
  wifiManager.setAPCallback(configModeCallback);
  wifiManager.setSaveConfigCallback(saveConfigCallback);

  //wifiManager.resetSettings();
  wifiManager.autoConnect("node-mcu-AP");


  connectMQTTServer();
}

void loop() {
  client.loop();
  pinRele1.loop();
}

//Função responsável pela conexão ao servidor MQTT
void connectMQTTServer() {
  Serial.println("Conectando ao servidor MQTT...");

  if (client.connect(CLIENT_ID.c_str(), "use-token-auth", DEVICE_TOKEN)) {
     Serial.println("Conectado ao Broker MQTT...");
     client.setCallback(callback);

     client.subscribe(COMMAND_TOPIC_R1);
     client.subscribe(COMMAND_TOPIC_R2);
     digitalWrite(pinLED, HIGH);
  } else {
     Serial.print("erro = ");
     Serial.println(client.state());
     connectMQTTServer();
  }
}

void callback(char* topic, unsigned char* payload, unsigned int length) {
  Serial.print("topico ");
  Serial.println(topic);

  StaticJsonBuffer<30> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(payload);

  if ( !root.success() ) {
     Serial.println("Erro no Json Parse");
     return;
  }

  int value = root["value"];

  if (strcmp(topic, COMMAND_TOPIC_R1) == 0) {
   // if(value == 1) pinRele1.start();
   // else pinRele1.stop();
   digitalWrite(2, value);
  } else { digitalWrite(2, value);
    //if(value == 1) pinRele1.start();
 // else pinRele1.stop();
  }
}

void configModeCallback( WiFiManager *myWiFiManager) {
  Serial.println("Entrou no modo de configuração");
  Serial.println(WiFi.softAPIP());
  Serial.println(myWiFiManager->getConfigPortalSSID());
}

void saveConfigCallback() {
  Serial.println("Configuração salva");
  Serial.println(WiFi.softAPIP());
}
