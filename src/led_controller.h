#ifndef LED_CONTROLLER_H
#define LED_CONTROLLER_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "config.h"

// Global LED object
extern Adafruit_NeoPixel pixels;
extern TaskHandle_t ledTaskHandle;

// LED control functions
void initializeLED();
void startLEDController();
void ledControlTask(void *pvParameters);
void testColor(uint8_t r, uint8_t g, uint8_t b);
void turnOffLED();

#endif