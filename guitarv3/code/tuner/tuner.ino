// Thomas McDonagh 2021
// Written for AtMega328P at 8MHz

#include <AceButton.h> // Button handling
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Fonts/FreeSerifBold9pt7b.h> // Font
#include <Fonts/FreeSerifBold18pt7b.h>
#include <Adafruit_SSD1306.h>

using namespace ace_button;

const int buttonPin = 6;
const int oledPowerPin = 2;

AceButton button(buttonPin);

void handleEvent(AceButton*, uint8_t, uint8_t);

Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);

// Notes were set with trial and error
// If I put the real Hz values it doesn't tune correctly
const double noteDropD = 75.12;
const double noteLowE = 83.98;
const double noteA = 112.46;
const double noteD = 150.24;
const double noteG = 200.32;
const double noteB = 253.04;
const double noteHighE = 337.39;

const double deMidPoint = (noteDropD + noteLowE) / 2;
const double eaMidPoint = (noteLowE + noteA) / 2;
const double adMidPoint = (noteA + noteD) / 2;
const double dgMidPoint = (noteD + noteG) / 2;
const double gbMidPoint = (noteG + noteB) / 2;
const double beMidPoint = (noteB + noteHighE) / 2;

const double tolerance = 0.50; // Hz amount that note can be off for it to still register as correct note

boolean showHz = false;
boolean screenOn = false;

int farHeight, midHeight, nearHeight;

//clipping indicator variables
boolean clipping = 0;

//data storage variables
byte newData = 0;
byte prevData = 0;
unsigned int time = 0;//keeps time and sends vales to store in timer[] occasionally
int timer[10];//storage for timing of events
int slope[10];//storage for slope of events
unsigned int totalTimer;//used to calculate period
unsigned int period;//storage for period of wave
byte index = 0;//current storage index
float frequency;//storage for frequency calculations
int maxSlope = 0;//used to calculate max slope as trigger point
int newSlope;//storage for incoming slope data

//variables for decided whether you have a match
byte noMatch = 0;//counts how many non-matches you've received to reset variables if it's been too long
byte slopeTol = 3;//slope tolerance- adjust this if you need
int timerTol = 10;//timer tolerance- adjust this if you need

//variables for amp detection
unsigned int ampTimer = 0;
byte maxAmp = 0;
byte checkMaxAmp;
byte ampThreshold = 30;//raise if you have a very noisy signal

void setup() {

  //Serial.begin(9600);



  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(oledPowerPin, OUTPUT);
  digitalWrite(oledPowerPin, HIGH);
  button.setEventHandler(handleEvent);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Address 0x3C for 128x32

  ButtonConfig* buttonConfig = button.getButtonConfig();
  buttonConfig->setEventHandler(handleEvent);
  buttonConfig->setFeature(ButtonConfig::kFeatureDoubleClick);
  buttonConfig->setFeature(ButtonConfig::kFeatureSuppressClickBeforeDoubleClick);
  buttonConfig->setFeature(ButtonConfig::kFeatureSuppressAfterClick);
  buttonConfig->setFeature(ButtonConfig::kFeatureSuppressAfterDoubleClick);

  // Clear the buffer.
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.display(); // actually display all of the above
  //  delay(1000);

  cli();//diable interrupts

  //set up continuous sampling of analog pin 0 at 38.5kHz

  //clear ADCSRA and ADCSRB registers
  ADCSRA = 0;
  ADCSRB = 0;

  ADMUX |= (1 << REFS0); //set reference voltage
  ADMUX |= (1 << ADLAR); //left align the ADC value- so we can read highest 8 bits from ADCH register only

  ADCSRA |= (1 << ADPS2) | (1 << ADPS0); //set ADC clock with 32 prescaler- 16mHz/32=500kHz
  ADCSRA |= (1 << ADATE); //enabble auto trigger
  ADCSRA |= (1 << ADIE); //enable interrupts when measurement complete
  ADCSRA |= (1 << ADEN); //enable ADC
  ADCSRA |= (1 << ADSC); //start ADC measurements

  sei();//enable interrupts

  digitalWrite(oledPowerPin, LOW);
  display.ssd1306_command(SSD1306_DISPLAYOFF);
}

