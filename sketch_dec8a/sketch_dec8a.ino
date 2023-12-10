// Define pins for each haptic vibe motor
#define RIGHT_VIBE_PIN 15 // 15
#define LEFT_VIBE_PIN 27  // 27

void setup() {
  // put your setup code here, to run once:
  pinMode(RIGHT_VIBE_PIN, OUTPUT);
  pinMode(LEFT_VIBE_PIN, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(RIGHT_VIBE_PIN, HIGH);
  digitalWrite(LEFT_VIBE_PIN, HIGH);
  delay(2000);
  digitalWrite(RIGHT_VIBE_PIN, LOW);
  digitalWrite(LEFT_VIBE_PIN, LOW);
}
