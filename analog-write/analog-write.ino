#define ledPin 11

void setup() {
  // define pin as output for LED
  pinMode(ledPin, OUTPUT);
}

void loop() {

// light LED up (controlled by step count or delay)
  for (int lightValue = 0; lightValue <= 255; lightValue += 50) {
    analogWrite(ledPin, lightValue);
    delay(30);
  }

// dimm LED down (controlled by step count or delay)
  for (int lightValue = 255; lightValue >= 0; lightValue -= 50) {
    analogWrite(ledPin, lightValue);
    delay(30);
  }
}