ISR(ADC_vect) {//when new ADC value ready

  PORTB &= B11101111;//set pin 12 low
  prevData = newData;//store previous value
  newData = ADCH;//get value from A0
  if (prevData < 127 && newData >= 127) { //if increasing and crossing midpoint
    newSlope = newData - prevData;//calculate slope
    if (abs(newSlope - maxSlope) < slopeTol) { //if slopes are ==
      //record new data and reset time
      slope[index] = newSlope;
      timer[index] = time;
      time = 0;
      if (index == 0) { //new max slope just reset
        PORTB |= B00010000;//set pin 12 high
        noMatch = 0;
        index++;//increment index
      }
      else if (abs(timer[0] - timer[index]) < timerTol && abs(slope[0] - newSlope) < slopeTol) { //if timer duration and slopes match
        //sum timer values
        totalTimer = 0;
        for (byte i = 0; i < index; i++) {
          totalTimer += timer[i];
        }
        period = totalTimer;//set period
        //reset new zero index values to compare with
        timer[0] = timer[index];
        slope[0] = slope[index];
        index = 1;//set index to 1
        PORTB |= B00010000;//set pin 12 high
        noMatch = 0;
      }
      else { //crossing midpoint but not match
        index++;//increment index
        if (index > 9) {
          reset();
        }
      }
    }
    else if (newSlope > maxSlope) { //if new slope is much larger than max slope
      maxSlope = newSlope;
      time = 0;//reset clock
      noMatch = 0;
      index = 0;//reset index
    }
    else { //slope not steep enough
      noMatch++;//increment no match counter
      if (noMatch > 9) {
        reset();
      }
    }
  }

  if (newData == 0 || newData == 1023) { //if clipping
    clipping = 1;//currently clipping
    Serial.println("clipping");
  }

  time++;//increment timer at rate of 38.5kHz

  ampTimer++;//increment amplitude timer
  if (abs(127 - ADCH) > maxAmp) {
    maxAmp = abs(127 - ADCH);
  }
  if (ampTimer == 1000) {
    ampTimer = 0;
    checkMaxAmp = maxAmp;
    maxAmp = 0;
  }

}

void reset() { //clean out some variables
  index = 0;//reset index
  noMatch = 0;//reset match couner
  maxSlope = 0;//reset slope
}


void checkClipping() { //manage clipping indication
  if (clipping) { //if currently clipping
    clipping = 0;
  }
}

void bootScreen() {
  display.ssd1306_command(SSD1306_DISPLAYON);
  //  digitalWrite(oledPowerPin, HIGH);
//  delay(250);

  display.clearDisplay();
  display.setFont(&FreeSerifBold9pt7b);
  display.setCursor(10, 22);
  display.print("Guitar Tuner");
  display.display(); // actually display all of the above
  delay(1000);
  screenOn = true;
  showHz = false;
}
bool powerPin = false;
// The event handler for the button.
void handleEvent(AceButton* /* button */, uint8_t eventType, uint8_t /* buttonState */) {
  switch (eventType) {
    case AceButton::kEventClicked:
    case AceButton::kEventReleased:
      //      if (powerPin) {
      //        digitalWrite(oledPowerPin, LOW);
      //        powerPin = false;
      //      }
      //      else {
      //        digitalWrite(oledPowerPin, HIGH);
      //        powerPin = true;
      //      }
      if (!screenOn) {
        bootScreen();
      }
      else {
        //digitalWrite(oledPowerPin, LOW);
        display.clearDisplay();
        display.display();
        screenOn = false;
        display.ssd1306_command(SSD1306_DISPLAYOFF);
        //        delay(250);
        //        digitalWrite(oledPowerPin, LOW);
      }
      break;
    case AceButton::kEventDoubleClicked: // Double click for frequency in Hz
      showHz = !showHz;
      break;
  }
}

void displayLeftMost() {
  display.fillRect(8, 4, 8, farHeight, WHITE); // Leftmost block
  display.fillRect(22, 4, 8, midHeight, WHITE); // Left middle block
  display.fillRect(36, 4, 8, nearHeight, WHITE); // Rightmost left block
}
void displayLeftMiddle() {
  display.fillRect(22, 4, 8, midHeight, WHITE); // Left middle block
  display.fillRect(36, 4, 8, nearHeight, WHITE); // Rightmost left block
}
void displayRightMostLeft() {
  display.fillRect(36, 4, 8, nearHeight, WHITE); // Rightmost left block
}

