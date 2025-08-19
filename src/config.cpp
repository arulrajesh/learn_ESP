#include "config.h"
#include "led_controller.h"
#include <Preferences.h>

Config config;
Preferences preferences;

// Helper function to apply color order
uint32_t applyColorOrder(uint8_t r, uint8_t g, uint8_t b) {
    extern Adafruit_NeoPixel pixels;
    
    switch(config.colorOrder) {
        case 0: return pixels.Color(r, g, b);  // RGB
        case 1: return pixels.Color(g, r, b);  // GRB
        case 2: return pixels.Color(b, r, g);  // BRG
        case 3: return pixels.Color(b, g, r);  // BGR
        case 4: return pixels.Color(r, b, g);  // RBG
        case 5: return pixels.Color(g, b, r);  // GBR
        default: return pixels.Color(r, g, b); // Default to RGB
    }
}

const char* getColorOrderName(int order) {
    const char* colorOrderNames[] = {"RGB", "GRB", "BRG", "BGR", "RBG", "GBR"};
    if (order >= 0 && order < 6) {
        return colorOrderNames[order];
    }
    return "Unknown";
}

void printConfig() {
    Serial.println("Current Configuration:");
    Serial.printf("  Delay: %d ms\n", config.delayMs);
    Serial.printf("  Color Order: %s\n", getColorOrderName(config.colorOrder));
    Serial.printf("  Color 1: RGB(%d, %d, %d)\n", config.color1_r, config.color1_g, config.color1_b);
    Serial.printf("  Color 2: RGB(%d, %d, %d)\n", config.color2_r, config.color2_g, config.color2_b);
    Serial.printf("  Color 3: RGB(%d, %d, %d)\n", config.color3_r, config.color3_g, config.color3_b);
    Serial.printf("  Enabled: %s\n", config.enabled ? "Yes" : "No");
}

void saveConfig() {
    preferences.begin("led_config", false);  // Read/write mode
    preferences.putInt("delayMs", config.delayMs);
    preferences.putInt("colorOrder", config.colorOrder);
    preferences.putUChar("color1_r", config.color1_r);
    preferences.putUChar("color1_g", config.color1_g);
    preferences.putUChar("color1_b", config.color1_b);
    preferences.putUChar("color2_r", config.color2_r);
    preferences.putUChar("color2_g", config.color2_g);
    preferences.putUChar("color2_b", config.color2_b);
    preferences.putUChar("color3_r", config.color3_r);
    preferences.putUChar("color3_g", config.color3_g);
    preferences.putUChar("color3_b", config.color3_b);
    preferences.putBool("enabled", config.enabled);
    preferences.end();
    
    Serial.println("Configuration saved to flash memory");
}

void loadConfig() {
    preferences.begin("led_config", true);   // Read-only mode
    config.delayMs = preferences.getInt("delayMs", 1000);
    config.colorOrder = preferences.getInt("colorOrder", 0);  // Default to RGB
    config.color1_r = preferences.getUChar("color1_r", 255);
    config.color1_g = preferences.getUChar("color1_g", 0);
    config.color1_b = preferences.getUChar("color1_b", 0);
    config.color2_r = preferences.getUChar("color2_r", 0);
    config.color2_g = preferences.getUChar("color2_g", 255);
    config.color2_b = preferences.getUChar("color2_b", 0);
    config.color3_r = preferences.getUChar("color3_r", 0);
    config.color3_g = preferences.getUChar("color3_g", 0);
    config.color3_b = preferences.getUChar("color3_b", 255);
    config.enabled = preferences.getBool("enabled", true);
    preferences.end();
    
    Serial.println("Configuration loaded from flash memory");
    printConfig();
}