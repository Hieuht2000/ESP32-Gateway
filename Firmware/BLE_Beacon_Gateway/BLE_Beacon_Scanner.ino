#include <HTTPClient.h>
#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <BLEEddystoneURL.h>
#include <BLEEddystoneTLM.h>
#include <BLEBeacon.h>
#include "mqtt.h"
#include "led.h"
#include "settings.h"
#include "lora.h"

int scanTime = 5; //In seconds
BLEScan *pBLEScan;

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks
{
  void onResult(BLEAdvertisedDevice advertisedDevice)
  {

    if (advertisedDevice.haveManufacturerData() == true)
    {
      std::string strManufacturerData = advertisedDevice.getManufacturerData();

      uint8_t cManufacturerData[100];
      strManufacturerData.copy((char *)cManufacturerData, strManufacturerData.length(), 0);

      if (strManufacturerData.length() == 25 && cManufacturerData[0] == 0x4C && cManufacturerData[1] == 0x00)
      {
        int rssi = advertisedDevice.getRSSI();
        Serial.println("Found an iBeacon!");
        BLEBeacon oBeacon = BLEBeacon();
        oBeacon.setData(strManufacturerData);
        Serial.printf("iBeacon Frame\n");
        Serial.printf("ID: %04X Major: %d Minor: %d UUID: %s Power: %d RSSi: %d\n", oBeacon.getManufacturerId(), ENDIAN_CHANGE_U16(oBeacon.getMajor()), ENDIAN_CHANGE_U16(oBeacon.getMinor()), oBeacon.getProximityUUID().toString().c_str(), oBeacon.getSignalPower(), rssi);
        Serial.println("\n");
        
        std::string deviceAddress = advertisedDevice.getAddress().toString();
        if (deviceAddress == iBeaconMacAddress) {
          sendMqttiBeacon(deviceAddress.c_str(), oBeacon.getManufacturerId(), ENDIAN_CHANGE_U16(oBeacon.getMajor()), ENDIAN_CHANGE_U16(oBeacon.getMinor()), oBeacon.getProximityUUID().toString().c_str(), oBeacon.getSignalPower(), rssi);
          bleBlink();
          sendLoRaiBeacon(deviceAddress.c_str(), oBeacon.getManufacturerId(), ENDIAN_CHANGE_U16(oBeacon.getMajor()), ENDIAN_CHANGE_U16(oBeacon.getMinor()), oBeacon.getProximityUUID().toString().c_str(), oBeacon.getSignalPower(), rssi);
        }
      }
    }

    uint8_t *payLoad = advertisedDevice.getPayload();
    const uint8_t serviceDataEddystone[3] = {0x16, 0xAA, 0xFE}; 
    const size_t payLoadLen = advertisedDevice.getPayloadLength();
    uint8_t *payLoadEnd = payLoad + payLoadLen - 1; 
    while (payLoad < payLoadEnd) {
      if (payLoad[1] == serviceDataEddystone[0] && payLoad[2] == serviceDataEddystone[1] && payLoad[3] == serviceDataEddystone[2]) {
        // found!
        payLoad += 4;
        break;
      }
      payLoad += *payLoad + 1;
    }

    if (payLoad < payLoadEnd)
    {
      if (*payLoad == 0x10)
      {
        Serial.println("Found an EddystoneURL beacon!");
        BLEEddystoneURL foundEddyURL = BLEEddystoneURL();
        uint8_t URLLen = *(payLoad - 4) - 3;  
        foundEddyURL.setData(std::string((char*)payLoad, URLLen));
        std::string bareURL = foundEddyURL.getURL();
        if (bareURL[0] == 0x00)
        {
          Serial.println("DATA-->");
          uint8_t *payLoad = advertisedDevice.getPayload();
          for (int idx = 0; idx < payLoadLen; idx++)
          {
            Serial.printf("0x%02X ", payLoad[idx]);
          }
          Serial.println("\nInvalid Data");
          return;
        }

        Serial.printf("Found URL: %s\n", foundEddyURL.getURL().c_str());
        Serial.printf("Decoded URL: %s\n", foundEddyURL.getDecodedURL().c_str());
        Serial.printf("TX power %d\n", foundEddyURL.getPower());
        Serial.println("\n");
      } 
      else if (*payLoad == 0x20)
      {
        Serial.println("Found an EddystoneTLM beacon!");
        BLEEddystoneTLM eddystoneTLM;
        eddystoneTLM.setData(std::string((char*)payLoad, 14));
        float roundedTemp = round(eddystoneTLM.getTemp() * 100.0) / 100.0;
        Serial.printf("Reported battery voltage: %dmV\n", eddystoneTLM.getVolt());
        Serial.printf("Reported temperature: %.2f°C (raw data=0x%04X)\n", eddystoneTLM.getTemp(), eddystoneTLM.getRawTemp());
        Serial.printf("Reported advertise count: %d\n", eddystoneTLM.getCount());
        Serial.printf("Reported time since last reboot: %ds\n", eddystoneTLM.getTime());
        Serial.println("\n");
        
        String scannedUUID = advertisedDevice.getServiceUUID().toString().c_str();
        std::string deviceAddress = advertisedDevice.getAddress().toString();
        if (deviceAddress == EddyMacAddress1) {
        sendEddystoneTlmMqttMessage1(deviceAddress.c_str(), scannedUUID, eddystoneTLM.getVolt(),  roundedTemp, eddystoneTLM.getCount(), eddystoneTLM.getTime());
        sendLoRaEddystoneTlm1(deviceAddress.c_str(), scannedUUID, eddystoneTLM.getVolt(),  roundedTemp, eddystoneTLM.getCount(), eddystoneTLM.getTime());
        bleBlink();

        }
        else if (deviceAddress == EddyMacAddress2) {
        sendEddystoneTlmMqttMessage2(deviceAddress.c_str(), scannedUUID, eddystoneTLM.getVolt(),  roundedTemp, eddystoneTLM.getCount(), eddystoneTLM.getTime());
        sendLoRaEddystoneTlm2(deviceAddress.c_str(), scannedUUID, eddystoneTLM.getVolt(),  roundedTemp, eddystoneTLM.getCount(), eddystoneTLM.getTime());
        bleBlink();
        }
      }
    }
  }
};

void setup()
{
  initStatusLeds();
  updateLed(0, CRGB::Red);
  updateLed(1, CRGB::Blue);
  Serial.begin(115200);
  Serial.println("Scanning...");

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);

  mqttInit();
  loraInit();
}

void loop()
{
  BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
  Serial.println("Scan done!\n");
  pBLEScan->clearResults(); 
  delay(2000);
}
