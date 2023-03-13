#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library.
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET -1        // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3D  ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define LOGO_HEIGHT 16
#define LOGO_WIDTH 16

static const unsigned char PROGMEM logo_bmp[] = { 0b00000000, 0b11000000,
                                                  0b00000001, 0b11000000,
                                                  0b00000001, 0b11000000,
                                                  0b00000011, 0b11100000,
                                                  0b11110011, 0b11100000,
                                                  0b11111110, 0b11111000,
                                                  0b01111110, 0b11111111,
                                                  0b00110011, 0b10011111,
                                                  0b00011111, 0b11111100,
                                                  0b00001101, 0b01110000,
                                                  0b00011011, 0b10100000,
                                                  0b00111111, 0b11100000,
                                                  0b00111111, 0b11110000,
                                                  0b01111100, 0b11110000,
                                                  0b01110000, 0b01110000,
                                                  0b00000000, 0b00110000 };



// [TODO] Define your global variables here 


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Don't proceed, loop forever
  }

  // [TODO] Configure pinMode() here



  delay(2000);  // Pause for 2 seconds

  // Calls the first screen. Everytime you reset or switch 
  // on the device this function is called.
  first();
  delay(4000);  // Pause for 4 seconds
  
  // [TODO] Call function here to display Menu screen with selection bar sitting on "1. Start"


  delay(100);
}

void loop() {
  // put your main code here, to run repeatedly:

  // [TODO] Read "Down" Button's current state.
  // If pressed -> Perform relevant action.
  // Hint: Go through Lecture 3 slides. We have discussed this in the "Tasks" section.  

  

  // [TODO] Read "Select" Button's state.
  // If pressed -> Perform relevant action.
  
  delay(100);
}


// Clears Display -> Sets text size -> sets text color -> set cursor position -> Print -> call display()
void first() {
  display.clearDisplay();
  display.setTextSize(3);
  display.setTextColor(WHITE);  // Note: Color can be BLACK or WHITE. This is a hint for creating your "Menu" screen.
  display.setCursor(15, 20);
  display.println("Hello");
  display.display();
}