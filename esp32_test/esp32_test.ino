
const int MOTOR_OUTPUT_PIN = 21;

void setup() {
  // put your setup code here, to run once:
  pinMode(MOTOR_OUTPUT_PIN, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(MOTOR_OUTPUT_PIN, HIGH);   // turn LED on (3.3V)
  delay(2000);                          // delay is in milliseconds; so wait one second
  digitalWrite(MOTOR_OUTPUT_PIN, LOW);    // turn LED off (0V)
  delay(2000);                          // wait for a second
}
