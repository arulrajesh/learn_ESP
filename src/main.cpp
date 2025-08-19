#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#define RGB_LED_PIN 21
#define NUM_PIXELS 1

Adafruit_NeoPixel pixels(NUM_PIXELS, RGB_LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
    Serial.begin(115200);
    delay(2000); // Give time for USB CDC to initialize
    
    pixels.begin();
    pixels.clear();
    pixels.show();
    
    Serial.println("ESP32-S3-Zero Ready!");
    Serial.println("RGB LED Test Starting...");
}

void loop() {
    Serial.println("Red");
    pixels.setPixelColor(0, pixels.Color(255, 0, 0));
    pixels.show();
    delay(1000);
    
    Serial.println("Green");
    pixels.setPixelColor(0, pixels.Color(0, 255, 0));
    pixels.show();
    delay(1000);
    
    Serial.println("Blue");
    pixels.setPixelColor(0, pixels.Color(0, 0, 255));
    pixels.show();
    delay(1000);
    
    Serial.println("Off");
    pixels.setPixelColor(0, pixels.Color(0, 0, 0));
    pixels.show();
    delay(1000);
}