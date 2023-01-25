void setup() {
  // put your setup code here, to run once:
  pinMode(10, OUTPUT);
}

void loop() {
  digitalWrite(10, HIGH); // on
  delay(2000);
  digitalWrite(10, LOW); // off
  delay(4000);
}
