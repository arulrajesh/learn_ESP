#include <Arduino.h>
#include "config.h"
#include "wifi_manager.h"
#include "web_server.h"
#include "led_controller.h"

void setup() {
    Serial.begin(115200);
    delay(2000);
    
    Serial.println("ESP32 Web Configuration System Starting...");
    
    // Initialize components in order
    initializeLED();
    loadConfig();
    setupWiFiAP();
    setupWebServer();
    startLEDController();
    
    Serial.println("System ready!");
    Serial.println("Open the web interface to configure LED colors and timing");
}

void loop() {
    handleWebClients();
    delay(10);
}