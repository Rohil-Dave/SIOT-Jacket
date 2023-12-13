// Include all libraries needed
#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_MMA8451.h>
#include "Adafruit_TSL2591.h"
#include <WiFi.h>
#include <SPIFFS.h>

// Define pins for each strip of 10 neopixels
#define LED_COUNT 10
#define RIGHT_LED_PIN 8  // TX
#define LEFT_LED_PIN 7   // RX
#define FRONT_LED_PIN 21 // MI
#define BACK_LED_PIN 19  // MO
// Define pins for each haptic vibe motor
#define RIGHT_VIBE_PIN 15 // 15
#define LEFT_VIBE_PIN 27  // 27

#define FILE_PATH "/ride_data.csv"
File dataFile;

// WiFi network details
const char* ssid     = "2 Childish";
const char* password = "Gambino3005";

// Initialize all LED strips
Adafruit_NeoPixel rightSleeveStrip(LED_COUNT, RIGHT_LED_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel leftSleeveStrip(LED_COUNT, LEFT_LED_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel frontStrip(LED_COUNT, FRONT_LED_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel backStrip(LED_COUNT, BACK_LED_PIN, NEO_GRB + NEO_KHZ800);
// Initialize both accelerometer
Adafruit_MMA8451 rightSleeveMMA = Adafruit_MMA8451();
Adafruit_MMA8451 leftSleeveMMA = Adafruit_MMA8451();

// Add global variables to track the timing for right and left sleeves
unsigned long rightSleeveTimer = 0;
unsigned long leftSleeveTimer = 0;
bool rightSleeveActive = false;
bool leftSleeveActive = false;

unsigned long lastTurnUpdate = 0;
const long turnUpdateInterval = 500;  // Update interval of half second for raise of arm for turn signal

// Adjust the threshold values for each axis
float xThreshold = 4.0; // Variable to check against for change in X acceleration to trigger light change
float yThreshold = 4.0; // Variable to check against for change in Y acceleration to trigger light change
float zThreshold = 4.0; // Variable to check against for change in Z acceleration to trigger light change

float prevRightXacc = 0; // Variable to store previous X acceleration of MMA on right sleeve
float prevRightYacc = 0; // Variable to store previous Y acceleration of MMA on right sleeve
float prevRightZacc = 0; // Variable to store previous Z acceleration of MMA on right sleeve
float prevLeftXacc = 0; // Variable to store previous X acceleration of MMA on left sleeve
float prevLeftYacc = 0; // Variable to store previous Y acceleration of MMA on left sleeve
float prevLeftZacc = 0; // Variable to store previous Z acceleration of MMA on left sleeve

// Initialize light sensor
Adafruit_TSL2591 TSL = Adafruit_TSL2591(2591); // pass in a number for the sensor identifier (for your use later)

unsigned long lastGlowUpdate = 0;
const long glowUpdateInterval = 2000;  // Update interval of 2 seconds for ambient light
bool glowActive = false;

unsigned long lastStoreUpdate = 0;
const long storeInterval = 500; // How often data entry is recorded, upper bounded by turnUpdateInterval

void setup(void) {
  Serial.begin(115200);
  Serial.println("SETUP");
  dataFile = SPIFFS.open(FILE_PATH, "a"); // Initializes write to file for ride_data.csv
  if (dataFile) {
    Serial.println("Printing to file...");
    dataFile.println("Time elapsed, Glow State, Right Sleeve State, Right X Acc, Right Y Acc, Right Z Acc, Left Sleeve State, Left X Acc, Left Y Acc, Left Z Acc");
  }

  Serial.println("Starting Adafruit MMA8451 test!");
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
    Serial.println(F("TSL2591 found!"));
  } 
  else 
  {
    Serial.println(F("No sensor found ... check your wiring?"));
    while (1);
  }

  // Configure light sensor
  TSL.setGain(TSL2591_GAIN_MED);
  TSL.setTiming(TSL2591_INTEGRATIONTIME_300MS);
  
  // Initialize each vibe motor
  pinMode(RIGHT_VIBE_PIN, OUTPUT);
  pinMode(LEFT_VIBE_PIN, OUTPUT);

  Serial.println();
  Serial.print("[WiFi] Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
// Auto reconnect is set true as default
// To set auto connect off, use the following function
//    WiFi.setAutoReconnect(false);

    // Will try for about 10 seconds (20x 500ms)
  int tryDelay = 500;
  int numberOfTries = 20;

    // Wait for the WiFi event
  while (true) {
        
    switch(WiFi.status()) {
      case WL_NO_SSID_AVAIL:
        Serial.println("[WiFi] SSID not found");
        break;
      case WL_CONNECT_FAILED:
        Serial.print("[WiFi] Failed - WiFi not connected! Reason: ");
        return;
        break;
      case WL_CONNECTION_LOST:
        Serial.println("[WiFi] Connection was lost");
        break;
      case WL_SCAN_COMPLETED:
        Serial.println("[WiFi] Scan is completed");
        break;
      case WL_DISCONNECTED:
        Serial.println("[WiFi] WiFi is disconnected");
        break;
      case WL_CONNECTED:
        Serial.println("[WiFi] WiFi is connected!");
        Serial.print("[WiFi] IP address: ");
        Serial.println(WiFi.localIP());
        return;
        break;
      default:
        Serial.print("[WiFi] WiFi Status: ");
        Serial.println(WiFi.status());
        break;
    }
    delay(tryDelay);
        
    if(numberOfTries <= 0){
      Serial.print("[WiFi] Failed to connect to WiFi!");
      // Use disconnect function to force stop trying to connect
      WiFi.disconnect();
      return;
    } else {
      numberOfTries--;
    }
  }
}

// Function to write states and sensor values to SPIFFS
void writeToSPIFFS(float currentMillis) {
  if (dataFile){
    dataFile.print(currentMillis); dataFile.print(",");
    dataFile.print(glowActive); dataFile.print(",");
    dataFile.print(rightSleeveActive); dataFile.print(",");
    dataFile.print(prevRightXacc); dataFile.print(",");
    dataFile.print(prevRightYacc); dataFile.print(",");
    dataFile.print(prevRightZacc); dataFile.print(",");
    dataFile.print(leftSleeveActive); dataFile.print(",");
    dataFile.print(prevLeftXacc); dataFile.print(",");
    dataFile.print(prevLeftYacc); dataFile.print(",");
    dataFile.println(prevLeftZacc);
  }
  
}

// Function that checks respective change in acceleration to activate LEDs and vibe motor on right or left sleeve
void processSleeve(Adafruit_MMA8451 &mma, Adafruit_NeoPixel &strip, float &prevXacc, float &prevYacc, float &prevZacc, const char* sleeveName, unsigned long &sleeveTimer, bool &sleeveActive) {
  mma.read();
  sensors_event_t event; 
  mma.getEvent(&event);

  // Printing the accelerometer data for the specific sleeve
  // Serial.print(sleeveName);
  // Serial.print(" X:\t"); Serial.print(event.acceleration.x); Serial.print("\t");
  // Serial.print("Y:\t"); Serial.print(event.acceleration.y); Serial.print("\t");
  // Serial.print("Z:\t"); Serial.print(event.acceleration.z); Serial.println(" m/s^2");

  // Record current accelerations in all directions
  float currentXacc = event.acceleration.x;
  float currentYacc = event.acceleration.y;
  float currentZacc = event.acceleration.z;

  // Calculate the change in acceleration for each axis
  float deltaX = abs(currentXacc - prevXacc);
  float deltaY = abs(currentYacc - prevYacc);
  float deltaZ = abs(currentZacc - prevZacc);

  // Update the previous acceleration values
  prevXacc = currentXacc;
  prevYacc = currentYacc;
  prevZacc = currentZacc;

  if (!sleeveActive) {
    if (deltaX > xThreshold && deltaY > yThreshold && deltaZ > zThreshold) {
      sleeveActive = true;  // Turn on active state
      sleeveTimer = millis(); // Start timer
      for (int i = 0; i < LED_COUNT; i++) {
        strip.setPixelColor(i, 200, 200, 200); // White-ish color
        strip.show();  // Turn on LEDs
        delay(50);
      }
    }
  } else if (millis() - sleeveTimer >= 3000) {
    for (int i = 0; i < LED_COUNT; i++) {
      strip.setPixelColor(i, 0, 0, 0); // No color
    }
    strip.show(); // Turn off LEDs
    sleeveActive = false; // Switch to off state
  }
}


// Function that activates front and back LEDs during low light conditions
void glowOnDark(void) {
  
  // More advanced data read example. Read 32 bits with top 16 bits IR, bottom 16 bits full spectrum
  // That way you can do whatever math and comparisons you want!
  uint32_t lum = TSL.getFullLuminosity();
  uint16_t ir, full;
  ir = lum >> 16;
  full = lum & 0xFFFF;

  if (full - ir < 300) {
    glowActive = true;
    for (int i = 0; i < LED_COUNT; i++) {
      frontStrip.setPixelColor(i, 200, 200, 200); // White-ish color
      backStrip.setPixelColor(i, 200, 200, 200);
    }
    frontStrip.show();
    backStrip.show();// Turn on LEDs
    //digitalWrite(RIGHT_VIBE_PIN, HIGH); // Turn on vibe motors
    //digitalWrite(LEFT_VIBE_PIN, HIGH); 
  }
  else {
    glowActive = false;
    for (int i = 0; i < LED_COUNT; i++) {
      frontStrip.setPixelColor(i, 0, 0, 0); // Off
      backStrip.setPixelColor(i, 0, 0, 0);
    }
    frontStrip.show();  // Turn off LEDs
    backStrip.show();
    //digitalWrite(RIGHT_VIBE_PIN, LOW); // Turn off vibe motors
    //digitalWrite(LEFT_VIBE_PIN, LOW); 
  }
}

void loop() {
  // Print the data
  float currentMillis = millis();
  if (currentMillis - lastStoreUpdate >= storeInterval) {
    writeToSPIFFS(currentMillis);
    lastStoreUpdate = currentMillis;
  }
  // Ambient Light Activated LEDs
  if (millis() - lastGlowUpdate >= glowUpdateInterval) {
    glowOnDark();
    lastGlowUpdate = millis();
  }
  // Turn Signal Activated LEDs
  if (millis() - lastTurnUpdate >= turnUpdateInterval) {
    processSleeve(rightSleeveMMA, rightSleeveStrip, prevRightXacc, prevRightYacc, prevRightZacc, "Right Sleeve", rightSleeveTimer, rightSleeveActive);
    processSleeve(leftSleeveMMA, leftSleeveStrip, prevLeftXacc, prevLeftYacc, prevLeftZacc, "Left Sleeve", leftSleeveTimer, leftSleeveActive);
    lastTurnUpdate = millis();
  }
}