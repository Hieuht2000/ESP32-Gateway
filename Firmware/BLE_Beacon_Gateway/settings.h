#pragma once

#include <ETH.h>

// Ethernet settings
#define ETH_CLK_MODE    ETH_CLOCK_GPIO16_OUT
#define ETH_POWER_PIN   5
#define ETH_TYPE        ETH_PHY_LAN8720
#define ETH_ADDR        0
#define ETH_MDC_PIN     23
#define ETH_MDIO_PIN    18

// MQTT Settings
#define MQTT_HOST IPAddress(192, 168, 0, 103)
#define MQTT_PORT 1883
#define MQTT_TOPIC_SEND "BLEtoMQTT/iBeacon"
#define MQTT_TOPIC_SEND_2 "BLEtoMQTT/Eddystone1"
#define MQTT_TOPIC_SEND_3 "BLEtoMQTT/Eddystone2"

// BLE beacons MAC addresses
const std::string iBeaconMacAddress = "b8:d6:1a:5c:1e:c6";
const std::string EddyMacAddress1 = "30:ae:a4:19:78:56";
const std::string EddyMacAddress2 = "30:ae:a4:1e:98:62";

// LoRa Settings
#define LORA_RXD 12
#define LORA_TXD 13

// LED Settings
#define ESP_LED_PIN 32
#define LED_COUNT   3
#define CHANNEL     0