/**************************************************************************
 This is an example for our Monochrome OLEDs based on SSD1306 drivers

 Pick one up today in the adafruit shop!
 ------> http://www.adafruit.com/category/63_98

 This example is for a 128x64 pixel display using I2C to communicate
 3 pins are required to interface (two I2C and one reset).

 Adafruit invests time and resources providing this open
 source code, please support Adafruit and open-source
 hardware by purchasing products from Adafruit!

 Written by Limor Fried/Ladyada for Adafruit Industries,
 with contributions from the open source community.
 BSD license, check license.txt for more information
 All text above, and the splash screen below must be
 included in any redistribution.
 **************************************************************************/

#include <SPI.h>
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
#define OLED_RESET 4         // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3D  ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define NUMFLAKES 10  // Number of snowflakes in the animation example

#define LOGO_HEIGHT 16
#define LOGO_WIDTH 16

// Setup for buttons
#define BUTTON_PIN_YELLOW 3
#define BUTTON_PIN_RED 2
int yellowButtonState = 0;
int redButtonState = 0;

// Current menu selection
int menuSelection = 0;

// Setup for LED
#define LED_PIN 9

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

void setup() {
  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Don't proceed, loop forever
  }

  // Initialize button pins
  pinMode(BUTTON_PIN_YELLOW, INPUT);
  pinMode(BUTTON_PIN_RED, INPUT);

  // Initialize LED pin
  pinMode(LED_PIN, OUTPUT);

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000);  // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();

  showHelloText();  // Show startup text

  showMenu();                                        // Show the menu
  display.fillRect(0, 0, 128, 16, SSD1306_INVERSE);  // Highlight first option
  display.display();                                 // Show the highlight box

  // testanimate(logo_bmp, LOGO_WIDTH, LOGO_HEIGHT);  // Animate bitmaps
}

void loop() {

  yellowButtonState = digitalRead(BUTTON_PIN_YELLOW);
  redButtonState = digitalRead(BUTTON_PIN_RED);

  delay(100);

  // Check if down-button is pressed and menu is switched
  if (yellowButtonState == 1) {
    menuSelection += 1;
    if (menuSelection == 3) {
      menuSelection = 0;  // if last position (2) is reached go back to first
    }

    display.clearDisplay();
    showMenu();
    switch (menuSelection) {
      case 0:  // First option highlighted
        display.fillRect(0, 0, 128, 16, SSD1306_INVERSE);
        break;
      case 1:  // Second option highlighted
        display.fillRect(0, 16, 128, 17, SSD1306_INVERSE);
        break;
      case 2:  // Third option highlighted
        display.fillRect(0, 33, 128, 17, SSD1306_INVERSE);
        break;
    }
    display.display();
  }

  // Show each screen if Select-button is pressed
  if (redButtonState == 1) {
    switch (menuSelection) {
      case 0:  // Open first option
        showFirstScreen();
        blinkLED();
        break;
      case 1:  // Open second option
        showSecondScreen();
        break;
      case 2:  // Open third option
        display.clearDisplay();
        display.display();
        break;
    }
  }
}

void showHelloText(void) {
  display.clearDisplay();
  display.setTextSize(3);  // Draw 3X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 0);
  display.println("Hello");
  display.display();  // Show text on display
  delay(1000);        // TODO change back to 4000
}

void showMenu(void) {
  display.clearDisplay();
  display.setTextSize(2);  // Draw 3X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 0);
  display.println("1. Start");
  display.setCursor(10, 17);
  display.println("2. Stats");
  display.setCursor(10, 34);
  display.println("3. End");
  display.display();  // Show text on display
}

void showFirstScreen(void) {
  display.clearDisplay();
  display.drawBitmap(
    (display.width() - LOGO_WIDTH) / 2,
    (display.height() - LOGO_HEIGHT) / 2,
    logo_bmp, LOGO_WIDTH, LOGO_HEIGHT, 1);
  display.display();
}


void blinkLED(void) {
  for (int i = 0; i <= 2; i += 1) {
    digitalWrite(LED_PIN, HIGH);  // On
    delay(500);
    digitalWrite(LED_PIN, LOW);  // Off
    delay(500);
  }
}





void testdrawstyles(void) {
  display.clearDisplay();

  display.setTextSize(1);               // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);  // Draw white text
  display.setCursor(0, 0);              // Start at top-left corner
  display.println(F("Hello, world!"));

  display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);  // Draw 'inverse' text
  display.println(3.141592);

  display.setTextSize(2);  // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.print(F("0x"));
  display.println(0xDEADBEEF, HEX);

  display.display();
  delay(2000);
}

void showSecondScreen(void) {
  display.clearDisplay();

  display.setTextSize(2);  // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 0);
  display.println(F("Sidra"));
  display.setCursor(10, 17);
  display.println(F("&"));
  display.setCursor(10, 34);
  display.println(F("Philipp"));
  display.display();  // Show initial text
  delay(100);

  // Scroll in right and left, pausing in-between:
  display.startscrollright(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);
  display.startscrollleft(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);
}

void testdrawbitmap(void) {
  display.clearDisplay();

  display.drawBitmap(
    (display.width() - LOGO_WIDTH) / 2,
    (display.height() - LOGO_HEIGHT) / 2,
    logo_bmp, LOGO_WIDTH, LOGO_HEIGHT, 1);
  display.display();
  delay(1000);
}

#define XPOS 0  // Indexes into the 'icons' array in function below
#define YPOS 1
#define DELTAY 2

void testanimate(const uint8_t *bitmap, uint8_t w, uint8_t h) {
  int8_t f, icons[NUMFLAKES][3];

  // Initialize 'snowflake' positions
  for (f = 0; f < NUMFLAKES; f++) {
    icons[f][XPOS] = random(1 - LOGO_WIDTH, display.width());
    icons[f][YPOS] = -LOGO_HEIGHT;
    icons[f][DELTAY] = random(1, 6);
    Serial.print(F("x: "));
    Serial.print(icons[f][XPOS], DEC);
    Serial.print(F(" y: "));
    Serial.print(icons[f][YPOS], DEC);
    Serial.print(F(" dy: "));
    Serial.println(icons[f][DELTAY], DEC);
  }

  for (;;) {                 // Loop forever...
    display.clearDisplay();  // Clear the display buffer

    // Draw each snowflake:
    for (f = 0; f < NUMFLAKES; f++) {
      display.drawBitmap(icons[f][XPOS], icons[f][YPOS], bitmap, w, h, SSD1306_WHITE);
    }

    display.display();  // Show the display buffer on the screen
    delay(200);         // Pause for 1/10 second

    // Then update coordinates of each flake...
    for (f = 0; f < NUMFLAKES; f++) {
      icons[f][YPOS] += icons[f][DELTAY];
      // If snowflake is off the bottom of the screen...
      if (icons[f][YPOS] >= display.height()) {
        // Reinitialize to a random position, just off the top
        icons[f][XPOS] = random(1 - LOGO_WIDTH, display.width());
        icons[f][YPOS] = -LOGO_HEIGHT;
        icons[f][DELTAY] = random(1, 6);
      }
    }
  }
}
