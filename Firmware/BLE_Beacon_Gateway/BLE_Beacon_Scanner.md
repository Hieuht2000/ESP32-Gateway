## BLE Beacon Scanner

Initiates a BLE device scan.
Connect to ETHERNET / MQTT server.

Checks if the discovered devices are 
- an iBeacon
- an Eddystone TLM beacon
- an Eddystone URL beacon

and sends the decoded beacon information over MQTT and Lora server.