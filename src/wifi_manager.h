#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WiFi.h>

// WiFi Access Point credentials
extern const char* ap_ssid;
extern const char* ap_password;

// WiFi management functions
void setupWiFiAP();
IPAddress getAPIP();
int getConnectedClients();

#endif