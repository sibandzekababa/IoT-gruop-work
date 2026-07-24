#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

WiFiClientSecure KB;
PubSubClient mqttClient(KB);

const char* SSID = "Clasrooms";
const char* Pass = "Chix@Akira";
const char* BrokerURL = "61917c0a47134645af2b8a974c7bcc8e.s1.eu.hivemq.cloud";
const int port = 8883;
const char* user_mqtt = "Kababa";
const char* mqtt_pass = "Kababa wa Bongwe";

void connectMQTT() {
  KB.setInsecure();
  mqttClient.setServer(BrokerURL, port);

  while (!mqttClient.connected()) {
    String clientId = "esp32-" + String((uint32_t)ESP.getEfuseMac(), HEX);
    Serial.printf("MQTT: connecting as %s ", clientId.c_str());

    if (mqttClient.connect(clientId.c_str(), user_mqtt, mqtt_pass)) {
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
  WiFi.begin(SSID, Pass);

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
