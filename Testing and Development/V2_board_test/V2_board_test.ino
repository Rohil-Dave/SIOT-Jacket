#include <Adafruit_NeoPixel.h>

#define LED_COUNT 10
#define LED_PIN MISO

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  // Some boards work best if we also make a serial connection
  Serial.begin(9600);

  /// Initialize all NeoPixel LED strips and reduce brightness
  strip.begin();
  strip.setBrightness(64);
  for (int i = 0; i < LED_COUNT; i++) {
    strip.setPixelColor(i, 0, 0, 0);
  }
}

void loop() {
  // Say hi!
  Serial.println("Red");

  for (int i = 0; i < LED_COUNT; i++) {
    strip.setPixelColor(i, 255, 0, 0);
  }
  strip.show();
  delay(3000);

  Serial.println("Off");
  for (int i = 0; i < LED_COUNT; i++) {
    strip.setPixelColor(i, 0, 0, 0);
  }
  strip.show();
  delay(3000);
}
