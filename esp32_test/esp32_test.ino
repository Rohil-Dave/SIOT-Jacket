#include <Adafruit_NeoPixel.h>



#define LED_COUNT 60
#define LED_PIN 21

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);


void setup() {
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  strip.setBrightness(64);  // range: 0 to 255
}

void loop() {
  // Set all pixels to red
  for (int i = 0; i < LED_COUNT; i++) {
    strip.setPixelColor(i, 255, 0, 0); // Red color
  }
  strip.show();
  delay(3000); // Wait for 3 seconds

  // Set all pixels to blue
  for (int i = 0; i < LED_COUNT; i++) {
    strip.setPixelColor(i, 0, 0, 255); // Blue color
  }
  strip.show();
  delay(3000); // Wait for 3 seconds
}
