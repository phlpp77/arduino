#define lightSensor A0
int lightValue = 0;

void setup() {
  pinMode(lightSensor, INPUT);
  Serial.begin(9600);
}

void loop() {
  lightValue = analogRead(lightSensor);
  Serial.print("Light value: ");
  Serial.println(lightValue);
  // put your main code here, to run repeatedly:
  delay(2);

}
