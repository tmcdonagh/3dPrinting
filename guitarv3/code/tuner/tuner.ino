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

//const int buttonPin = 6;

const int oledPowerPin = 2;

//AceButton button(buttonPin);
AceButton button(A2);
void handleEvent(AceButton*, uint8_t, uint8_t);

//Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 64, &Wire);

// Tested Hz values
//
// C# 69.93
// D 74.25
// D# 78.17
// E 83.25
// F 88.22
// F# 93.35
// G 99.13
// G# 104.52
// A 111.16
// A# 117.26
// B 124.88
// C 131.72
// C# 140.37
// D 149.08
// D# 157.63
// E 167.23
// F 176.43
// F# 186.71
// G 198.26
// G# 209.03
// A 223.62
// A# 237.42
// B 249.75
// C 267.10
// C# 278.71
// D 300.48
// D# 315.26
// E 331.57
// F 349.65
// F# 369.83
// G 400.65
// G# 418.07
// A 447.23
// A# 469.05
// B 493.10
// C 519.76
// C# 565.62
// D 600.97
// D# 620.35
// E 663.14
// F 712.26
// F# 739.65
// G 769.24

const double cs = 69.93;
const double d = 74.25;
const double ds = 78.17;
const double e = 83.25;
const double f = 88.22;
const double fs = 93.35;
const double g = 99.13;
const double gs = 104.52;
const double a1 = 110.52;
const double as1 = 117.26;
const double b1 = 124.88;
const double c1 = 131.72;
const double cs1 = 140.37;
const double d1 = 147.93;
const double ds1 = 157.63;
const double e1 = 167.23;
const double f1 = 176.43;
const double fs1 = 186.71;
const double g1 = 196.23;
const double gs1 = 209.03;
const double a2 = 223.62;
const double as2 = 237.42;
const double b2 = 249.75;
const double c2 = 267.10;
const double cs2 = 278.71;
const double d2 = 300.48;
const double ds2 = 315.26;
const double e2 = 331.57;
const double f2 = 349.65;
const double fs2 = 369.83;
const double g2 = 400.65;
const double gs2 = 418.07;
const double a3 = 447.23;
const double as3 = 469.05;
const double b3 = 493.10;
const double c3 = 519.76;
const double cs3 = 565.62;
const double d3 = 600.97;
const double ds3 = 620.35;
const double e3 = 663.14;
const double f3 = 712.26;
const double fs3 = 739.65;
const double g3 = 769.24;








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
//byte ampThreshold = 30;//raise if you have a very noisy signal
byte ampThreshold = 30;//raise if you have a very noisy signal

void setup() {

  //Serial.begin(9600);



  //pinMode(buttonPin, INPUT_PULLUP);
  pinMode(A2, INPUT_PULLUP);
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
  showHz = true;
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



//void displayBars(double note, double lowerLimit, double upperLimit) {
//  if (showHz) {
//    farHeight = 28;
//    midHeight = 21;
//    nearHeight = 14;
//  }
//  else {
//    farHeight = 28;
//    midHeight = 28;
//    nearHeight = 28;
//  }
//  // Left Side
//  if (frequency > lowerLimit && frequency < ((note - lowerLimit) / 3) + lowerLimit) {
//    displayLeftMost();
//  }
//  if (frequency >= ((note - lowerLimit) / 3) + lowerLimit && frequency < (((note - lowerLimit) / 3) * 2) + lowerLimit) {
//    displayLeftMiddle();
//  }
//  if (frequency >= (((note - lowerLimit) / 3) * 2) + lowerLimit && frequency < (note - tolerance)) {
//    displayRightMostLeft();
//  }
//
//  // Right Side
//  if (frequency > (note + tolerance) && frequency <= ((upperLimit - note) / 3) + note) {
//    displayLeftMostRight();
//  }
//  if (frequency > ((upperLimit - note) / 3) + note && frequency <= (((upperLimit - note) / 3) * 2) + note) {
//    displayRightMiddle();
//  }
//  if (frequency > (((upperLimit - note) / 3) * 2) + note && frequency <= upperLimit) {
//    displayRightMost();
//  }
//}

void displayBars(double note, double belowNote, double aboveNote, String letter) {
  double lowerLimit = (belowNote + note) / 2;
  double upperLimit = (note + aboveNote) / 2;

  if (frequency >= lowerLimit && frequency < upperLimit) {

    display.print(letter);
    
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
}


void loop() {

  checkClipping();
  button.check();


  if (checkMaxAmp > ampThreshold) {
    frequency = 38462 / float(period); //calculate frequency timer rate/period

    //print results

    Serial.print(frequency);
    Serial.println(" hz");

    //frequency = frequency / 2; // Running at 8MHz instead of 16MHz so frequency is doubled
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

      displayBars(cs, 30, d, "C#");
      displayBars(d, cs, ds, "D");
      displayBars(ds, d, e, "D#");
      displayBars(e, ds, f, "E");
      displayBars(f, e, fs, "F");
      displayBars(fs, f, g, "F#");
      displayBars(g, fs, gs, "G");
      displayBars(gs, g, a1, "G#");
      displayBars(a1, gs, as1, "A");
      displayBars(as1, a1, b1, "A#");
      displayBars(b1, as1, c1, "B");
      displayBars(c1, b1, cs1, "C");

      displayBars(cs1, c1, d1, "C#");
      displayBars(d1, cs1, ds1, "D");
      displayBars(ds1, d1, e1, "D#");
      displayBars(e1, ds1, f1, "E");
      displayBars(f1, e1, fs1, "F");
      displayBars(fs1, f1, g1, "F#");
      displayBars(g1, fs1, gs1, "G");
      displayBars(gs1, g1, a2, "G#");
      displayBars(a2, gs1, as2, "A");
      displayBars(as2, a2, b2, "A#");
      displayBars(b2, as2, c2, "B");
      displayBars(c2, b2, cs2, "C");

      displayBars(cs2, c2, d2, "C#");
      displayBars(d2, cs2, ds2, "D");
      displayBars(ds2, d2, e2, "D#");
      displayBars(e2, ds2, f2, "E");
      displayBars(f2, e2, fs2, "F");
      displayBars(fs2, f2, g2, "F#");
      displayBars(g2, fs2, gs2, "G");
      displayBars(gs2, g2, a3, "G#");
      displayBars(a3, gs2, as3, "A");
      displayBars(as3, a3, b3, "A#");
      displayBars(b3, as3, c3, "B");
      displayBars(c3, b3, cs3, "C");

      displayBars(cs3, c3, d3, "C#");
      displayBars(d3, cs3, ds3, "D");
      displayBars(ds3, d3, e3, "D#");
      displayBars(e3, ds3, f3, "E");
      displayBars(f3, e3, fs3, "F");
      displayBars(fs3, f3, g3, "F#");
      displayBars(g3, fs3, 820.00, "G");
      /*
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
      */
      if (showHz) {
        display.setCursor(39, 31);
        display.setFont(&FreeSerifBold9pt7b);
        //display.print("Hz: ");
        if (frequency > 65 && frequency < 800) {
          display.print(frequency);
        }
      }
      //yield();
      display.display();
    }
  }
  //delay(100); // Stops the reading from going crazy but messes up double button press
}
