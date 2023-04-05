/**************************************************************************
  This is a library for several Adafruit displays based on ST77* drivers.

  Works with the Adafruit 1.8" TFT Breakout w/SD card
    ----> http://www.adafruit.com/products/358
  The 1.8" TFT shield
    ----> https://www.adafruit.com/product/802
  The 1.44" TFT breakout
    ----> https://www.adafruit.com/product/2088
  The 1.14" TFT breakout
  ----> https://www.adafruit.com/product/4383
  The 1.3" TFT breakout
  ----> https://www.adafruit.com/product/4313
  The 1.54" TFT breakout
    ----> https://www.adafruit.com/product/3787
  The 1.69" TFT breakout
    ----> https://www.adafruit.com/product/5206
  The 2.0" TFT breakout
    ----> https://www.adafruit.com/product/4311
  as well as Adafruit raw 1.8" TFT display
    ----> http://www.adafruit.com/products/618

  Check out the links above for our tutorials and wiring diagrams.
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional).

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 **************************************************************************/


/* LCD display setup */

#include <Adafruit_GFX.h>     // Core graphics library
#include <Adafruit_ST7735.h>  // Hardware-specific library for ST7735
#include <Adafruit_ST7789.h>  // Hardware-specific library for ST7789
#include <SPI.h>

// These pins will also work for the 1.8" TFT shield.
#define TFT_CS 10
#define TFT_RST 9  // Or set to -1 and connect to Arduino RESET pin
#define TFT_DC 8

// For 1.44" and 1.8" TFT with ST7735 use:
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);


/* RFID setup */

#include <Wire.h>
#include "SparkFun_Qwiic_Rfid.h"
#define RFID_ADDR 0x7D  // Default I2C address

Qwiic_Rfid myRfid(RFID_ADDR);  // Create the RFID reader object
String currentTag;             // Create variable to hold current tag number
int serialInput;
bool state;


/* Light sensor setup */

// Set analog pins to receive the lightsensor data
#define recyclingStationPin A0
#define landfillStationPin A1
#define compostStationPin A2
#define charmStationPin A3

// Initialize value variables
int recyclingStationValue = 0;
int landfillStationValue = 0;
int compostStationValue = 0;
int charmStationValue = 0;

int lightlevel = 50;


/* Product class and array */

// Setup class to read the products data
class Product {
public:

  // Values of product
  String id;
  String name;
  int station;
  String hint1;
  String hint2;

  // Initializer
  Product(String a, String b, int c, String d, String e) {
    id = a;
    name = b;
    station = c;
    hint1 = d;
    hint2 = e;
  }
};

#define PRODUCT_COUNT 3  // Array lengeth - amount of products we use

// Array of products that are used with id being the RFID-tag number
Product products[] = {
  Product("13205111514375", "Milk", 1, "This item is made from paper and plastic", "This item can be used again when correclty disposed of"),
  Product("132051153205227", "Strawberry Jam", 2, "This item is made from paper and plastic", "This item can be used again"),
  Product("123", "Milk", 1, "This item is made from paper and plastic", "This item can be used again when correclty disposed of"),
};

/* Game logic */
int productIndex = 99;
int currentTries = 0;
int overallPoints = 0;


/* Setup when Arudino starts */
void setup(void) {
  Serial.begin(9600);  // Start serial connection to computer for print etc.

  tft.initR(INITR_BLACKTAB);  // Init ST7735S chip, black tab

  Serial.println(F("Initialized"));


  testtriangles();
  delay(500);


  // PROGRAM START

  startUpScreen();      // Show the logo at start
  setupRFID();          // Initialize the RFID scanner
  setupLightSensors();  // Initialize light sensors

  clearDisplay();  // Clears the statup image from the screen
  // Start with the interactions
  showScanItem();
}


