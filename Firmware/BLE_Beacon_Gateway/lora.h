#include <Arduino.h>
#include <settings.h>
#include <ArduinoJson.h>

unsigned long lastLoRaSent = 0;
static char recv_buf[512];
int counter = 0;

static int at_send_check_response(char *p_ack, int timeout_ms, char *p_cmd)
{
  int ch;
  int num = 0;
  int index = 0;
  int startMillis = 0;
  memset(recv_buf, 0, sizeof(recv_buf));
  Serial1.print(p_cmd);
  Serial.print(p_cmd);
  delay(200);
  startMillis = millis();

  if (p_ack == NULL)
      return 0;

  do
  {
      while (Serial1.available() > 0)
      {
          ch = Serial1.read();
          recv_buf[index++] = ch;
          Serial.print((char)ch);
          delay(2);
      }

      if (strstr(recv_buf, p_ack) != NULL)
          return 1;

  } while (millis() - startMillis < timeout_ms);
  Serial.println();
  return 0;
}

void loraInit()
{
  Serial1.begin(9600, SERIAL_8N1, LORA_RXD, LORA_TXD, false);
  Serial.print("LORA start\r\n");
  at_send_check_response("+AT: OK", 1000, "AT\r\n");
  at_send_check_response("+MODE: TEST", 1000, "AT+MODE=TEST\r\n");
  at_send_check_response("+TEST: RFCFG", 1000, "AT+TEST=RFCFG,868,SF7,125,8,8, 14,ON,OFF,OFF\r\n");
  delay(200);
}

String toHexString(const String &input)
{
  String hexString = "";
  for (size_t i = 0; i < input.length(); ++i)
  {
    hexString += String(input.charAt(i), HEX);
  }
  return hexString;
}

void sendLoRaiBeacon(const char *mac, uint16_t ID, uint16_t major, uint16_t minor, const char *uuid, int8_t signalPower, int rssi)
{
  // Check if it's time to send a LoRa message (every 2 minutes)
  if (millis() - lastLoRaSent >= 0) {
    char cmd[512];

    DynamicJsonDocument doc(256);
    doc["MAC"] = mac;
    doc["ID"] = ID;
    doc["Major"] = major;
    doc["Minor"] = minor;
    doc["UUID"] = uuid;
    doc["SignalPower"] = signalPower;
    doc["RSSI"] = rssi;

    String jsonString;
    serializeJson(doc, jsonString);

    String hexString = toHexString(jsonString);

    sprintf(cmd, "AT+TEST=TXLRPKT,\"%s\"\r\n", hexString.c_str());
    int ret = at_send_check_response("+TEST: TXLRPKT", 5000, cmd);

    if (ret)
      Serial.println("Sent lora iBeacon.\n");
    else
      Serial.println("Send failed!\r\n\r\n");

    loraBlink();

    // Update the last time a LoRa message was sent
    lastLoRaSent = millis();
  }
}

void sendLoRaEddystoneTlm1(const char *mac, String uuid, uint16_t batteryVoltage, float temperature, uint32_t advertiseCount, uint32_t timeSinceReboot)
{
  // Check if it's time to send a LoRa message (every 2 minutes)
  if (millis() - lastLoRaSent >= 0) {
    char cmd[512];

    DynamicJsonDocument doc(256);
    doc["MAC"] = mac;
    doc["UUID"] = uuid;
    doc["BatteryVoltage"] = batteryVoltage;
    doc["Temperature"] = temperature;
    doc["AdvertiseCount"] = advertiseCount;
    doc["TimeSinceReboot"] = timeSinceReboot;

    String jsonString;
    serializeJson(doc, jsonString);

    String hexString = toHexString(jsonString);

    sprintf(cmd, "AT+TEST=TXLRPKT,\"%s\"\r\n", hexString.c_str());
    int ret = at_send_check_response("+TEST: TXLRPKT", 5000, cmd);

    if (ret)
      Serial.println("Sent lora Eddystone 1.\n");
    else
      Serial.println("Send failed!\r\n\r\n");

    loraBlink();

    // Update the last time a LoRa message was sent
    lastLoRaSent = millis();
  }
}

void sendLoRaEddystoneTlm2(const char *mac, String uuid, uint16_t batteryVoltage, float temperature, uint32_t advertiseCount, uint32_t timeSinceReboot)
{
  // Check if it's time to send a LoRa message (every 2 minutes)
  if (millis() - lastLoRaSent >= 0) {
    char cmd[512];

    DynamicJsonDocument doc(256);
    doc["MAC"] = mac;
    doc["UUID"] = uuid;
    doc["BatteryVoltage"] = batteryVoltage;
    doc["Temperature"] = temperature;
    doc["AdvertiseCount"] = advertiseCount;
    doc["TimeSinceReboot"] = timeSinceReboot;

    String jsonString;
    serializeJson(doc, jsonString);

    String hexString = toHexString(jsonString);

    sprintf(cmd, "AT+TEST=TXLRPKT,\"%s\"\r\n", hexString.c_str());
    int ret = at_send_check_response("+TEST: TXLRPKT", 5000, cmd);

    if (ret)
      Serial.println("Sent lora Eddystone 2.\n");
    else
      Serial.println("Send failed!\r\n\r\n");

    loraBlink();

    // Update the last time a LoRa message was sent
    lastLoRaSent = millis();
  }
}