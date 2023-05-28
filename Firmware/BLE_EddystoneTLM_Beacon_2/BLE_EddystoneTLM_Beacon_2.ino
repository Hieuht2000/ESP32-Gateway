/*
   Create a BLE server that will send periodic Eddystone URL frames.
   The design of creating the BLE server is:
   1. Create a BLE Server
   2. Create advertising data
   3. Start advertising.
   4. wait
   5. Stop advertising.
   6. deep sleep
   
   To read data advertised by this beacon use second ESP with example sketch BLE_Beacon_Scanner
*/
#include "sys/time.h"

#include <Arduino.h>

#include "BLEDevice.h"
#include "BLEUtils.h"
#include "BLEBeacon.h"
#include "BLEAdvertising.h"
#include "BLEEddystoneTLM.h"

#include "esp_sleep.h"

#define GPIO_DEEP_SLEEP_DURATION 4     // sleep x seconds and then wake up
RTC_DATA_ATTR static time_t last;    // remember last boot in RTC Memory
RTC_DATA_ATTR static uint32_t bootcount; // remember number of boots in RTC Memory

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/
BLEAdvertising *pAdvertising;
struct timeval nowTimeStruct;

time_t lastTenth;

#define BEACON_UUID "8ec76ea3-6668-48da-9866-75be8bc86f4d" // UUID 1 128-Bit (may use linux tool uuidgen or random numbers via https://www.uuidgenerator.net/)

void setBeacon()
{
  char beacon_data[25];
  uint16_t beconUUID = 0xFEAA;
  uint16_t volt = 3300; // 3300mV = 3.3V
  float tempFloat = random(1000, 2000) / 100.0f;
  Serial.printf("Random temperature is %.2f°C\n", tempFloat);
  int temp = (int)(tempFloat * 256);
  Serial.printf("Converted to 8.8 format %0X%0X\n", (temp >> 8) & 0xFF, (temp & 0xFF)); 

  BLEAdvertisementData oAdvertisementData = BLEAdvertisementData();
  BLEAdvertisementData oScanResponseData = BLEAdvertisementData();

  oScanResponseData.setFlags(0x06); // GENERAL_DISC_MODE 0x02 | BR_EDR_NOT_SUPPORTED 0x04
  oScanResponseData.setCompleteServices(BLEUUID(beconUUID));

  beacon_data[0] = 0x20;                // Eddystone Frame Type (Unencrypted Eddystone-TLM)
  beacon_data[1] = 0x00;                // TLM version
  beacon_data[2] = (volt >> 8);           // Battery voltage, 1 mV/bit i.e. 0xCE4 = 3300mV = 3.3V
  beacon_data[3] = (volt & 0xFF);           //
  beacon_data[4] = (temp >> 8);           // Beacon temperature
  beacon_data[5] = (temp & 0xFF);           //
  beacon_data[6] = ((bootcount & 0xFF000000) >> 24);  // Advertising PDU count
  beacon_data[7] = ((bootcount & 0xFF0000) >> 16);  //
  beacon_data[8] = ((bootcount & 0xFF00) >> 8);   //
  beacon_data[9] = (bootcount & 0xFF);        //
  beacon_data[10] = ((lastTenth & 0xFF000000) >> 24); // Time since power-on or reboot as 0.1 second resolution counter
  beacon_data[11] = ((lastTenth & 0xFF0000) >> 16);   //
  beacon_data[12] = ((lastTenth & 0xFF00) >> 8);    //
  beacon_data[13] = (lastTenth & 0xFF);       //

  oScanResponseData.setServiceData(BLEUUID(beconUUID), std::string(beacon_data, 14));
  oAdvertisementData.setName("TLMBeacon_2");
  pAdvertising->setAdvertisementData(oAdvertisementData);
  pAdvertising->setScanResponseData(oScanResponseData);
}

void setup()
{
  Serial.begin(115200);
  gettimeofday(&nowTimeStruct, NULL);

  Serial.printf("Starting ESP32. Bootcount = %d\n", bootcount++);
  Serial.printf("Deep sleep (%lds since last reset, %lds since last boot)\n", nowTimeStruct.tv_sec, nowTimeStruct.tv_sec - last);

  last = nowTimeStruct.tv_sec;
  lastTenth = nowTimeStruct.tv_sec * 10; // Time since last reset as 0.1 second resolution counter

  // Create the BLE Device
  BLEDevice::init("TLMBeacon_2");

  BLEDevice::setPower(ESP_PWR_LVL_N12);

  pAdvertising = BLEDevice::getAdvertising();

  setBeacon();
  // Start advertising
  pAdvertising->start();
  Serial.println("Advertising started for 10s ...");
  delay(10000);
  pAdvertising->stop();
  Serial.printf("Enter deep sleep for 4s\n");
  esp_deep_sleep(1000000LL * GPIO_DEEP_SLEEP_DURATION);
}

void loop()
{
}
