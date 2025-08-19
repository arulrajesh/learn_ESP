#include "led_controller.h"

// Global LED object
Adafruit_NeoPixel pixels(NUM_PIXELS, RGB_LED_PIN, NEO_RGB + NEO_KHZ800);
TaskHandle_t ledTaskHandle = NULL;

void initializeLED() {
    pixels.begin();
    pixels.clear();
    pixels.show();
    Serial.println("LED initialized");
}

void testColor(uint8_t r, uint8_t g, uint8_t b) {
    uint32_t color = applyColorOrder(r, g, b);
    pixels.setPixelColor(0, color);
    pixels.show();
    Serial.printf("Test color displayed: RGB(%d,%d,%d)\n", r, g, b);
}

void turnOffLED() {
    pixels.setPixelColor(0, pixels.Color(0, 0, 0));
    pixels.show();
}

void ledControlTask(void *pvParameters) {
    Serial.print("LED Control Task running on core ");
    Serial.println(xPortGetCoreID());
    
    int colorIndex = 0;
    uint32_t lastUpdate = 0;
    
    for(;;) {
        // Check for configuration updates
        uint32_t notificationValue;
        if (xTaskNotifyWait(0, 0xFFFFFFFF, &notificationValue, 0)) {
            Serial.println("LED: Configuration updated");
        }
        
        if (config.enabled && (millis() - lastUpdate >= config.delayMs)) {
            uint32_t colors[3] = {
                applyColorOrder(config.color1_r, config.color1_g, config.color1_b),
                applyColorOrder(config.color2_r, config.color2_g, config.color2_b),
                applyColorOrder(config.color3_r, config.color3_g, config.color3_b)
            };
            
            pixels.setPixelColor(0, colors[colorIndex]);
            pixels.show();
            
            colorIndex = (colorIndex + 1) % 3;
            lastUpdate = millis();
        } else if (!config.enabled) {
            // Turn off LED when disabled
            turnOffLED();
        }
        
        vTaskDelay(50 / portTICK_PERIOD_MS);  // Check every 50ms
    }
}

void startLEDController() {
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
    Serial.println("LED Controller task started");
}