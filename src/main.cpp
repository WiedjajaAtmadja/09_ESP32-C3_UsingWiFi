#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include "wifi_id.h"

WiFiUDP udp;
NTPClient timeClient(udp, 7*3600); // UTC+7

void setup() {
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("Connected to WiFi network");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  timeClient.begin();
}

void loop() {
  timeClient.update();
  Serial.print("Current time: ");
  Serial.print(timeClient.getFormattedTime());
  Serial.println();
  delay(1000);
}
