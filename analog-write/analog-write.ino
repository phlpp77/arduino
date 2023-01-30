#define ledPin 11

void setup() {
  // put your setup code here, to run once:
pinMode(ledPin, OUTPUT);
}

void loop() {
  analogWrite(ledPin, 255);
  // put your main code here, to run repeatedly:

}
