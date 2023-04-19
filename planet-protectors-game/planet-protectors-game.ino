/**************************************************************************
  This is a library for several Adafruit displays based on ST77* drivers.

  Works with the Adafruit 1.8" TFT Breakout w/SD card
    ----> http://www.adafruit.com/products/358
  The 1.8" TFT shield
    ----> https://www.adafruit.com/product/802
  The 1.44" TFT breakout
    ----> https://www.adafruit.com/product/2088

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
#define TFT_CS 45   //10 //45
#define TFT_RST -1  //9  //43 // Or set to -1 and connect to Arduino RESET pin
#define TFT_DC 41   //8 // 41

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
#define recyclingStationPin A3
#define landfillStationPin A2
#define compostStationPin A1
#define charmStationPin A0

// Initialize value variables
int recyclingStationValue = 0;
int landfillStationValue = 0;
int compostStationValue = 0;
int charmStationValue = 0;

int lightlevel = 20;

/* LED setup */
#define redLED 7
#define greenLED 6


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




#define PRODUCT_COUNT 14  // Array lengeth - amount of products we use

// Array of products that are used with id being the RFID-tag number
Product products[PRODUCT_COUNT] = {
  Product("4224317959177", "Milk", 1, "This item is made \nfrom paper\nand \nplastic", "This item can be \nused again\nwhen cor-\nrectly \ndisposed"),
  Product("13205111514375", "Cereal", 1, "This item is made from cardboard", "This item can be \nused again\nwhen cor-\nrectly \ndisposed"),
  Product("13205115110472", "Straw- \n berry Jam", 1, "This item is made from paper and \n plastic", "This item can be \nused again\nwhen cor-\nrectly \ndisposed"),
  Product("42243114578", "Peanut \n Butter", 1, "This item is made from plastic", "This item can be \nused again\nwhen cor-\nrectly \ndisposed"),
  Product("5322812843128", "Battery", 4, "This item contains \nhazardus \nwaste", "This item is hard to\nrecycle"),
  Product("422431138630", "Coffee \n Cup", 1, "This item is made from paper and plastic", "This item can be \nused again\nwhen cor-\nrectly \ndisposed"),
  Product("42244133105210", "Soda can", 1, "This item is made from aluminum", "This item can be \nused again\nwhen cor-\nrectly \ndisposed"),
  Product("5322815818413", "Granola \n Wrapper", 2, "This item is made from plastic", "This item can NOT be used again when correclty disposed"),
  Product("421210235163139", "Apple", 3, "This item is grown\nfrom the\nground", "This item can decompose easily\n(when put\nback on the ground)"),
  Product("5322816722282", "Banana", 3, "This item is grown\nfrom the\nground", "This item can decompose easily\n(when put\nback on the ground)"),
  Product("132051129186140", "Plastic \n Fork", 2, "This item is made from plastic", "This item can NOT be used again when correclty disposed"),
  Product("1320511531432", "Light \n Bulb", 4, "This item contains \nhazardus \nwaste", "This item is hard to\nrecycle"),
  Product("4224315212932", "Old \n Phone", 4, "This item contains \nhazardus \nwaste", "This item is hard to\nrecycle"),
  Product("5322813062151", "Paint \n Bucket", 4, "This item contains \n hazardus \n waste", "This item is hard to\nrecycle"),
};

/* Game logic variables */
int productIndex = 99;
int currentTries = 0;
int overallPoints = 0;


/* Setup when Arudino starts */
void setup(void) {
  Serial.begin(9600);            // Start serial connection to computer for print etc.
  tft.initR(INITR_144GREENTAB);  // Init ST7735S chip - screen
  tft.setRotation(1);            // Rotate the screen by 90º to fit the mountain

  // Start animation
  startUpScreen();  // Show the logo at start

  // Setup of sensors and lights
  setupRFID();          // Initialize the RFID scanner
  setupLightSensors();  // Initialize light sensors
  setupLEDs();          // Initialize green and red LED

  // Start with game
  clearDisplay();  // Clears the startup image from the screen
  showScanItem();  // Start with the interactions
}


