#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <ArduinoOTA.h>
#include <Ticker.h>
#include <PubSubClient.h>
#include "wifi_id.h"

#define MQTT_BROKER  "broker.emqx.io"
#define MQTT_TOPIC_SUBSCRIBE "binus/iot2025/esp32/cmd"
#define MQTT_TOPIC_PUBLISH   "binus/iot2025/esp32/data"
char g_szDeviceId[30];

Ticker ticker1Second;
WiFiUDP udp;
WiFiClient espClient;
PubSubClient mqtt(espClient);
NTPClient timeClient(udp, 7*3600); // UTC+7
// Teleplot target (change IP if needed)
const char* teleplotIP = "10.143.73.42";  
const int teleplotPort = 47269; // default Teleplot UDP port

void sendToTeleplot(const char* label, float value) {
  udp.beginPacket(teleplotIP, teleplotPort);
  udp.printf("%s:%f\n", label, value);
  udp.endPacket();
}

void connectToWiFi();
boolean mqttConnect();
void OneSecondTicker() {
    timeClient.update();
    // Serial.print("Current time: ");
    // Serial.print(timeClient.getFormattedTime());
    // Serial.println();
    // sendToTeleplot("CurrentTime", timeClient.getEpochTime());
}

void setup() {
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  Serial.begin(115200);
  connectToWiFi();
  timeClient.begin();
  ArduinoOTA.setHostname("esp32_iot8");
  ArduinoOTA.begin();
  ticker1Second.attach(1.0, OneSecondTicker);
  mqttConnect();
}

void loop() {
  ArduinoOTA.handle();
  mqtt.loop();
}

void mqttCallback(char* topic, byte* payload, unsigned int len) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.write(payload, len);
  // digitalWrite(12, payload[0]-'0');
  // digitalWrite(13, payload[1]-'0');

  Serial.println();
}

boolean mqttConnect() {

  sprintf(g_szDeviceId, "esp32_%08X",(uint32_t)ESP.getEfuseMac());
  mqtt.setServer(MQTT_BROKER, 1883);
  mqtt.setCallback(mqttCallback);
  Serial.printf("Connecting to %s clientId: %s\n", MQTT_BROKER, g_szDeviceId);

  boolean fMqttConnected = false;
  for (int i=0; i<3 && !fMqttConnected; i++) {
    Serial.print("Connecting to mqtt broker...");
    fMqttConnected = mqtt.connect(g_szDeviceId);
    if (fMqttConnected == false) {
      Serial.print(" fail, rc=");
      Serial.println(mqtt.state());
      delay(1000);
    }
  }
  if (fMqttConnected)
  {
    Serial.println(" success");
    bool fResult = mqtt.subscribe(MQTT_TOPIC_SUBSCRIBE);
    Serial.printf("Subcribe topic: %s->%d \r\n", MQTT_TOPIC_SUBSCRIBE, fResult);
    // onPublishMessage();
  }
  return mqtt.connected();
}

void connectToWiFi()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.printf("Connecting to WiFi %s ...\r\n", WIFI_SSID);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println("Connected to WiFi network");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