/* Main game logic loop */
void loop() {
  // tft.invertDisplay(true);
  // delay(500);
  // tft.invertDisplay(false);
  // delay(500);
  // Serial.print("loop");
  delay(1000);


  String scannerValue = myRfid.getTag();  // Read scanner data
  Serial.print(scannerValue);

  // Saves active product into the currentTag variable
  if (scannerValue != "000000") {
    currentTag = scannerValue;
    Serial.print("Tag detected: ");
    Serial.println(currentTag);

    productIndex = getProductIndex();  // Search for tag number in array
    Serial.print("This is what I found in the databse: ");
    Serial.print(productIndex);
    // Serial.println(products[productIndex].name);
  }

  readLightSensorValues();  // Read the light sensors at the stations

  // if (currentTag == "13205111514375") {
  //   clearDisplay();
  //   showText("You found the milk");
  //   delay(2000);
  //   clearDisplay();
  // }

  /* Check recycling station */
  if (recyclingStationValue <= lightlevel) {
    // item arrived at recylcing station
    Serial.println("Item found on recycling station");
    // check if it is the correct item by going into the tag and station
    // for (int i = 0; i < PRODUCT_COUNT; i += 1) {  // Go through all items in array to search for the currentTag
    //   if (products[i].id == currentTag) {

    //     Serial.print("This is what I found in the databse: ");
    //     Serial.println(products[i].name);

    //     if (products[i].station == 1) {   // Check if the recycling station is the correct placement
    //       showCorrect(3 - currentTries);  // Shows the points minus the tries the user needed to get there
    //       currentTries = 0;               // Reset the try counter
    //     } else {
    //       handleHints(i);
    //     }
    //   }
    // }
    if (productIndex != 99) {                     // If item was found in array
      if (products[productIndex].station == 1) {  // Check if the landfill station is the correct placement
        showCorrect(3 - currentTries);            // Shows the points minus the tries the user needed to get there
        currentTries = 0;                         // Reset the try counter
      } else {
        handleHints(productIndex);
      }
    }
  }

  if (landfillStationValue <= lightlevel) {
    Serial.println("Item found on landfill station");

    if (productIndex != 99) {                     // If item was found in array
      if (products[productIndex].station == 2) {  // Check if the landfill station is the correct placement
        showCorrect(3 - currentTries);            // Shows the points minus the tries the user needed to get there
        currentTries = 0;                         // Reset the try counter
      } else {
        handleHints(productIndex);
      }
    }
  }

  showScanItem();
}

// Search for the product in the array
int getProductIndex() {
  for (int i = 0; i < PRODUCT_COUNT; i += 1) {  // Go through all items in array to search for the currentTag
    Serial.print("Current ID check: ");
    Serial.println(products[i].id);
    if (products[i].id == currentTag) {
      return i;
    }
  }
  return 99;
}

// Handle the hints when user makes error
void handleHints(int product) {
  Serial.print("The item is not correct here. Number of tries: ");
  Serial.println(currentTries);
  // Check how many times the user made an error
  switch (currentTries) {
    case 0:                      // First try
      showWrong(true, product);  // Show frist hint
      currentTries += 1;
      break;
    case 1:                       // Second try
      showWrong(false, product);  // Show second hint
      currentTries += 1;
      break;
    case 2:                // Third try
      showFinallyWrong();  // Show that the user was finally wrong
      currentTries = 0;
      break;
  }
}

/* Initialize RFID scanner */
void setupRFID() {
  // Begin I-squared-C
  Wire.begin();
  Serial.begin(115200);

  if (myRfid.begin())
    Serial.println("Ready to scan some tags!");
  else
    Serial.println("Could not communicate with Qwiic RFID!");
}

/* Initialize light sensors */
void setupLightSensors() {
  pinMode(recyclingStationPin, INPUT);
  pinMode(landfillStationPin, INPUT);
  pinMode(compostStationPin, INPUT);
  pinMode(charmStationPin, INPUT);
}

/* Show the startup screen logo */
void startUpScreen() {
  // show bitmap logo here
  // delay(4000);
}

/* Read and update sensor values */
void readLightSensorValues() {
  recyclingStationValue = analogRead(recyclingStationPin);
  landfillStationValue = analogRead(landfillStationPin);
  compostStationValue = analogRead(compostStationPin);
  charmStationValue = analogRead(charmStationPin);
  Serial.print("Light sensor values: ");
  Serial.print(recyclingStationValue);
  Serial.print("; ");
  Serial.print(landfillStationValue);
  Serial.print("; ");
  Serial.print(compostStationValue);
  Serial.print("; ");
  Serial.println(charmStationValue);
}

// DISPLAY

void showScanItem() {
  showText("Scan item at home");
}

