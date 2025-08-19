#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

// Hardware configuration
#define RGB_LED_PIN 21
#define NUM_PIXELS 1

// Configuration structure
struct Config {
    int delayMs = 1000;           // Default 1 second
    uint8_t color1_r = 255;       // Default Red
    uint8_t color1_g = 0;
    uint8_t color1_b = 0;
    uint8_t color2_r = 0;         // Default Green
    uint8_t color2_g = 255;
    uint8_t color2_b = 0;
    uint8_t color3_r = 0;         // Default Blue
    uint8_t color3_g = 0;
    uint8_t color3_b = 255;
    bool enabled = true;          // LED cycling enabled/disabled
    int colorOrder = 0;           // 0=RGB, 1=GRB, 2=BRG, 3=BGR, 4=RBG, 5=GBR
};

// Global configuration instance
extern Config config;

// Configuration management functions
void loadConfig();
void saveConfig();
void printConfig();

// Color utility functions
uint32_t applyColorOrder(uint8_t r, uint8_t g, uint8_t b);
const char* getColorOrderName(int order);

#endif