[EddystoneTLM frame specification](https://github.com/google/eddystone/blob/master/eddystone-tlm/tlm-plain.md)

   Create a BLE server that will send periodic Eddystone TLM frames.
   The design of creating the BLE server is:
   1. Create a BLE Server
   2. Create advertising data
   3. Start advertising.
   4. wait
   5. Stop advertising.
   6. deep sleep
