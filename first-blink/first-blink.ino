#define ledPin 12
#define switchPin 10
int switchState = 0;
bool safeState = false;

void setup() {
  // put your setup code here, to run once:
  pinMode(ledPin, OUTPUT);
  pinMode(switchPin, INPUT);
}

void loop() {
  switchState = digitalRead(switchPin);
  if (switchState == 1) {
    safeState = !safeState;
  }
  if (safeState == 1) {
    digitalWrite(ledPin, HIGH);
  } else {
    digitalWrite(ledPin, HIGH);  // on
    delay(500);
    digitalWrite(ledPin, LOW);  // off
    delay(1000);
  }
}
