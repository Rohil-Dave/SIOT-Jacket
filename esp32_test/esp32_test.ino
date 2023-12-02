#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_MMA8451.h>

#define LED_COUNT 60
#define LED_PIN 21
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

Adafruit_MMA8451 mma = Adafruit_MMA8451();

float prevXacc = 0; // Variable to store previous X acceleration
float threshold = 2.0; // Variable to check against for change in X acceleration to trigger light change


void setup(void) {
  Serial.begin(9600);
  Serial.println("Adafruit MMA8451 test!");
  
  if (! mma.begin()) {
    Serial.println("Couldnt start");
    while (1);
  }
  Serial.println("MMA8451 found!");
  
  mma.setRange(MMA8451_RANGE_2_G);
  
  Serial.print("Range = "); Serial.print(2 << mma.getRange());  
  Serial.println("G");

  strip.begin(); // Initialize NeoPixel LED
  strip.show(); // Initialize all pixels to 'off'
  strip.setBrightness(100);  // range: 0 to 255
}

void loop() {
  // Read the 'raw' data in 14-bit counts
  mma.read();
  Serial.print("X:\t"); Serial.print(mma.x); 
  Serial.print("\tY:\t"); Serial.print(mma.y); 
  Serial.print("\tZ:\t"); Serial.print(mma.z); 
  Serial.println();

  /* Get a new sensor event */ 
  sensors_event_t event; 
  mma.getEvent(&event);

  /* Display the results (acceleration is measured in m/s^2) */
  Serial.print("X: \t"); Serial.print(event.acceleration.x); Serial.print("\t");
  Serial.print("Y: \t"); Serial.print(event.acceleration.y); Serial.print("\t");
  Serial.print("Z: \t"); Serial.print(event.acceleration.z); Serial.print("\t");
  Serial.println("m/s^2 ");

  // Get the current acceleration
  float currentXacc = event.acceleration.x;

  // Check for a change in acceleration;
  if (abs(currentXacc - prevXacc) > threshold) {
    // Turn on the Neopixel
    for (int i = 0; i < LED_COUNT; i++) {
    strip.setPixelColor(i, 200, 200, 200); // White-ish color
    }
    strip.show();
    delay(3000);
  } else {
    //Turn off the NeoPixel
    for (int i = 0; i < LED_COUNT; i++) {
    strip.setPixelColor(i, 0, 0, 0); // Off
    }
    strip.show();
  }
  
  prevXacc = currentXacc;
  
  
  /* Get the orientation of the sensor */
  uint8_t o = mma.getOrientation();
  
  switch (o) {
    case MMA8451_PL_PUF: 
      Serial.println("Portrait Up Front");
      break;
    case MMA8451_PL_PUB: 
      Serial.println("Portrait Up Back");
      break;    
    case MMA8451_PL_PDF: 
      Serial.println("Portrait Down Front");
      break;
    case MMA8451_PL_PDB: 
      Serial.println("Portrait Down Back");
      break;
    case MMA8451_PL_LRF: 
      Serial.println("Landscape Right Front");
      break;
    case MMA8451_PL_LRB: 
      Serial.println("Landscape Right Back");
      break;
    case MMA8451_PL_LLF: 
      Serial.println("Landscape Left Front");
      break;
    case MMA8451_PL_LLB: 
      Serial.println("Landscape Left Back");
      break;
    }
  Serial.println();
  delay(500);
  
}
