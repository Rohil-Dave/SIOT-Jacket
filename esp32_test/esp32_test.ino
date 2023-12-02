#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_MMA8451.h>

#define LED_COUNT 60
#define RIGHT_LED_PIN 21
#define LEFT_LED_PIN 17

#define RIGHT_VIBE_PIN 34
#define LEFT_VIBE_PIN 25

Adafruit_NeoPixel rightSleeveStrip(LED_COUNT, RIGHT_LED_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel leftSleeveStrip(LED_COUNT, LEFT_LED_PIN, NEO_GRB + NEO_KHZ800);

Adafruit_MMA8451 rightSleeveMMA = Adafruit_MMA8451();
Adafruit_MMA8451 leftSleeveMMA = Adafruit_MMA8451();

float prevRightXacc = 0; // Variable to store previous X acceleration of MMA on right sleeve
float prevLeftXacc = 0; // Variable to store previous X acceleration of MMA on left sleeve
float threshold = 4.0; // Variable to check against for change in X acceleration to trigger light change


void setup(void) {
  Serial.begin(9600);
  Serial.println("Adafruit MMA8451 test!");
  
  // Initialize Right Sleeve MMA8451
  if (! rightSleeveMMA.begin(0x1D)) { // Unique I2C address for the right sleeve accelerometer
    Serial.println("Couldn't start right sleeve MMA8451");
    while (1);
  }
  Serial.println("Right sleeve MMA8451 found!");

  // Initialize Left Sleeve MMA8451
  if (! leftSleeveMMA.begin(0x1C)) { // Unique I2C address for the left sleeve accelerometer, 0x1C requires A pin to GND
    Serial.println("Couldn't start left sleeve MMA8451");
    while (1);
  }
  Serial.println("Left sleeve MMA8451 found!");
  
  rightSleeveMMA.setRange(MMA8451_RANGE_2_G);
  leftSleeveMMA.setRange(MMA8451_RANGE_2_G);

  pinMode(RIGHT_VIBE_PIN, OUTPUT);
  pinMode(LEFT_VIBE_PIN, OUTPUT);

  rightSleeveStrip.begin(); // Initialize Right NeoPixel LED strip
  rightSleeveStrip.show(); // Initialize all pixels to 'off'
  rightSleeveStrip.setBrightness(64);  // range: 0 to 255

  leftSleeveStrip.begin(); // Initialize Left NeoPixel LED strip
  leftSleeveStrip.show(); // Initialize all pixels to 'off'
  leftSleeveStrip.setBrightness(64);  // range: 0 to 255
}

// Function that checks respective change in acceleration to activate LEDs and vibe motor on right or left sleeve
void processSleeve(Adafruit_MMA8451 &mma, Adafruit_NeoPixel &strip, float &prevXacc, const char* sleeveName, int motorPin) {
  mma.read();
  sensors_event_t event; 
  mma.getEvent(&event);

  // Printing the accelerometer data for the specific sleeve
  Serial.print(sleeveName);
  Serial.print(" X:\t"); Serial.print(event.acceleration.x); Serial.print("\t");
  Serial.print("Y:\t"); Serial.print(event.acceleration.y); Serial.print("\t");
  Serial.print("Z:\t"); Serial.print(event.acceleration.z); Serial.println(" m/s^2");

  float currentXacc = event.acceleration.x;

  if (abs(currentXacc - prevXacc) > threshold) {
    for (int i = 0; i < LED_COUNT; i++) {
      strip.setPixelColor(i, 225, 0, 0); // White-ish color
    }
    strip.show();  // Turn on LEDs
    digitalWrite(motorPin, HIGH); // Turn on vibe motor
    delay(3000);
    digitalWrite(motorPin, LOW); // Turn off vibe motor
  } else {
    for (int i = 0; i < LED_COUNT; i++) {
      strip.setPixelColor(i, 0, 0, 0); // Off
    }
    strip.show();
  }
  
  prevXacc = currentXacc;
}

void loop() {
  // Process right sleeve and print its data
  processSleeve(rightSleeveMMA, rightSleeveStrip, prevRightXacc, "Right Sleeve", RIGHT_VIBE_PIN);

  // Process left sleeve and print its data
  processSleeve(leftSleeveMMA, leftSleeveStrip, prevLeftXacc, "Left Sleeve", LEFT_VIBE_PIN);

  delay(1000);
}
