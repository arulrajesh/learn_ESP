#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>
#include <ArduinoJson.h>
#include <Adafruit_NeoPixel.h>

#define RGB_LED_PIN 21
#define NUM_PIXELS 1

// WiFi credentials - UPDATE THESE!
const char* ssid = "Theppa"; // Replace with your WiFi SSID
const char* password = "cornflake@#spasta";

// Objects
Adafruit_NeoPixel pixels(NUM_PIXELS, RGB_LED_PIN, NEO_GRB + NEO_KHZ800);
WebServer server(80);
Preferences preferences;

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
};

Config config;
TaskHandle_t ledTaskHandle = NULL;

// ===== CONFIGURATION MANAGEMENT =====

void printConfig() {
    Serial.println("Current Configuration:");
    Serial.printf("  Delay: %d ms\n", config.delayMs);
    Serial.printf("  Color 1: RGB(%d, %d, %d)\n", config.color1_r, config.color1_g, config.color1_b);
    Serial.printf("  Color 2: RGB(%d, %d, %d)\n", config.color2_r, config.color2_g, config.color2_b);
    Serial.printf("  Color 3: RGB(%d, %d, %d)\n", config.color3_r, config.color3_g, config.color3_b);
    Serial.printf("  Enabled: %s\n", config.enabled ? "Yes" : "No");
}

void saveConfig() {
    preferences.begin("led_config", false);  // Read/write mode
    preferences.putInt("delayMs", config.delayMs);
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
    
    Serial.println("💾 Configuration saved to flash memory");
}

void loadConfig() {
    preferences.begin("led_config", true);   // Read-only mode
    config.delayMs = preferences.getInt("delayMs", 1000);
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
    
    Serial.println("📂 Configuration loaded from flash memory");
    printConfig();
}

// ===== WEB SERVER HANDLERS =====