void showCorrect(int points) {
  clearDisplay();

  // Show correct on screen
  tft.setTextSize(3);
  showText("CORRECT!");
  delay(1000);
  clearDisplay();

  // Show points won on screen
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.println("You got");
  tft.setCursor(10, 30);
  tft.print(points);
  tft.println("/3");
  tft.setCursor(10, 50);
  tft.print("points!");
  delay(3000);
  clearDisplay();

  overallPoints += points;  // Add points to overall point counter

  // Show new total points on screen
  tft.setCursor(10, 10);
  tft.println("Total points:");
  tft.setCursor(10, 50);
  tft.print(overallPoints);
  delay(3000);
  clearDisplay();
}

void showWrong(bool firstTry, int product) {
  clearDisplay();
  tft.setTextSize(3);
  showText("TRY AGAIN!");
  delay(1000);
  clearDisplay();
  tft.setTextSize(2);
  tft.setTextWrap(true);
  String text = "Hint: ";
  Serial.print(products[product].hint2);
  if (firstTry) {
    text += products[product].hint1;
  } else {
    text += products[product].hint2;
  }
  tft.print(text);

  while (recyclingStationValue <= lightlevel || landfillStationValue <= lightlevel) {  // Show hint as long as item is on statiion
    delay(1000);
    readLightSensorValues();
    Serial.println(recyclingStationValue);
  }

  clearDisplay();
}

void showFinallyWrong() {
  clearDisplay();

  tft.setTextSize(3);
  tft.setTextWrap(true);
  showText("Sorry, this is not correct!");
  delay(4000);
  clearDisplay();
}

// HELP FUNCTIONS DISPLAY

/* Clear display by filling it in black */
void clearDisplay() {
  tft.fillScreen(0x0000);
}

/* Shows text on screen in white */
void showText(char *text) {
  tft.setCursor(10, 10);
  tft.setTextSize(2);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextWrap(true);
  tft.print(text);
}



void testlines(uint16_t color) {
  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x = 0; x < tft.width(); x += 6) {
    tft.drawLine(0, 0, x, tft.height() - 1, color);
    delay(0);
  }
  for (int16_t y = 0; y < tft.height(); y += 6) {
    tft.drawLine(0, 0, tft.width() - 1, y, color);
    delay(0);
  }

  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x = 0; x < tft.width(); x += 6) {
    tft.drawLine(tft.width() - 1, 0, x, tft.height() - 1, color);
    delay(0);
  }
  for (int16_t y = 0; y < tft.height(); y += 6) {
    tft.drawLine(tft.width() - 1, 0, 0, y, color);
    delay(0);
  }

  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x = 0; x < tft.width(); x += 6) {
    tft.drawLine(0, tft.height() - 1, x, 0, color);
    delay(0);
  }
  for (int16_t y = 0; y < tft.height(); y += 6) {
    tft.drawLine(0, tft.height() - 1, tft.width() - 1, y, color);
    delay(0);
  }

  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x = 0; x < tft.width(); x += 6) {
    tft.drawLine(tft.width() - 1, tft.height() - 1, x, 0, color);
    delay(0);
  }
  for (int16_t y = 0; y < tft.height(); y += 6) {
    tft.drawLine(tft.width() - 1, tft.height() - 1, 0, y, color);
    delay(0);
  }
}

void testdrawtext(char *text, uint16_t color) {
  tft.setCursor(0, 0);
  tft.setTextColor(color);
  tft.setTextWrap(true);
  tft.print(text);
}

void testfastlines(uint16_t color1, uint16_t color2) {
  tft.fillScreen(ST77XX_BLACK);
  for (int16_t y = 0; y < tft.height(); y += 5) {
    tft.drawFastHLine(0, y, tft.width(), color1);
  }
  for (int16_t x = 0; x < tft.width(); x += 5) {
    tft.drawFastVLine(x, 0, tft.height(), color2);
  }
}

void testdrawrects(uint16_t color) {
  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x = 0; x < tft.width(); x += 6) {
    tft.drawRect(tft.width() / 2 - x / 2, tft.height() / 2 - x / 2, x, x, color);
  }
}

