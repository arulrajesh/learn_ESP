#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#define RGB_LED_PIN 21
#define NUM_PIXELS 1

Adafruit_NeoPixel pixels(NUM_PIXELS, RGB_LED_PIN, NEO_GRB + NEO_KHZ800);

// Task handle - like a "phone number" to reach the LED task
TaskHandle_t LEDTaskHandle = NULL;

// === TASK 1: LED WORKER (Waits for orders) ===
void LEDWorkerTask(void *pvParameters) {
    Serial.println("🔴 LED Worker: I'm ready and waiting for orders...");
    
    uint32_t colors[] = {
        pixels.Color(255, 0, 0),    // Red
        pixels.Color(0, 255, 0),    // Green
        pixels.Color(0, 0, 255),    // Blue
    };
    int colorIndex = 0;
    
    for(;;) {
        Serial.println("🔴 LED Worker: Sleeping... waiting for notification...");
        
        // WAIT for someone to "tap me on the shoulder"
        uint32_t notificationValue;
        if(xTaskNotifyWait(0, 0, &notificationValue, portMAX_DELAY)) {
            
            Serial.print("🔴 LED Worker: Got notification! Value = ");
            Serial.println(notificationValue);
            
            // Change LED color
            pixels.setPixelColor(0, colors[colorIndex]);
            pixels.show();
            
            Serial.print("🔴 LED Worker: Changed LED to color ");
            Serial.println(colorIndex);
            
            colorIndex = (colorIndex + 1) % 3; // Cycle: 0, 1, 2, 0, 1, 2...
        }
    }
}

// === TASK 2: BUTTON SIMULATOR (Sends orders) ===
void ButtonTask(void *pvParameters) {
    Serial.println("🔵 Button Task: I will send orders every 2 seconds");
    
    int buttonPressCount = 0;
    
    for(;;) {
        delay(2000); // Wait 2 seconds
        
        buttonPressCount++;
        Serial.print("🔵 Button Task: Button pressed #");
        Serial.print(buttonPressCount);
        Serial.println(" - Sending notification to LED Worker!");
        
        // "TAP THE LED WORKER ON THE SHOULDER"
        xTaskNotify(
            LEDTaskHandle,      // WHO to notify (the LED worker)
            buttonPressCount,   // WHAT to send (just the press count)
            eSetValueWithOverwrite  // HOW to send it
        );
        
        Serial.println("🔵 Button Task: Notification sent! Going back to sleep...");
    }
}

void setup() {
    Serial.begin(115200);
    delay(2000);
    
    pixels.begin();
    pixels.clear();
    pixels.show();
    
    Serial.println("=== SIMPLE TASK NOTIFICATION DEMO ===");
    Serial.println("Think of this like a restaurant:");
    Serial.println("- LED Worker = Cook (waits for orders)");
    Serial.println("- Button Task = Waiter (brings orders)");
    Serial.println("=====================================");
    
    // Create the LED Worker (the "cook")
    xTaskCreatePinnedToCore(
        LEDWorkerTask,
        "LEDWorker",
        10000,
        NULL,
        1,
        &LEDTaskHandle,    // SAVE this handle - it's like the cook's "phone number"
        0                  // Core 0
    );
    
    // Create the Button Task (the "waiter")
    xTaskCreatePinnedToCore(
        ButtonTask,
        "ButtonTask", 
        10000,
        NULL,
        1,
        NULL,
        1                  // Core 1
    );
    
    Serial.println("✅ Both tasks created!");
    Serial.println("Watch the communication between them...");
}

void loop() {
    // Nothing to do here - tasks handle everything!
    delay(10000);
    Serial.println("📊 Main: Tasks are running independently...");
}