/* Main game logic loop */
void loop() {

  String scannerValue = myRfid.getTag();  // Read scanner data

  delay(250);  // Delay needed for RFID Scanner

  // Saves active product into the currentTag variable
  if (scannerValue != "000000") {  // Check if tag is on scanner
    currentTag = scannerValue;     // Save value to current Tag
    Serial.print("Tag detected: ");
    Serial.println(currentTag);

    productIndex = getProductIndex();  // Search for tag number in array
    if (productIndex != 99) {          // When the product was found in the database
      Serial.print("This is what I found in the databse: ");
      Serial.println(products[productIndex].name);
      clearDisplay();

      // Print item name on screen
      tft.setCursor(10, 10);
      tft.print("You found");
      tft.setCursor(10, 30);
      tft.print(products[productIndex].name);
      tft.setCursor(10, 70);
      tft.print("Please,");
      tft.setCursor(10, 90);
      tft.print("dispose!");
      delay(2000);
    } else {
      Serial.println("No matching item was found in the database");
    }
  }

  readLightSensorValues();  // Read the light sensors at the stations

  // Checking stations
  if (recyclingStationValue <= lightlevel) {  // Check for item at recycling station
    Serial.println("Item found on recycling station");
    checkCorrect(productIndex, 1);  // Check if the item is correctly disposed at recycling station
  }

  if (landfillStationValue <= lightlevel) {  // Check for item at landfill station
    Serial.println("Item found on landfill station");
    checkCorrect(productIndex, 2);  // Check if the item is correctly disposed at landfill station
  }

  if (compostStationValue <= lightlevel) {  // Check for item at compost station
    Serial.println("Item found on compost station");
    checkCorrect(productIndex, 3);  // Check if the item is correctly disposed at compost station
  }

  if (charmStationValue <= lightlevel) {  // Check for item at charm station
    Serial.println("Item found on charm station");
    checkCorrect(productIndex, 4);  // Check if the item is correctly disposed at charm station
  }
}


// --- GAME LOGIC/RECURRING FUNCTIONS ---

/* Search for the product in the array */
int getProductIndex() {
  for (int i = 0; i < PRODUCT_COUNT; i += 1) {  // Go through all items in array to search for the currentTag
    delay(10);
    Serial.print("Current ID check: ");
    Serial.println(products[i].id);
    if (products[i].id == currentTag) {  // Check if element is the current tag
      return i;                          // Return the product index that was found for the current tag
    }
  }
  return 99;  // If current tag was not found in array return 99
}

/* Check if something was correctly placed */
void checkCorrect(int product, int station) {
  if (product != 99) {                           // Check if item was found in array (maybe not needed anymore because checked in code above as well)
    if (products[product].station == station) {  // Check if the station is correct
      showCorrect(3 - currentTries);             // Shows the points minus the tries the user needed to get there
      currentTries = 0;                          // Reset the try counter
    } else {                                     // Show hints if not correct
      handleHints(product);                      // Handles wrong answers and gives hints
    }
  }
}

/* Handle the hints when user makes error */
void handleHints(int product) {
  Serial.print("The item is not correct here. Number of tries: ");
  Serial.println(currentTries);
  // Check how many times the user made an error
  switch (currentTries) {
    case 0:                      // First try
      showWrong(true, product);  // Show frist hint
      currentTries += 1;         // Add the try to the counter
      break;
    case 1:                       // Second try
      showWrong(false, product);  // Show second hint
      currentTries += 1;          // Add the try to the counter
      break;
    case 2:                // Third try
      showFinallyWrong();  // Show that the user was finally wrong
      currentTries = 0;    // Reset tries because new item needs to be scanned
      break;
  }
}

/* Read and update sensor values */
void readLightSensorValues() {
  recyclingStationValue = analogRead(recyclingStationPin);
  landfillStationValue = analogRead(landfillStationPin);
  compostStationValue = analogRead(compostStationPin);
  charmStationValue = analogRead(charmStationPin);

  // Just printing the values for debugging
  Serial.print("Light sensor values: ");
  Serial.print(recyclingStationValue);
  Serial.print("; ");
  Serial.print(landfillStationValue);
  Serial.print("; ");
  Serial.print(compostStationValue);
  Serial.print("; ");
  Serial.println(charmStationValue);
}


// --- SETUP FUNCTIONS ---

/* Initialize RFID scanner */
void setupRFID() {
  // Begin I-squared-C
  Wire.begin();
  Serial.begin(115200);

  if (myRfid.begin()) // Start the RFID scanner 
    Serial.println("RFID scanner started correctly");
  else
    Serial.println("Could not communicate with Qwiic RFID!");
}

