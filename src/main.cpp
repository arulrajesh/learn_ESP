#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#define RGB_LED_PIN 21
#define NUM_PIXELS 1

Adafruit_NeoPixel pixels(NUM_PIXELS, RGB_LED_PIN, NEO_GRB + NEO_KHZ800);

// Global variables for inter-task communication
volatile int counter = 0;
volatile bool ledState = false;

// Task handles (references to our tasks)
TaskHandle_t Task1Handle = NULL;
TaskHandle_t Task2Handle = NULL;

// Task 1: Runs on Core 0 - Controls RGB LED
void Task1_LEDController(void *pvParameters) {
    Serial.print("Task1_LEDController running on core ");
    Serial.println(xPortGetCoreID());
    
    int colorIndex = 0;
    uint32_t colors[] = {
        pixels.Color(255, 0, 0),    // Red
        pixels.Color(0, 255, 0),    // Green
        pixels.Color(0, 0, 255),    // Blue
        pixels.Color(255, 255, 0),  // Yellow
        pixels.Color(255, 0, 255),  // Purple
        pixels.Color(0, 255, 255)   // Cyan
    };
    
    for(;;) { // Infinite loop (equivalent to while(1))
        // Change LED color
        pixels.setPixelColor(0, colors[colorIndex]);
        pixels.show();
        
        Serial.print("[Core 0] LED Color: ");
        Serial.print(colorIndex);
        Serial.print(", Counter: ");
        Serial.println(counter);
        
        colorIndex = (colorIndex + 1) % 6; // Cycle through colors
        
        vTaskDelay(1000 / portTICK_PERIOD_MS); // Wait 1000ms
    }
}

// Task 2: Runs on Core 1 - Math calculations and counter
void Task2_Calculator(void *pvParameters) {
    Serial.print("Task2_Calculator running on core ");
    Serial.println(xPortGetCoreID());
    
    for(;;) {
        // Simulate heavy calculation work
        float result = 0;
        for(int i = 0; i < 100000; i++) {
            result += sin(i * 0.001) * cos(i * 0.001);
        }
        
        // Update global counter (this happens every ~200ms)
        counter++;
        
        Serial.print("[Core 1] Calculation done. Result: ");
        Serial.print(result, 2);
        Serial.print(", Counter: ");
        Serial.println(counter);
        
        vTaskDelay(200 / portTICK_PERIOD_MS); // Wait 200ms
    }
}

// Task 3: Runs on Core 1 - Serial monitor
void Task3_SerialMonitor(void *pvParameters) {
    Serial.print("Task3_SerialMonitor running on core ");
    Serial.println(xPortGetCoreID());
    
    for(;;) {
        Serial.println("=== SYSTEM STATUS ===");
        Serial.print("Free heap memory: ");
        Serial.print(ESP.getFreeHeap());
        Serial.println(" bytes");
        
        Serial.print("Uptime: ");
        Serial.print(millis() / 1000);
        Serial.println(" seconds");
        
        Serial.print("Tasks running on Core 0: ");
        Serial.println(uxTaskGetNumberOfTasks() - 2); // Approximate
        
        Serial.println("========================");
        
        vTaskDelay(3000 / portTICK_PERIOD_MS); // Wait 3 seconds
    }
}

void setup() {
    Serial.begin(115200);
    delay(2000);
    
    // Initialize NeoPixel
    pixels.begin();
    pixels.clear();
    pixels.show();
    
    Serial.println("=== ESP32 Dual Core FreeRTOS Demo ===");
    Serial.print("Setup running on core: ");
    Serial.println(xPortGetCoreID());
    
    // Create Task 1 - LED Controller on Core 0
    xTaskCreatePinnedToCore(
        Task1_LEDController,    // Function to run
        "LEDController",        // Task name (for debugging)
        10000,                  // Stack size (bytes)
        NULL,                   // Parameters to pass to function
        1,                      // Task priority (0-25, higher = more priority)
        &Task1Handle,           // Task handle
        0                       // Core ID (0 or 1)
    );
    
    // Create Task 2 - Calculator on Core 1  
    xTaskCreatePinnedToCore(
        Task2_Calculator,       // Function to run
        "Calculator",           // Task name
        10000,                  // Stack size
        NULL,                   // Parameters
        2,                      // Priority (higher than Task1)
        &Task2Handle,           // Task handle  
        1                       // Core ID (Core 1)
    );
    
    // Create Task 3 - Serial Monitor on Core 1
    xTaskCreatePinnedToCore(
        Task3_SerialMonitor,    // Function to run
        "SerialMonitor",        // Task name
        10000,                  // Stack size
        NULL,                   // Parameters
        1,                      // Priority (same as Task1)
        NULL,                   // We don't need the handle
        1                       // Core ID (Core 1)
    );
    
    Serial.println("All tasks created successfully!");
    Serial.println("Task1 (LED) -> Core 0, Priority 1");
    Serial.println("Task2 (Calculator) -> Core 1, Priority 2");  
    Serial.println("Task3 (Monitor) -> Core 1, Priority 1");
}

void loop() {
    // The main loop() function is actually another task!
    // It runs on Core 1 by default with priority 1
    
    Serial.print("[Main Loop] Running on core: ");
    Serial.println(xPortGetCoreID());
    
    delay(5000); // Print this every 5 seconds
}