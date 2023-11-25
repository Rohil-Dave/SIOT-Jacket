
const int LED_OUTPUT_PIN = 21;

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_OUTPUT_PIN, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(LED_OUTPUT_PIN, HIGH);   // turn LED on (3.3V)
  delay(1000);                          // delay is in milliseconds; so wait one second
  digitalWrite(LED_OUTPUT_PIN, LOW);    // turn LED off (0V)
  delay(1000);                          // wait for a second
}
