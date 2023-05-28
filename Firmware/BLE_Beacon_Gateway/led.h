#pragma once

#include <FastLED.h>
#include "settings.h"

CRGB leds[LED_COUNT];

// Initialize 
void initStatusLeds() {
  FastLED.addLeds<WS2812B, ESP_LED_PIN, GRB>(leds, LED_COUNT);
  FastLED.setBrightness(100);
}

// Set the color of LED at position <ledIndex>
void updateLed(uint8_t ledIndex, CRGB led) {
    leds[ledIndex] = led;
    FastLED.show();
}

// Lora status
void loraBlink(){
  updateLed(0, CRGB::White);
  delay(150);
  updateLed(0, CRGB::Red);
}

// BLE status
void bleBlink(){
  updateLed(1, CRGB::White);
  delay(150);
  updateLed(1, CRGB::Blue);
}

// MQTT status
void mqttBlink(){
  updateLed(2, CRGB::White);
  delay(150);
  updateLed(2, CRGB::Green);
}

