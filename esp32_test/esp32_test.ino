#include <Adafruit_NeoPixel.h>



#define LED_COUNT 1
#define LED_PIN 21

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);


void setup() {
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  strip.setBrightness(64);  // range: 0 to 255
}

void loop() {
  strip.setPixelColor(0, 255, 0, 0);
  strip.show();
  delay(3000);
  strip.setPixelColor(0, 0, 0, 255);
  strip.show();
  delay(3000);
}