void handleRoot() {
    String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <title>ESP32 LED Controller</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        body { font-family: Arial; margin: 40px; background: #f0f0f0; }
        .container { max-width: 600px; margin: 0 auto; background: white; padding: 30px; border-radius: 10px; box-shadow: 0 4px 6px rgba(0,0,0,0.1); }
        h1 { color: #333; text-align: center; }
        .form-group { margin: 20px 0; }
        label { display: block; margin-bottom: 5px; font-weight: bold; }
        input[type="number"], input[type="color"] { width: 100%; padding: 10px; border: 1px solid #ddd; border-radius: 5px; }
        .color-group { display: flex; gap: 10px; align-items: center; }
        .color-input { flex: 1; }
        button { background: #4CAF50; color: white; padding: 12px 24px; border: none; border-radius: 5px; cursor: pointer; font-size: 16px; margin: 10px 5px; }
        button:hover { background: #45a049; }
        .toggle-btn { background: #2196F3; }
        .toggle-btn:hover { background: #1976D2; }
        .status { padding: 10px; margin: 10px 0; border-radius: 5px; text-align: center; }
        .success { background: #d4edda; color: #155724; border: 1px solid #c3e6cb; }
        .preview { width: 50px; height: 50px; border: 2px solid #333; border-radius: 50%; display: inline-block; margin-left: 10px; }
        .current-config { background: #e9ecef; padding: 15px; border-radius: 5px; margin: 20px 0; }
    </style>
</head>
<body>
    <div class="container">
        <h1>🌈 ESP32 LED Controller</h1>
        
        <div class="current-config">
            <h3>Current Configuration:</h3>
            <p><strong>Delay:</strong> <span id="currentDelay">Loading...</span> ms</p>
            <p><strong>Status:</strong> <span id="currentStatus">Loading...</span></p>
            <p><strong>Colors:</strong> 
                <span class="preview" id="preview1"></span>
                <span class="preview" id="preview2"></span>
                <span class="preview" id="preview3"></span>
            </p>
        </div>

        <div id="status"></div>

        <form onsubmit="updateConfig(event)">
            <div class="form-group">
                <label for="delay">Color Change Delay (milliseconds):</label>
                <input type="number" id="delay" min="100" max="10000" value="1000">
            </div>

            <div class="form-group">
                <label>Color 1:</label>
                <div class="color-group">
                    <input type="color" id="color1" value="#ff0000" class="color-input">
                    <span class="preview" id="color1-preview"></span>
                </div>
            </div>

            <div class="form-group">
                <label>Color 2:</label>
                <div class="color-group">
                    <input type="color" id="color2" value="#00ff00" class="color-input">
                    <span class="preview" id="color2-preview"></span>
                </div>
            </div>

            <div class="form-group">
                <label>Color 3:</label>
                <div class="color-group">
                    <input type="color" id="color3" value="#0000ff" class="color-input">
                    <span class="preview" id="color3-preview"></span>
                </div>
            </div>

            <button type="submit">💾 Save Configuration</button>
            <button type="button" onclick="toggleLED()" class="toggle-btn">🔄 Toggle LED</button>
            <button type="button" onclick="loadCurrentConfig()">🔄 Refresh</button>
        </form>
    </div>

    <script>
        // Update color previews when color inputs change
        document.getElementById('color1').addEventListener('input', function() {
            document.getElementById('color1-preview').style.backgroundColor = this.value;
        });
        document.getElementById('color2').addEventListener('input', function() {
            document.getElementById('color2-preview').style.backgroundColor = this.value;
        });
        document.getElementById('color3').addEventListener('input', function() {
            document.getElementById('color3-preview').style.backgroundColor = this.value;
        });

        function hexToRgb(hex) {
            const result = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(hex);
            return result ? {
                r: parseInt(result[1], 16),
                g: parseInt(result[2], 16),
                b: parseInt(result[3], 16)
            } : null;
        }

        function rgbToHex(r, g, b) {
            return "#" + ((1 << 24) + (r << 16) + (g << 8) + b).toString(16).slice(1);
        }

        async function updateConfig(event) {
            event.preventDefault();
            
            const delay = document.getElementById('delay').value;
            const color1 = hexToRgb(document.getElementById('color1').value);
            const color2 = hexToRgb(document.getElementById('color2').value);
            const color3 = hexToRgb(document.getElementById('color3').value);

            const config = {
                delayMs: parseInt(delay),
                color1_r: color1.r, color1_g: color1.g, color1_b: color1.b,
                color2_r: color2.r, color2_g: color2.g, color2_b: color2.b,
                color3_r: color3.r, color3_g: color3.g, color3_b: color3.b
            };

            try {
                const response = await fetch('/update', {
                    method: 'POST',
                    headers: { 'Content-Type': 'application/json' },
                    body: JSON.stringify(config)
                });

                const result = await response.text();
                document.getElementById('status').innerHTML = '<div class="status success">' + result + '</div>';
                loadCurrentConfig();
            } catch (error) {
                document.getElementById('status').innerHTML = '<div class="status error">Error: ' + error + '</div>';
            }
        }

        async function toggleLED() {
            try {
                const response = await fetch('/toggle', { method: 'POST' });
                const result = await response.text();
                document.getElementById('status').innerHTML = '<div class="status success">' + result + '</div>';
                loadCurrentConfig();
            } catch (error) {
                document.getElementById('status').innerHTML = '<div class="status error">Error: ' + error + '</div>';
            }
        }

        async function loadCurrentConfig() {
            try {
                const response = await fetch('/config');
                const config = await response.json();
                
                document.getElementById('delay').value = config.delayMs;
                document.getElementById('currentDelay').textContent = config.delayMs;
                document.getElementById('currentStatus').textContent = config.enabled ? 'Running' : 'Stopped';
                
                const color1Hex = rgbToHex(config.color1_r, config.color1_g, config.color1_b);
                const color2Hex = rgbToHex(config.color2_r, config.color2_g, config.color2_b);
                const color3Hex = rgbToHex(config.color3_r, config.color3_g, config.color3_b);
                
                document.getElementById('color1').value = color1Hex;
                document.getElementById('color2').value = color2Hex;
                document.getElementById('color3').value = color3Hex;
                
                document.getElementById('color1-preview').style.backgroundColor = color1Hex;
                document.getElementById('color2-preview').style.backgroundColor = color2Hex;
                document.getElementById('color3-preview').style.backgroundColor = color3Hex;
                
                document.getElementById('preview1').style.backgroundColor = color1Hex;
                document.getElementById('preview2').style.backgroundColor = color2Hex;
                document.getElementById('preview3').style.backgroundColor = color3Hex;
                
            } catch (error) {
                console.error('Error loading config:', error);
            }
        }

        // Load current config when page loads
        loadCurrentConfig();
    </script>
</body>
</html>
)rawliteral";

    server.send(200, "text/html", html);
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

        server.send(200, "text/plain", "✅ Configuration updated and saved!");
        Serial.println("🌐 Web: Configuration updated");
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
    
    String response = config.enabled ? "✅ LED cycling enabled" : "⏸️ LED cycling disabled";
    server.send(200, "text/plain", response);
    Serial.printf("🌐 Web: LED %s\n", config.enabled ? "enabled" : "disabled");
}

void handleGetConfig() {
    JsonDocument doc;
    doc["delayMs"] = config.delayMs;
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

// ===== LED CONTROL TASK =====

void ledControlTask(void *pvParameters) {
    Serial.print("💡 LED Control Task running on core ");
    Serial.println(xPortGetCoreID());
    
    int colorIndex = 0;
    uint32_t lastUpdate = 0;
    
    for(;;) {
        // Check for configuration updates
        uint32_t notificationValue;
        if (xTaskNotifyWait(0, 0xFFFFFFFF, &notificationValue, 0)) {
            Serial.println("💡 LED: Configuration updated");
        }
        
        if (config.enabled && (millis() - lastUpdate >= config.delayMs)) {
            uint32_t colors[3] = {
                pixels.Color(config.color1_r, config.color1_g, config.color1_b),
                pixels.Color(config.color2_r, config.color2_g, config.color2_b),
                pixels.Color(config.color3_r, config.color3_g, config.color3_b)
            };
            
            pixels.setPixelColor(0, colors[colorIndex]);
            pixels.show();
            
            colorIndex = (colorIndex + 1) % 3;
            lastUpdate = millis();
        } else if (!config.enabled) {
            // Turn off LED when disabled
            pixels.setPixelColor(0, pixels.Color(0, 0, 0));
            pixels.show();
        }
        
        vTaskDelay(50 / portTICK_PERIOD_MS);  // Check every 50ms
    }
}

// ===== MAIN SETUP =====

void setup() {
    Serial.begin(115200);
    delay(2000);
    
    Serial.println("🚀 ESP32 Web Configuration System Starting...");
    
    // Initialize LED
    pixels.begin();
    pixels.clear();
    pixels.show();
    
    // Load saved configuration
    loadConfig();
    
    // Connect to WiFi
    Serial.print("📶 Connecting to WiFi: ");
    Serial.println(ssid);
    
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    
    Serial.println();
    Serial.println("✅ WiFi connected!");
    Serial.print("🌐 Web interface: http://");
    Serial.println(WiFi.localIP());
    
    // Setup web server routes
    server.on("/", handleRoot);
    server.on("/update", HTTP_POST, handleUpdate);
    server.on("/toggle", HTTP_POST, handleToggle);
    server.on("/config", HTTP_GET, handleGetConfig);
    
    server.begin();
    Serial.println("🌐 Web server started");
    
    // Create LED control task
    xTaskCreatePinnedToCore(
        ledControlTask,
        "LEDControl",
        10000,
        NULL,
        1,
        &ledTaskHandle,
        0
    );
    
    Serial.println("✅ System ready!");
    Serial.println("Open the web interface to configure LED colors and timing");
}

void loop() {
    server.handleClient();
    delay(10);
}