/* Initialize light sensors */
void setupLightSensors() {
  pinMode(recyclingStationPin, INPUT);
  pinMode(landfillStationPin, INPUT);
  pinMode(compostStationPin, INPUT);
  pinMode(charmStationPin, INPUT);
  
  // Calculating the average light level - not needed anymore
  // readLightSensorValues();
  // int lightLevelSum = recyclingStationValue + landfillStationValue + compostStationValue + charmStationValue;
  // int averageLightLevel = lightLevelSum / 4;
  // lightlevel = averageLightLevel - 50;
  Serial.print("Light level adapted to environment: ");
  Serial.println(lightlevel);
}

/* Initialize LEDs */
void setupLEDs() {
  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
}

/* Show the startup screen logo */
void startUpScreen() {
  startUpAnimation();  // Startup animation
  delay(500);
  tft.setCursor(10, 10);
  tft.setTextSize(3);
  tft.setTextColor(0x001F);
  tft.print("PLANET");
  tft.setCursor(10, 40);
  tft.setTextColor(0xF800);
  tft.print("PROTEC");
  tft.setCursor(10, 65);
  tft.print("TORS");

  delay(3000);
}


// --- SHOW CONTENT FUNCTIONS ---

/* Show text to scan item */
void showScanItem() {
  clearDisplay();
  showText("Scan item \n at home");
}

/* Show text to scan new item */
void showScanNewItem() {
  clearDisplay();
  showText("Scan new \n item at \n home");
}

/* Show that decision was correct */
void showCorrect(int points) {
  clearDisplay();

  // Show correct on screen
  tft.setTextSize(3);
  digitalWrite(greenLED, HIGH);
  showText("CORRECT!");
  delay(3000);
  digitalWrite(greenLED, LOW);
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
  currentTag = "000000";    // Reset the current tag
  productIndex = 99;        // Reset the product index

  // Show new total points on screen
  tft.setCursor(10, 10);
  tft.print("Overall");
  tft.setCursor(10, 30);
  tft.print("points:");
  tft.print(overallPoints);

  while (recyclingStationValue <= lightlevel || landfillStationValue <= lightlevel || compostStationValue <= lightlevel || charmStationValue <= lightlevel) {  // Show points as long as item is placed
    delay(1000);
    readLightSensorValues();
  }

  delay(1000);
  clearDisplay();
  showScanNewItem();
}

/* Show that decision was wrong */
void showWrong(bool firstTry, int product) {
  clearDisplay();
  tft.setTextSize(3);
  digitalWrite(redLED, HIGH);
  showText("TRY AGAIN");
  delay(3000);
  digitalWrite(redLED, LOW);
  clearDisplay();

  tft.setTextSize(2);
  String text;
  Serial.print(products[product].hint2);
  if (firstTry) {
    text = products[product].hint1;
  } else {
    text = products[product].hint2;
  }
  Serial.print("Print hint: ");
  Serial.println(text);
  tft.setCursor(0, 5);
  tft.println("Hint: ");
  tft.print(text);

  while (recyclingStationValue <= lightlevel || landfillStationValue <= lightlevel || compostStationValue <= lightlevel || charmStationValue <= lightlevel) {  // Show hint as long as item is on statiion
    delay(1000);
    readLightSensorValues();
  }

  // Write: Try another station!
  delay(1000);
  clearDisplay();
  tft.setCursor(10, 10);
  tft.print("Try");
  tft.setCursor(10, 30);
  tft.print("another");
  tft.setCursor(10, 50);
  tft.print("station!");
}

/* Show that last try was still wrong */
void showFinallyWrong() {
  clearDisplay();

  // Write: Sorry! This is not correct.
  digitalWrite(redLED, HIGH);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.print("Sorry!");
  tft.setCursor(10, 30);
  tft.print("This is");
  tft.setCursor(10, 50);
  tft.print("not");
  tft.setCursor(10, 70);
  tft.print("correct.");
  tft.setCursor(10, 90);
  tft.print("Try new");
  tft.setCursor(10, 110);
  tft.print("item!");

  currentTag = "000000";  // Reset the current tag
  productIndex = 99;      // Reset the product index

  while (recyclingStationValue <= lightlevel || landfillStationValue <= lightlevel || compostStationValue <= lightlevel || charmStationValue <= lightlevel) {  // Show message as long as item is on statiion
    delay(1000);
    readLightSensorValues();
  }
  delay(1000);
  digitalWrite(redLED, LOW);
  clearDisplay();
  showScanNewItem();
}


// --- DISPLAY HELP FUNCTIONS ---

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

/* Show the triangle startup animation */
void startUpAnimation() {
  for (int i = 0; i <= 3; i++) {
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
      delay(10);
    }
    clearDisplay();
  }
}