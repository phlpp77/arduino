// Define all LED pins
#define greenLedPin 13   // Green LED is pin 13
#define yellowLedPin 12  // Yellow LED is pin 12
#define redLedPin 11     // Red LED is pin 11

// Define button pin
#define buttonPin 7  // Define the button to be pin 7

// Define variables for if statements
int buttonState = 0;

void setup() {
  // Set pinmodes of LED pins to output
  pinMode(greenLedPin, OUTPUT);
  pinMode(yellowLedPin, OUTPUT);
  pinMode(redLedPin, OUTPUT);

  // Set pinmode of button pin to input
  pinMode(buttonPin, INPUT);
}

void loop() {
  buttonState = digitalRead(buttonPin);
  if (buttonState == 1) {
    for (int i = 0; i <= 2; i++) {
      digitalWrite(greenLedPin, HIGH);
      delay(500);  // Wait for 1000 millisecond(s)
      digitalWrite(greenLedPin, LOW);
      delay(500);  // Wait for 1000 millisecond(s)
    }
    // Pause for 3s (3000ms)
    delay(3000);

    // Flash LEDs from right to left
    digitalWrite(redLedPin, HIGH);  // Red on
    delay(500);
    digitalWrite(redLedPin, LOW);  // Red off
    delay(500);
    // Swtich to 2nd LED
    digitalWrite(yellowLedPin, HIGH);  // Yellow on
    delay(500);
    digitalWrite(yellowLedPin, LOW);  // Yellow off
    delay(500);
    // Switch to 3rd LED
    digitalWrite(greenLedPin, HIGH);  // Green on
    delay(500);
    digitalWrite(greenLedPin, LOW);  // Green off
    delay(500);

    // Pause 5s (5000ms)
    delay(5000);

    // Flash LEDs from left to right
    digitalWrite(greenLedPin, HIGH);  // Green on
    delay(500);
    digitalWrite(greenLedPin, LOW);  // Green off
    delay(500);
    // Swtich to 2nd LED
    digitalWrite(yellowLedPin, HIGH);  // Yellow on
    delay(500);
    digitalWrite(yellowLedPin, LOW);  // Yellow off
    delay(500);
    // Switch to 3rd LED
    digitalWrite(redLedPin, HIGH);  // Red on
    delay(500);
    digitalWrite(redLedPin, LOW);  // Red off
    delay(500);
    
  }
}