void displayLeftMostRight() {
  display.fillRect(84, 4, 8, nearHeight, WHITE); // Leftmost right block
}
void displayRightMiddle() {
  display.fillRect(84, 4, 8, nearHeight, WHITE); // Leftmost right block
  display.fillRect(98, 4, 8, midHeight, WHITE); // Right middle block
}
void displayRightMost() {
  display.fillRect(84, 4, 8, nearHeight, WHITE); // Leftmost right block
  display.fillRect(98, 4, 8, midHeight, WHITE); // Right middle block
  display.fillRect(112, 4, 8, farHeight, WHITE); // Rightmost block
}



void displayBars(double note, double lowerLimit, double upperLimit) {
  if (showHz) {
    farHeight = 28;
    midHeight = 21;
    nearHeight = 14;
  }
  else {
    farHeight = 28;
    midHeight = 28;
    nearHeight = 28;
  }
  // Left Side
  if (frequency > lowerLimit && frequency < ((note - lowerLimit) / 3) + lowerLimit) {
    displayLeftMost();
  }
  if (frequency >= ((note - lowerLimit) / 3) + lowerLimit && frequency < (((note - lowerLimit) / 3) * 2) + lowerLimit) {
    displayLeftMiddle();
  }
  if (frequency >= (((note - lowerLimit) / 3) * 2) + lowerLimit && frequency < (note - tolerance)) {
    displayRightMostLeft();
  }

  // Right Side
  if (frequency > (note + tolerance) && frequency <= ((upperLimit - note) / 3) + note) {
    displayLeftMostRight();
  }
  if (frequency > ((upperLimit - note) / 3) + note && frequency <= (((upperLimit - note) / 3) * 2) + note) {
    displayRightMiddle();
  }
  if (frequency > (((upperLimit - note) / 3) * 2) + note && frequency <= upperLimit) {
    displayRightMost();
  }
}


void loop() {

  checkClipping();
  button.check();


  if (checkMaxAmp > ampThreshold) {
    frequency = 38462 / float(period); //calculate frequency timer rate/period

    //print results

    Serial.print(frequency);
    Serial.println(" hz");

    frequency = frequency / 2; // Running at 8MHz instead of 16MHz so frequency is doubled
    if (frequency > 65 && screenOn) {
      display.clearDisplay();
      if (showHz) {
        display.setCursor(58, 14);
        display.setFont(&FreeSerifBold9pt7b);
        //display.fillRect(36, 4, 8, nearHeight, WHITE); // Test for text alignment
        //display.fillRect(84, 4, 8, nearHeight, WHITE); // Test for text alignment
      }
      else {
        display.setCursor(51, 29);
        display.setFont(&FreeSerifBold18pt7b);

      }

      if (frequency > 65 && frequency < 500 && frequency < deMidPoint) { // D: 75.71
        display.print("D");

        displayBars(noteDropD, 65, deMidPoint);
      }
      else if (frequency >= deMidPoint && frequency < eaMidPoint) { // E: 84.35
        display.print("E");

        displayBars(noteLowE, deMidPoint, eaMidPoint);
      }
      else if (frequency >= eaMidPoint && frequency < adMidPoint) { // A: 112.46
        display.print("A");

        displayBars(noteA, eaMidPoint, adMidPoint);
      }
      else if (frequency >= adMidPoint && frequency < dgMidPoint) { // D: 149.08
        display.print("D");

        displayBars(noteD, adMidPoint, dgMidPoint);
      }
      else if (frequency >= dgMidPoint && frequency < gbMidPoint) { // G: 200.32
        display.print("G");

        displayBars(noteG, dgMidPoint, gbMidPoint);
      }
      else if (frequency >= gbMidPoint && frequency < beMidPoint) { // B: 253.04
        display.print("B");

        displayBars(noteB, gbMidPoint, beMidPoint);
      }
      else if (frequency >= beMidPoint) { // E: 337.39
        display.print("E");

        displayBars(noteHighE, beMidPoint, noteHighE + 25);
      }
      if (showHz) {
        display.setCursor(39, 31);
        display.setFont(&FreeSerifBold9pt7b);
        //display.print("Hz: ");
        if (frequency > 65 && frequency < 500) {
          display.print(frequency);
        }
      }
      //yield();
      display.display();
    }
  }
}
