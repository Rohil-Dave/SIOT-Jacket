#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_MMA8451.h>
#include "Adafruit_TSL2591.h"

#define LED_COUNT 60
#define RIGHT_LED_PIN 21
#define LEFT_LED_PIN 17

#define RIGHT_VIBE_PIN 25
#define LEFT_VIBE_PIN 26

Adafruit_NeoPixel rightSleeveStrip(LED_COUNT, RIGHT_LED_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel leftSleeveStrip(LED_COUNT, LEFT_LED_PIN, NEO_GRB + NEO_KHZ800);

Adafruit_MMA8451 rightSleeveMMA = Adafruit_MMA8451();
Adafruit_MMA8451 leftSleeveMMA = Adafruit_MMA8451();

Adafruit_TSL2591 tsl = Adafruit_TSL2591(2591); // pass in a number for the sensor identifier (for your use later)

float prevRightXacc = 0; // Variable to store previous X acceleration of MMA on right sleeve
float prevLeftXacc = 0; // Variable to store previous X acceleration of MMA on left sleeve
float threshold = 4.0; // Variable to check against for change in X acceleration to trigger light change


void setup(void) {
  Serial.begin(9600);
//  Serial.println("Adafruit MMA8451 test!");
//  
//  // Initialize Right Sleeve MMA8451
//  if (! rightSleeveMMA.begin(0x1D)) { // Unique I2C address for the right sleeve accelerometer
//    Serial.println("Couldn't start right sleeve MMA8451");
//    while (1);
//  }
//  Serial.println("Right sleeve MMA8451 found!");
//
//  // Initialize Left Sleeve MMA8451
//  if (! leftSleeveMMA.begin(0x1C)) { // Unique I2C address for the left sleeve accelerometer, 0x1C requires A pin to GND
//    Serial.println("Couldn't start left sleeve MMA8451");
//    while (1);
//  }
//  Serial.println("Left sleeve MMA8451 found!");
//  
//  rightSleeveMMA.setRange(MMA8451_RANGE_2_G);
//  leftSleeveMMA.setRange(MMA8451_RANGE_2_G);

  pinMode(RIGHT_VIBE_PIN, OUTPUT);
  pinMode(LEFT_VIBE_PIN, OUTPUT);

  rightSleeveStrip.begin(); // Initialize Right NeoPixel LED strip
  rightSleeveStrip.show(); // Initialize all pixels to 'off'
  rightSleeveStrip.setBrightness(64);  // range: 0 to 255

  leftSleeveStrip.begin(); // Initialize Left NeoPixel LED strip
  leftSleeveStrip.show(); // Initialize all pixels to 'off'
  leftSleeveStrip.setBrightness(64);  // range: 0 to 255

  Serial.println(F("Starting Adafruit TSL2591 Test!"));
  
  if (tsl.begin()) 
  {
    Serial.println(F("Found a TSL2591 sensor"));
  } 
  else 
  {
    Serial.println(F("No sensor found ... check your wiring?"));
    while (1);
  }
    
  /* Display some basic information on this sensor */
  displaySensorDetails();
  
  /* Configure the sensor */
  configureSensor();
}

/**************************************************************************/
/*
    Displays some basic information on this sensor from the unified
    sensor API sensor_t type (see Adafruit_Sensor for more information)
*/
/**************************************************************************/
void displaySensorDetails(void)
{
  sensor_t sensor;
  tsl.getSensor(&sensor);
  Serial.println(F("------------------------------------"));
  Serial.print  (F("Sensor:       ")); Serial.println(sensor.name);
  Serial.print  (F("Driver Ver:   ")); Serial.println(sensor.version);
  Serial.print  (F("Unique ID:    ")); Serial.println(sensor.sensor_id);
  Serial.print  (F("Max Value:    ")); Serial.print(sensor.max_value); Serial.println(F(" lux"));
  Serial.print  (F("Min Value:    ")); Serial.print(sensor.min_value); Serial.println(F(" lux"));
  Serial.print  (F("Resolution:   ")); Serial.print(sensor.resolution, 4); Serial.println(F(" lux"));  
  Serial.println(F("------------------------------------"));
  Serial.println(F(""));
  delay(500);
}

/**************************************************************************/
/*
    Configures the gain and integration time for the TSL2591
*/
/**************************************************************************/
void configureSensor(void)
{
  // You can change the gain on the fly, to adapt to brighter/dimmer light situations
  //tsl.setGain(TSL2591_GAIN_LOW);    // 1x gain (bright light)
  tsl.setGain(TSL2591_GAIN_MED);      // 25x gain
  //tsl.setGain(TSL2591_GAIN_HIGH);   // 428x gain
  
  // Changing the integration time gives you a longer time over which to sense light
  // longer timelines are slower, but are good in very low light situtations!
  //tsl.setTiming(TSL2591_INTEGRATIONTIME_100MS);  // shortest integration time (bright light)
  // tsl.setTiming(TSL2591_INTEGRATIONTIME_200MS);
  tsl.setTiming(TSL2591_INTEGRATIONTIME_300MS);
  // tsl.setTiming(TSL2591_INTEGRATIONTIME_400MS);
  // tsl.setTiming(TSL2591_INTEGRATIONTIME_500MS);
  // tsl.setTiming(TSL2591_INTEGRATIONTIME_600MS);  // longest integration time (dim light)

  /* Display the gain and integration time for reference sake */  
  Serial.println(F("------------------------------------"));
  Serial.print  (F("Gain:         "));
  tsl2591Gain_t gain = tsl.getGain();
  switch(gain)
  {
    case TSL2591_GAIN_LOW:
      Serial.println(F("1x (Low)"));
      break;
    case TSL2591_GAIN_MED:
      Serial.println(F("25x (Medium)"));
      break;
    case TSL2591_GAIN_HIGH:
      Serial.println(F("428x (High)"));
      break;
    case TSL2591_GAIN_MAX:
      Serial.println(F("9876x (Max)"));
      break;
  }
  Serial.print  (F("Timing:       "));
  Serial.print((tsl.getTiming() + 1) * 100, DEC); 
  Serial.println(F(" ms"));
  Serial.println(F("------------------------------------"));
  Serial.println(F(""));
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

/**************************************************************************/
/*
    Show how to read IR and Full Spectrum at once and convert to lux
*/
/**************************************************************************/
void advancedRead(void)
{
  // More advanced data read example. Read 32 bits with top 16 bits IR, bottom 16 bits full spectrum
  // That way you can do whatever math and comparisons you want!
  uint32_t lum = tsl.getFullLuminosity();
  uint16_t ir, full;
  ir = lum >> 16;
  full = lum & 0xFFFF;
  Serial.print(F("[ ")); Serial.print(millis()); Serial.print(F(" ms ] "));
  Serial.print(F("IR: ")); Serial.print(ir);  Serial.print(F("  "));
  Serial.print(F("Full: ")); Serial.print(full); Serial.print(F("  "));
  Serial.print(F("Visible: ")); Serial.print(full - ir); Serial.print(F("  "));
  Serial.print(F("Lux: ")); Serial.println(tsl.calculateLux(full, ir), 6);
}

// Test if LEDs glow on low light
void glowOnDark(void) {
  
  uint32_t lum = tsl.getFullLuminosity();
  uint16_t ir, full;
  ir = lum >> 16;
  full = lum & 0xFFFF;
  if (full - ir < 1000) {
    for (int i = 0; i < LED_COUNT; i++) {
      leftSleeveStrip.setPixelColor(i, 225, 0, 0); // Red
    }
    leftSleeveStrip.show();  // Turn on LEDs 
  }
    else {
    for (int i = 0; i < LED_COUNT; i++) {
      leftSleeveStrip.setPixelColor(i, 0, 0, 0); // Off
    }
    leftSleeveStrip.show();  // Turn on LEDs 
  }
}

void loop() {
  // Process right sleeve and print its data
  // processSleeve(rightSleeveMMA, rightSleeveStrip, prevRightXacc, "Right Sleeve", RIGHT_VIBE_PIN);

  // Process left sleeve and print its data
  // processSleeve(leftSleeveMMA, leftSleeveStrip, prevLeftXacc, "Left Sleeve", LEFT_VIBE_PIN);

  advancedRead();
  glowOnDark();
  delay(1000);
}
