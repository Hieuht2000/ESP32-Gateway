#pragma once

#include <WiFi.h>
extern "C" {
	#include "freertos/FreeRTOS.h"
	#include "freertos/timers.h"
}
#include <AsyncMqttClient.h>
#include <ArduinoJson.h>
#include <BLEBeacon.h>
#include "settings.h"
#include "led.h"

AsyncMqttClient mqttClient;
TimerHandle_t mqttReconnectTimer;
TimerHandle_t wifiReconnectTimer;
boolean eth_connected = false;


void connectToMqtt() {
  Serial.println("Connecting to MQTT...");
  mqttClient.connect();
}

void connectEthernet() {
  bool result = ETH.begin(ETH_ADDR, ETH_POWER_PIN, ETH_MDC_PIN, ETH_MDIO_PIN, ETH_TYPE, ETH_CLK_MODE, true);
  if (!result) {
        Serial.println("ETH Init failed");
  }
}

void WiFiEvent(WiFiEvent_t event) {
    switch (event) {
        case ARDUINO_EVENT_ETH_START:
            Serial.println("ETH Started");
            //set eth hostname here
            ETH.setHostname("ESP32 Gateway");
            break;
        case ARDUINO_EVENT_ETH_CONNECTED:
            Serial.println("ETH Connected");
            break;
        case ARDUINO_EVENT_ETH_GOT_IP:
            Serial.print("ETH MAC: ");
            Serial.print(ETH.macAddress());
            Serial.print(", IPv4: ");
            Serial.print(ETH.localIP());
            if (ETH.fullDuplex()) {
                Serial.print(", FULL_DUPLEX");
            }
            Serial.print(", ");
            Serial.print(ETH.linkSpeed());
            Serial.println("Mbps");
            connectToMqtt();
            eth_connected = true;
            break;
        case ARDUINO_EVENT_ETH_DISCONNECTED:
            Serial.println("ETH Disconnected");
            //eth_connected = false;
            break;
        case ARDUINO_EVENT_ETH_STOP:
            Serial.println("ETH Stopped");
            eth_connected = false;
            break;
        default:
            break;
    }
}

void onMqttConnect(bool sessionPresent) {
  Serial.println("Connected to MQTT.");
  Serial.print("Session present: ");
  Serial.println(sessionPresent);
  updateLed(2, CRGB::Green);
}

void publishMessageiBeacon(String message) {
  mqttClient.publish(MQTT_TOPIC_SEND, 0, true, message.c_str());
}

void publishMessageEddy1(String message) {
  mqttClient.publish(MQTT_TOPIC_SEND_2, 0, true, message.c_str());
}

void publishMessageEddy2(String message) {
  mqttClient.publish(MQTT_TOPIC_SEND_3, 0, true, message.c_str());
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  Serial.println("Disconnected from MQTT.");
  updateLed(2, CRGB::Black);

  if (WiFi.isConnected()) {
    xTimerStart(mqttReconnectTimer, 0);
  }
}

void onMqttSubscribe(uint16_t packetId, uint8_t qos) {
  Serial.println("Subscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
  Serial.print("  qos: ");
  Serial.println(qos);
}

void onMqttUnsubscribe(uint16_t packetId) {
  Serial.println("Unsubscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}

void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
  Serial.println("Publish received.");
  Serial.print("  topic: ");
  Serial.println(topic);
}

void onMqttPublish(uint16_t packetId) {
  Serial.println("Publish acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}

void sendMqttiBeacon(const char *mac, uint16_t ID, uint16_t major,  uint16_t minor, const char* uuid, int8_t signalPower, int rssi) {
  if (mqttClient.connected()) {
    DynamicJsonDocument  doc(200);

    doc["MAC"] = mac;
    doc["ID"] = ID;
    doc["Major"] = major;
    doc["Minor"] = minor;
    doc["UUID"] = uuid;
    doc["SignalPower"] = signalPower;
    doc["RSSI"] = rssi;
    
    String message;
    serializeJson(doc, message);
    publishMessageiBeacon(message);
    mqttBlink();
  } else {
    Serial.println("MQTT is not connected. Discarding message");
  }
}

void sendEddystoneTlmMqttMessage1(const char *mac, String uuid, uint16_t batteryVoltage, float temperature, uint32_t advertiseCount, uint32_t timeSinceReboot) {
  if (mqttClient.connected()) {
    DynamicJsonDocument doc(200);
    
    doc["MAC"] = mac;
    doc["UUID"] = uuid;
    doc["BatteryVoltage"] = batteryVoltage;
    doc["Temperature"] = temperature;
    doc["AdvertiseCount"] = advertiseCount;
    doc["TimeSinceReboot"] = timeSinceReboot;

    String message;
    serializeJson(doc, message);
    publishMessageEddy1(message);
    mqttBlink();
  } else {
    Serial.println("MQTT is not connected. Discarding message");
  }
}

void sendEddystoneTlmMqttMessage2(const char *mac, String uuid, uint16_t batteryVoltage, float temperature, uint32_t advertiseCount, uint32_t timeSinceReboot) {
  if (mqttClient.connected()) {
    DynamicJsonDocument doc(200);
    doc["MAC"] = mac;
    doc["UUID"] = uuid;
    doc["BatteryVoltage"] = batteryVoltage;
    doc["Temperature"] = temperature;
    doc["AdvertiseCount"] = advertiseCount;
    doc["TimeSinceReboot"] = timeSinceReboot;
    
    String message;
    serializeJson(doc, message);
    publishMessageEddy2(message);
    mqttBlink();
  } else {
    Serial.println("MQTT is not connected. Discarding message");
  }
}


void mqttInit() {

  mqttReconnectTimer = xTimerCreate("mqttTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(connectToMqtt));
  wifiReconnectTimer = xTimerCreate("wifiTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(connectEthernet));

  WiFi.onEvent(WiFiEvent);

  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.onSubscribe(onMqttSubscribe);
  mqttClient.onUnsubscribe(onMqttUnsubscribe);
  mqttClient.onMessage(onMqttMessage);
  mqttClient.onPublish(onMqttPublish);
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
  

  pinMode(ETH_POWER_PIN, OUTPUT);
  digitalWrite(ETH_POWER_PIN, HIGH);

  connectEthernet();

}

