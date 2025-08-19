#include "wifi_manager.h"

// WiFi Access Point credentials - ESP32 creates its own network
const char* ap_ssid = "ESP32-LED-Controller";
const char* ap_password = "12345678";  // Must be at least 8 characters, or leave empty for open network

void setupWiFiAP() {
    Serial.println("Creating WiFi Access Point...");
    Serial.print("   SSID: ");
    Serial.println(ap_ssid);
    Serial.print("   Password: ");
    Serial.println(ap_password);
    
    // Configure the ESP32 as an Access Point
    WiFi.softAP(ap_ssid, ap_password);
    
    // Get the IP address of the Access Point
    IPAddress apIP = WiFi.softAPIP();
    
    Serial.println();
    Serial.println("WiFi Access Point created!");
    Serial.print("Connect to WiFi: ");
    Serial.println(ap_ssid);
    Serial.print("Password: ");
    Serial.println(ap_password);
    Serial.print("Then open: http://");
    Serial.println(apIP);
    Serial.println("   (Usually http://192.168.4.1)");
    Serial.println();
}

IPAddress getAPIP() {
    return WiFi.softAPIP();
}

int getConnectedClients() {
    return WiFi.softAPgetStationNum();
}