void testfillrects(uint16_t color1, uint16_t color2) {
  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x = tft.width() - 1; x > 6; x -= 6) {
    tft.fillRect(tft.width() / 2 - x / 2, tft.height() / 2 - x / 2, x, x, color1);
    tft.drawRect(tft.width() / 2 - x / 2, tft.height() / 2 - x / 2, x, x, color2);
  }
}

void testfillcircles(uint8_t radius, uint16_t color) {
  for (int16_t x = radius; x < tft.width(); x += radius * 2) {
    for (int16_t y = radius; y < tft.height(); y += radius * 2) {
      tft.fillCircle(x, y, radius, color);
    }
  }
}

void testdrawcircles(uint8_t radius, uint16_t color) {
  for (int16_t x = 0; x < tft.width() + radius; x += radius * 2) {
    for (int16_t y = 0; y < tft.height() + radius; y += radius * 2) {
      tft.drawCircle(x, y, radius, color);
    }
  }
}

void testtriangles() {
  tft.fillScreen(ST77XX_BLACK);
  uint16_t color = 0xF800;
  int t;
  int w = tft.width() / 2;
  int x = tft.height() - 1;
  int y = 0;
  int z = tft.width();
  for (t = 0; t <= 15; t++) {
    tft.drawTriangle(w, y, y, x, z, x, color);
    x -= 4;
    y += 4;
    z -= 4;
    color += 100;
  }
}

void testroundrects() {
  tft.fillScreen(ST77XX_BLACK);
  uint16_t color = 100;
  int i;
  int t;
  for (t = 0; t <= 4; t += 1) {
    int x = 0;
    int y = 0;
    int w = tft.width() - 2;
    int h = tft.height() - 2;
    for (i = 0; i <= 16; i += 1) {
      tft.drawRoundRect(x, y, w, h, 5, color);
      x += 2;
      y += 3;
      w -= 4;
      h -= 6;
      color += 1100;
    }
    color += 100;
  }
}

void tftPrintTest() {
  tft.setTextWrap(false);
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(0, 30);
  tft.setTextColor(ST77XX_RED);
  tft.setTextSize(1);
  tft.println("Hello World!");
  tft.setTextColor(ST77XX_YELLOW);
  tft.setTextSize(2);
  tft.println("Hello World!");
  tft.setTextColor(ST77XX_GREEN);
  tft.setTextSize(3);
  tft.println("Hello World!");
  tft.setTextColor(ST77XX_BLUE);
  tft.setTextSize(4);
  tft.print(1234.567);
  delay(1500);
  tft.setCursor(0, 0);
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(0);
  tft.println("Hello World!");
  tft.setTextSize(1);
  tft.setTextColor(ST77XX_GREEN);
  // tft.print(p, 6);
  tft.println(" Want pi?");
  tft.println(" ");
  tft.print(8675309, HEX);  // print 8,675,309 out in HEX!
  tft.println(" Print HEX!");
  tft.println(" ");
  tft.setTextColor(ST77XX_WHITE);
  tft.println("Sketch has been");
  tft.println("running for: ");
  tft.setTextColor(ST77XX_MAGENTA);
  tft.print(millis() / 1000);
  tft.setTextColor(ST77XX_WHITE);
  tft.print(" seconds.");
}

void mediabuttons() {
  // play
  tft.fillScreen(ST77XX_BLACK);
  tft.fillRoundRect(25, 10, 78, 60, 8, ST77XX_WHITE);
  tft.fillTriangle(42, 20, 42, 60, 90, 40, ST77XX_RED);
  delay(500);
  // pause
  tft.fillRoundRect(25, 90, 78, 60, 8, ST77XX_WHITE);
  tft.fillRoundRect(39, 98, 20, 45, 5, ST77XX_GREEN);
  tft.fillRoundRect(69, 98, 20, 45, 5, ST77XX_GREEN);
  delay(500);
  // play color
  tft.fillTriangle(42, 20, 42, 60, 90, 40, ST77XX_BLUE);
  delay(50);
  // pause color
  tft.fillRoundRect(39, 98, 20, 45, 5, ST77XX_RED);
  tft.fillRoundRect(69, 98, 20, 45, 5, ST77XX_RED);
  // play color
  tft.fillTriangle(42, 20, 42, 60, 90, 40, ST77XX_GREEN);
}
