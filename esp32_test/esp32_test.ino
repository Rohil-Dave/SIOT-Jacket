// Include all libraries needed
#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_MMA8451.h>
#include "Adafruit_TSL2591.h"

// Define pins for each strip of 10 neopixels
#define LED_COUNT 10
#define RIGHT_LED_PIN 8  // TX
#define LEFT_LED_PIN 7   // RX
#define FRONT_LED_PIN 21 // MI
#define BACK_LED_PIN 19  // MO
// Define pins for each haptic vibe motor
#define RIGHT_VIBE_PIN 15 // 15
#define LEFT_VIBE_PIN 27  // 27

// Initialize all LED strips
Adafruit_NeoPixel rightSleeveStrip(LED_COUNT, RIGHT_LED_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel leftSleeveStrip(LED_COUNT, LEFT_LED_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel frontStrip(LED_COUNT, FRONT_LED_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel backStrip(LED_COUNT, BACK_LED_PIN, NEO_GRB + NEO_KHZ800);
// Initialize both accelerometer
Adafruit_MMA8451 rightSleeveMMA = Adafruit_MMA8451();
Adafruit_MMA8451 leftSleeveMMA = Adafruit_MMA8451();
// Initialize light sensor
Adafruit_TSL2591 TSL = Adafruit_TSL2591(2591); // pass in a number for the sensor identifier (for your use later)

// Add global variables to track the timing for right and left sleeves
unsigned long rightSleeveTimer = 0;
unsigned long leftSleeveTimer = 0;
bool rightSleeveActive = false;
bool leftSleeveActive = false;

unsigned long lastUpdateTime = 0;
const long updateInterval = 1000;  // Update interval of 1 second

// Adjust the threshold values for each axis
float xThreshold = 8.0; // Variable to check against for change in X acceleration to trigger light change
float yThreshold = 8.0; // Variable to check against for change in Y acceleration to trigger light change
float zThreshold = 8.0; // Variable to check against for change in Z acceleration to trigger light change

float prevRightXacc = 0; // Variable to store previous X acceleration of MMA on right sleeve
float prevRightYacc = 0; // Variable to store previous Y acceleration of MMA on right sleeve
float prevRightZacc = 0; // Variable to store previous Z acceleration of MMA on right sleeve
float prevLeftXacc = 0; // Variable to store previous X acceleration of MMA on left sleeve
float prevLeftYacc = 0; // Variable to store previous Y acceleration of MMA on left sleeve
float prevLeftZacc = 0; // Variable to store previous Z acceleration of MMA on left sleeve


void setup(void) {
  Serial.begin(9600);
  Serial.println("Adafruit MMA8451 test!");
  
  // Initialize Right Sleeve MMA8451
  if (! rightSleeveMMA.begin(0x1C)) { // Unique I2C address for the right sleeve accelerometer, 0x1C requires A pin to GND
    Serial.println("Couldn't start right sleeve MMA8451");
    while (1);
  }
  Serial.println("Right sleeve MMA8451 found!");

  // Initialize Left Sleeve MMA8451
  if (! leftSleeveMMA.begin(0x1D)) { // Unique I2C address for the left sleeve accelerometer
    Serial.println("Couldn't start left sleeve MMA8451");
    while (1);
  }
  Serial.println("Left sleeve MMA8451 found!");

  // Set range for accelerometers to 2G
  rightSleeveMMA.setRange(MMA8451_RANGE_2_G);
  leftSleeveMMA.setRange(MMA8451_RANGE_2_G);
  
  rightSleeveStrip.begin();  // Initialize all NeoPixel LED strips and reduce brightness
  rightSleeveStrip.setBrightness(64);
  leftSleeveStrip.begin();
  leftSleeveStrip.setBrightness(64);
  frontStrip.begin();
  frontStrip.setBrightness(64);
  backStrip.begin();
  backStrip.setBrightness(64);  // range: 0 to 255
  
  for (int i = 0; i < LED_COUNT; i++) {
      rightSleeveStrip.setPixelColor(i, 0, 0, 0);  // No color
      leftSleeveStrip.setPixelColor(i, 0, 0, 0);
      frontStrip.setPixelColor(i, 0, 0, 0);
      backStrip.setPixelColor(i, 0, 0, 0);
    }
  rightSleeveStrip.show();  // Initialize all pixels to 'off'
  leftSleeveStrip.show();
  frontStrip.show();
  backStrip.show();
  
  // Initialize ambient light sensor
  Serial.println(F("Starting Adafruit TSL2591 Test!"));
  
  if (TSL.begin()) 
  {
    Serial.println(F("Found a TSL2591 sensor"));
  } 
  else 
  {
    Serial.println(F("No sensor found ... check your wiring?"));
    while (1);
  }
    
  /* Display some basic information on this sensor */
  // displaySensorDetails();
  
  /* Configure the sensor */
  // configureSensor();
  
  // Initialize each vibe motor
  pinMode(RIGHT_VIBE_PIN, OUTPUT);
  pinMode(LEFT_VIBE_PIN, OUTPUT);
}


// Function that checks respective change in acceleration to activate LEDs and vibe motor on right or left sleeve
void processSleeve(Adafruit_MMA8451 &mma, Adafruit_NeoPixel &strip, float &prevXacc, float &prevYacc, float &prevZacc, const char* sleeveName, unsigned long &sleeveTimer, bool &sleeveActive) {
  mma.read();
  sensors_event_t event; 
  mma.getEvent(&event);

  // Printing the accelerometer data for the specific sleeve
  Serial.print(sleeveName);
  Serial.print(" X:\t"); Serial.print(event.acceleration.x); Serial.print("\t");
  Serial.print("Y:\t"); Serial.print(event.acceleration.y); Serial.print("\t");
  Serial.print("Z:\t"); Serial.print(event.acceleration.z); Serial.println(" m/s^2");

  // Calculate the change in acceleration for each axis
  float deltaX = abs(event.acceleration.x - prevXacc);
  float deltaY = abs(event.acceleration.y - prevYacc);
  float deltaZ = abs(event.acceleration.z - prevZacc);

  if (deltaX > xThreshold && deltaY > yThreshold && deltaZ > zThreshold) {
    if (!sleeveActive) {
      for (int i = 0; i < LED_COUNT; i++) {
        strip.setPixelColor(i, 200, 200, 200); // White-ish color
      }
      strip.show();  // Turn on LEDs
      sleeveActive = true;
      sleeveTimer = millis();
    }
  } else if (millis() - sleeveTimer >= 3000 || !sleeveActive) {
    for (int i = 0; i < LED_COUNT; i++) {
      strip.setPixelColor(i, 0, 0, 0); // No color
    }
    strip.show(); // Turn off LEDs
    sleeveActive = false;
  }
  
  // Update the previous acceleration values
  prevXacc = event.acceleration.x;
  prevYacc = event.acceleration.y;
  prevZacc = event.acceleration.z;
}


// Test if LEDs glow on low light
void glowOnDark(void) {
  
  uint32_t lum = TSL.getFullLuminosity();
  uint16_t ir, full;
  ir = lum >> 16;
  full = lum & 0xFFFF;
  if (full - ir < 500) {
    for (int i = 0; i < LED_COUNT; i++) {
      frontStrip.setPixelColor(i, 200, 200, 200); // White-ish color
      backStrip.setPixelColor(i, 200, 200, 200);
    }
    frontStrip.show();
    backStrip.show();// Turn on LEDs
    digitalWrite(RIGHT_VIBE_PIN, HIGH); // Turn on vibe motors
    digitalWrite(LEFT_VIBE_PIN, HIGH); 
  }
    else {
    for (int i = 0; i < LED_COUNT; i++) {
      frontStrip.setPixelColor(i, 0, 0, 0); // Off
      backStrip.setPixelColor(i, 0, 0, 0);
    }
    frontStrip.show();  // Turn off LEDs
    backStrip.show();
    digitalWrite(RIGHT_VIBE_PIN, LOW); // Turn off vibe motors
    digitalWrite(LEFT_VIBE_PIN, LOW); 
  }
}

void loop() {
  // Process right sleeve and print its data
  processSleeve(rightSleeveMMA, rightSleeveStrip, prevRightXacc, prevRightYacc, prevRightZacc, "Right Sleeve", rightSleeveTimer, rightSleeveActive);

  // Process left sleeve and print its data
  processSleeve(leftSleeveMMA, leftSleeveStrip, prevLeftXacc, prevLeftYacc, prevLeftZacc, "Left Sleeve", leftSleeveTimer, leftSleeveActive);

  // advancedRead();

  // Perform regular actions every second
  if (millis() - lastUpdateTime >= updateInterval) {
    glowOnDark();
    lastUpdateTime = millis();
  }
}

/**************************************************************************/
/*
    Show how to read IR and Full Spectrum at once and convert to lux
*/
/**************************************************************************/
//void advancedRead(void)
//{
//  // More advanced data read example. Read 32 bits with top 16 bits IR, bottom 16 bits full spectrum
//  // That way you can do whatever math and comparisons you want!
//  uint32_t lum = tsl.getFullLuminosity();
//  uint16_t ir, full;
//  ir = lum >> 16;
//  full = lum & 0xFFFF;
//  Serial.print(F("[ ")); Serial.print(millis()); Serial.print(F(" ms ] "));
//  Serial.print(F("IR: ")); Serial.print(ir);  Serial.print(F("  "));
//  Serial.print(F("Full: ")); Serial.print(full); Serial.print(F("  "));
//  Serial.print(F("Visible: ")); Serial.print(full - ir); Serial.print(F("  "));
//  Serial.print(F("Lux: ")); Serial.println(tsl.calculateLux(full, ir), 6);
//}
//

/**************************************************************************/
/*
    Displays some basic information on this sensor from the unified
    sensor API sensor_t type (see Adafruit_Sensor for more information)
*/
/**************************************************************************/
//void displaySensorDetails(void)
//{
//  sensor_t sensor;
//  tsl.getSensor(&sensor);
//  Serial.println(F("------------------------------------"));
//  Serial.print  (F("Sensor:       ")); Serial.println(sensor.name);
//  Serial.print  (F("Driver Ver:   ")); Serial.println(sensor.version);
//  Serial.print  (F("Unique ID:    ")); Serial.println(sensor.sensor_id);
//  Serial.print  (F("Max Value:    ")); Serial.print(sensor.max_value); Serial.println(F(" lux"));
//  Serial.print  (F("Min Value:    ")); Serial.print(sensor.min_value); Serial.println(F(" lux"));
//  Serial.print  (F("Resolution:   ")); Serial.print(sensor.resolution, 4); Serial.println(F(" lux"));  
//  Serial.println(F("------------------------------------"));
//  Serial.println(F(""));
//  delay(500);
//}

/**************************************************************************/
/*
    Configures the gain and integration time for the TSL2591
*/
/**************************************************************************/
//void configureSensor(void)
//{
//  // You can change the gain on the fly, to adapt to brighter/dimmer light situations
//  //tsl.setGain(TSL2591_GAIN_LOW);    // 1x gain (bright light)
//  tsl.setGain(TSL2591_GAIN_MED);      // 25x gain
//  //tsl.setGain(TSL2591_GAIN_HIGH);   // 428x gain
//  
//  // Changing the integration time gives you a longer time over which to sense light
//  // longer timelines are slower, but are good in very low light situtations!
//  //tsl.setTiming(TSL2591_INTEGRATIONTIME_100MS);  // shortest integration time (bright light)
//  // tsl.setTiming(TSL2591_INTEGRATIONTIME_200MS);
//  tsl.setTiming(TSL2591_INTEGRATIONTIME_300MS);
//  // tsl.setTiming(TSL2591_INTEGRATIONTIME_400MS);
//  // tsl.setTiming(TSL2591_INTEGRATIONTIME_500MS);
//  // tsl.setTiming(TSL2591_INTEGRATIONTIME_600MS);  // longest integration time (dim light)
//
//  /* Display the gain and integration time for reference sake */ 
//  Serial.println(F("------------------------------------"));
//  Serial.print  (F("Gain:         "));
//  tsl2591Gain_t gain = tsl.getGain();
//  switch(gain)
//  {
//    case TSL2591_GAIN_LOW:
//      Serial.println(F("1x (Low)"));
//      break;
//    case TSL2591_GAIN_MED:
//      Serial.println(F("25x (Medium)"));
//      break;
//    case TSL2591_GAIN_HIGH:
//      Serial.println(F("428x (High)"));
//      break;
//    case TSL2591_GAIN_MAX:
//      Serial.println(F("9876x (Max)"));
//      break;
//  }
//  Serial.print  (F("Timing:       "));
//  Serial.print((tsl.getTiming() + 1) * 100, DEC); 
//  Serial.println(F(" ms"));
//  Serial.println(F("------------------------------------"));
//  Serial.println(F(""));
//}