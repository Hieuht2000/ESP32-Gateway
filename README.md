# ESP32 Gateway (BLE - LORA)

A Bluetooth-Low-Energy Gateway with Ethernet, LoRa as backhauls using ESP32. The purpose is to make a compact Gateway device (home automation...) to collecting data from BLE nodes and upload to MQTT server (Ethernet) or The Thing Networks LoraWAN server(LORA module). The modules name in picture below and also in the BOM list.

## Hardware Overview

![image](https://github.com/Hieuht2000/ESP32-Gateway/assets/63698805/ae008c68-efb1-439b-9e60-471252998830)

##  Demo firmware 

The firmware setup is in [Demo/Demo Setup.png ](https://github.com/Hieuht2000/ESP32-Gateway/blob/f936b5e5aa8e6cdfc5cd262c80da75539e702d1e/Demo/Demo%20Setup.png). The device will collecting datas from ibeacons and Eddystone beacons in range but only upload the datas to MQTT server and LORA server of the beacons with matched MAC address(define in "setting.h"). The 3 status LEDs is for each connections, it blinks when data coming in or out: Green for MQTT, Blue for BLE and Red for LORA.

There a dashboard using Node-RED to vizualized the data. Firmware for beacons in demo setup can be found on firmware folder as well.


## Device photo
![ESP32 Gateway](https://github.com/Hieuht2000/ESP32-Gateway/assets/63698805/8e22e89b-d945-4b41-82a9-b2daa3057cb4) 

##  With Enclosure

![Enclosure1](https://github.com/Hieuht2000/ESP32-Gateway/assets/63698805/1cec4768-6532-4acb-8d70-30717d34aad1)

