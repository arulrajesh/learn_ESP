#include "web_server.h"
#include "config.h"
#include "led_controller.h"
#include <ArduinoJson.h>
#include <SPIFFS.h>

// Global web server object
WebServer server(80);

void handleRoot() {
    // Serve the main HTML file
    File file = SPIFFS.open("/index.html", "r");
    if (!file) {
        server.send(404, "text/plain", "File not found");
        return;
    }
    
    server.streamFile(file, "text/html");
    file.close();
}

void handleCSS() {
    // Serve the CSS file
    File file = SPIFFS.open("/style.css", "r");
    if (!file) {
        server.send(404, "text/plain", "CSS file not found");
        return;
    }
    
    server.streamFile(file, "text/css");
    file.close();
}

void handleJS() {
    // Serve the JavaScript file
    File file = SPIFFS.open("/script.js", "r");
    if (!file) {
        server.send(404, "text/plain", "JavaScript file not found");
        return;
    }
    
    server.streamFile(file, "application/javascript");
    file.close();
}

void handleUpdate() {
    if (server.hasArg("plain")) {
        String body = server.arg("plain");
        JsonDocument doc;
        
        DeserializationError error = deserializeJson(doc, body);
        if (error) {
            server.send(400, "text/plain", "Invalid JSON");
            return;
        }

        // Update configuration
        config.delayMs = doc["delayMs"];
        config.colorOrder = doc["colorOrder"];
        config.color1_r = doc["color1_r"];
        config.color1_g = doc["color1_g"];
        config.color1_b = doc["color1_b"];
        config.color2_r = doc["color2_r"];
        config.color2_g = doc["color2_g"];
        config.color2_b = doc["color2_b"];
        config.color3_r = doc["color3_r"];
        config.color3_g = doc["color3_g"];
        config.color3_b = doc["color3_b"];

        saveConfig();
        
        // Notify LED task of new configuration
        if (ledTaskHandle != NULL) {
            xTaskNotify(ledTaskHandle, 1, eSetBits);
        }

        server.send(200, "text/plain", "Configuration updated and saved!");
        Serial.println("Web: Configuration updated");
        printConfig();
    } else {
        server.send(400, "text/plain", "No data received");
    }
}

void handleToggle() {
    config.enabled = !config.enabled;
    saveConfig();
    
    if (ledTaskHandle != NULL) {
        xTaskNotify(ledTaskHandle, 1, eSetBits);
    }
    
    String response = config.enabled ? "LED cycling enabled" : "LED cycling disabled";
    server.send(200, "text/plain", response);
    Serial.printf("Web: LED %s\n", config.enabled ? "enabled" : "disabled");
}

void handleTestColor() {
    if (server.hasArg("plain")) {
        String body = server.arg("plain");
        JsonDocument doc;
        
        DeserializationError error = deserializeJson(doc, body);
        if (error) {
            server.send(400, "text/plain", "Invalid JSON");
            return;
        }

        uint8_t r = doc["color_r"];
        uint8_t g = doc["color_g"];
        uint8_t b = doc["color_b"];

        // Apply current color order and show test color
        testColor(r, g, b);

        server.send(200, "text/plain", "Test color displayed");
        Serial.printf("Web: Test color RGB(%d,%d,%d)\n", r, g, b);
    } else {
        server.send(400, "text/plain", "No data received");
    }
}

void handleGetConfig() {
    JsonDocument doc;
    doc["delayMs"] = config.delayMs;
    doc["colorOrder"] = config.colorOrder;
    doc["color1_r"] = config.color1_r;
    doc["color1_g"] = config.color1_g;
    doc["color1_b"] = config.color1_b;
    doc["color2_r"] = config.color2_r;
    doc["color2_g"] = config.color2_g;
    doc["color2_b"] = config.color2_b;
    doc["color3_r"] = config.color3_r;
    doc["color3_g"] = config.color3_g;
    doc["color3_b"] = config.color3_b;
    doc["enabled"] = config.enabled;

    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
}

void setupWebServer() {
    // Initialize SPIFFS for serving files
    if (!SPIFFS.begin(true)) {
        Serial.println("An error occurred while mounting SPIFFS");
        return;
    }
    
    // Setup web server routes
    server.on("/", handleRoot);
    server.on("/style.css", handleCSS);
    server.on("/script.js", handleJS);
    server.on("/update", HTTP_POST, handleUpdate);
    server.on("/toggle", HTTP_POST, handleToggle);
    server.on("/test-color", HTTP_POST, handleTestColor);
    server.on("/config", HTTP_GET, handleGetConfig);
    
    server.begin();
    Serial.println("Web server started");
    Serial.println("SPIFFS initialized for serving static files");
}

void handleWebClients() {
    server.handleClient();
}