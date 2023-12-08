#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_MMA8451.h>

#define RIGHT_LED_PIN 17
#define LEFT_LED_PIN 21
#define LED_COUNT 10

//Adafruit_NeoPixel leftSleeveStrip(LED_COUNT, LEFT_LED_PIN, NEO_GRB + NEO_KHZ800);
//Adafruit_NeoPixel rightSleeveStrip(LED_COUNT, RIGHT_LED_PIN, NEO_GRB + NEO_KHZ800);

//Adafruit_MMA8451 rightSleeveMMA = Adafruit_MMA8451();
Adafruit_MMA8451 mma = Adafruit_MMA8451();

void setup(void) {
  // put your setup code here, to run once:

//  rightSleeveStrip.begin(); // Initialize Right NeoPixel LED strip
//  rightSleeveStrip.show(); // Initialize all pixels to 'off'
//  rightSleeveStrip.setBrightness(64);  // range: 0 to 255
//  
//  leftSleeveStrip.begin(); // Initialize Left NeoPixel LED strip
//  leftSleeveStrip.show(); // Initialize all pixels to 'off'
//  leftSleeveStrip.setBrightness(64);  // range: 0 to 255

  Serial.begin(9600);
//  Serial.println("Adafruit MMA8451 test!");
//  
//  // Initialize Right Sleeve MMA8451
//  if (! rightSleeveMMA.begin(0x1C)) { // Unique I2C address for the right sleeve accelerometer, , 0x1C requires A pin to GND
//    Serial.println("Couldn't start right sleeve MMA8451");
//    while (1);
//  }
//  Serial.println("Right sleeve MMA8451 found!");
//
  // Initialize Left Sleeve MMA8451
  if (! mma.begin(0x1D)) { // Unique I2C address for the left sleeve accelerometer
    Serial.println("Couldn't start left sleeve MMA8451");
    while (1);
  }
  Serial.println("Left sleeve MMA8451 found!");
  
//  rightSleeveMMA.setRange(MMA8451_RANGE_2_G);
  mma.setRange(MMA8451_RANGE_2_G);
}

void loop() {
  // put your main code here, to run repeatedly:

  mma.read();
  sensors_event_t event; 
  mma.getEvent(&event);

  // Printing the accelerometer data for the specific sleeve
  //Serial.print(sleeveName);
  Serial.print(" X:\t"); Serial.print(event.acceleration.x); Serial.print("\t");
  Serial.print("Y:\t"); Serial.print(event.acceleration.y); Serial.print("\t");
  Serial.print("Z:\t"); Serial.print(event.acceleration.z); Serial.println(" m/s^2");

  delay(2000);
  //  for (int i = 0; i < LED_COUNT; i++) {
//      leftSleeveStrip.setPixelColor(i, 225, 0, 0); // Red color
//      rightSleeveStrip.setPixelColor(i, 225, 0, 0); // Red color
//    }
//    leftSleeveStrip.show();  // Turn on LEDs
//    rightSleeveStrip.show();
//  delay(3000);
//  for (int i = 0; i < LED_COUNT; i++) {
//      leftSleeveStrip.setPixelColor(i, 0, 0, 0); // Off
//      rightSleeveStrip.setPixelColor(i, 0, 0, 0); // Off
//    }
//    leftSleeveStrip.show();  // Turn on LEDs
//    rightSleeveStrip.show();
//  delay(3000);
}
