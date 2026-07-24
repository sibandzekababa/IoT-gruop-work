#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include "secrets.h"

const char *ssid = SEC_SSID;
const char *password = SEC_PASSWORD;
const char *broker = SEC_BROKER;
const int port = 8883;
const char *mqtt_user = SEC_MQTT_USER;
const char *mqtt_password = SEC_MQTT_PASSWORD;

WiFiClientSecure KB;
PubSubClient mqttClient(KB);

void connectMQTT() {
  KB.setInsecure();
  mqttClient.setServer(broker, port);

  while (!mqttClient.connected()) {
    String clientId = "esp32-" + String((uint32_t)ESP.getEfuseMac(), HEX);
    Serial.printf("MQTT: connecting as %s ", clientId.c_str());

    if (mqttClient.connect(clientId.c_str(), mqtt_user, mqtt_password)) {
      Serial.println("-> connected.");
    } else {
      Serial.printf("-> failed (rc=%d). Retrying in 2s\n", mqttClient.state());
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.print("\nConnected! IP Address: ");
  Serial.println(WiFi.localIP());

  connectMQTT();
}

void loop() {
  if (!mqttClient.connected()) {
    connectMQTT();
  }
  mqttClient.loop();

  static unsigned long lastMsg = 0;
  if (millis() - lastMsg > 5000) {
    lastMsg = millis();
    mqttClient.publish("esp32/status", "Welcome to Nyeredzi!");
  }
}
