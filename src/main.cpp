#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <ArduinoOTA.h>
#include "wifi_id.h"

WiFiUDP udp;
NTPClient timeClient(udp, 7*3600); // UTC+7
void connectToWiFi();

void setup() {
  Serial.begin(115200);
  connectToWiFi();
  timeClient.begin();
  ArduinoOTA.setHostname("esp32_iot8");
  ArduinoOTA.begin();
}

unsigned long previousMillis = 0;
void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= 1000) {
    previousMillis = currentMillis;
    timeClient.update();
    Serial.print("Current time: ");
    Serial.print(timeClient.getFormattedTime());
    Serial.println();
  }
  ArduinoOTA.handle();
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

