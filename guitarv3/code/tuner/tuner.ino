#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Fonts/FreeSerifBold9pt7b.h> // Font
//#include <Fonts/FreeSerifBold24pt7b.h> // Font
#include <Fonts/FreeSerifBold18pt7b.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);

//Notes:
// D: 75.71
// E: 84.35
// A: 112.46
// D: 149.08
// G: 200.32
// B: 253.04
// E: 337.39
const double noteDropD = 75.71;
const double noteLowE = 84.35;
const double noteA = 112.46;
const double noteD = 149.08;
const double noteG = 200.32;
const double noteB = 253.04;
const double noteHighE = 337.39;

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

  Serial.begin(9600);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Address 0x3C for 128x32

  //  display.display();
  //  delay(1000);

  // Clear the buffer.
  display.clearDisplay();
  //display.display();

  pinMode(13, OUTPUT); //led indicator pin
  pinMode(12, OUTPUT); //output pin

  // text display tests
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  //display.setFont(&FreeSerifBold24pt7b);
  display.setFont(&FreeSerifBold9pt7b);
  //display.setCursor(0, 0);
  display.setCursor(4, 32);
  display.print("Guitar Tuner");
  //  display.print("Connecting to SSID\n'adafruit':");
  //  display.print("connected!");
  //  display.println("IP: 10.0.1.23");
  //  display.println("Sending val #0");
  //  display.setCursor(0, 0);
  display.display(); // actually display all of the above
  delay(1000);

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


void loop() {

  checkClipping();


  if (checkMaxAmp > ampThreshold) {
    frequency = 38462 / float(period); //calculate frequency timer rate/period

    //print results

    Serial.print(frequency);
    Serial.println(" hz");


    //display.display();

    // These are off by a bit than what it says online
    // I printed the frequency first and matched it to a commercial tuner
    //
    //Notes:
    // D: 75.71
    // E: 84.35
    // A: 112.46
    // D: 149.08
    // G: 200.32
    // B: 253.04
    // E: 337.39
    //const int noteDropD = 75.71;
    //const int noteLowE = 84.35;
    //const int noteA = 112.46;
    //const int noteD = 149.08;
    //const int noteG = 200.32;
    //const int noteB = 253.04;
    //const int noteHighE = 337.39;

    double deMidPoint = (noteDropD + noteLowE)/2;
    double eaMidPoint = (noteLowE + noteA)/2;
    double adMidPoint = (noteA + noteD)/2;
    double dgMidPoint = (noteD + noteG)/2;
    double gbMidPoint = (noteG + noteB)/2;
    double beMidPoint = (noteB + noteHighE)/2;

    frequency = frequency / 2; // Running at 8MHz instead of 16MHz so frequency is doubled
    if (frequency > 65) {
      display.clearDisplay();
      display.setCursor(69, 30);
      display.setFont(&FreeSerifBold18pt7b);
      if (frequency > 65 && frequency < deMidPoint) { // D: 75.71
        display.print("D");
        // Left Side
        if (frequency > 65 && frequency < (noteDropD + noteLowE) / 3) {
          display.fillRect(8, 4, 8, 28, WHITE); // Leftmost block
        }
        if (frequency >= (noteDropD + noteLowE) / 3 && frequency < ((noteDropD + noteLowE) / 3) * 2) {
          display.fillRect(22, 4, 8, 28, WHITE); // Left middle block
        }
        if (frequency >= ((noteDropD + noteLowE) / 3) * 2 && frequency < (noteLowE - 1)) {
          display.fillRect(36, 4, 8, 28, WHITE); // Rightmost left block
        }
        // Right Side
        if (frequency > (noteLowE + 1) && frequency <= (noteLowE + noteA) / 3) {
          display.fillRect(84, 4, 8, 28, WHITE); // Leftmost right block
        }
        if (frequency > (noteLowE + noteA) / 3 && frequency <= ((noteLowE + noteA) / 3) * 2) {
          display.fillRect(98, 4, 8, 28, WHITE); // Right middle block
        }
        if (frequency > ((noteLowE + noteA) / 3) * 2 && frequency <= note
      }


    else if (frequency >= deMidPoint && frequency < eaMidPoint { // E: 84.35
        display.print("E");
      }

      else if (frequency >= eaMidPoint && frequency < adMidPoint) { // A: 112.46
        display.print("A");
      }

      else if (frequency >= adMidPoint && frequency < dgMidPoint) { // D: 149.08
        display.print("D");
      }

      else if (frequency >= dgMidPoint && frequency < gbMidPoint) { // G: 200.32
        display.print("G");
      }

      else if (frequency >= gbMidPoint && frequency < beMidPoint) { // B: 253.04
        display.print("B");
      }

      else if (frequency >= beMidPoint) { // E: 337.39
        display.print("E");
      }

      //display.fillRect(8, 4, 8, 28, WHITE);
      //display.fillRect(22, 4, 8, 28, WHITE);
      //display.fillRect(36, 4, 8, 28, WHITE);

      display.fillRect(84, 4, 8, 28, WHITE);
      display.fillRect(98, 4, 8, 28, WHITE);
      display.fillRect(112, 4, 8, 28, WHITE);
      //      display.print("Hz: ");
      //      display.print(frequency / 2);
      //      display.print(" | ");
      //      display

      //delay(10);
      //      display.setCursor(40, 32);
      //      display.setFont(&FreeSerifBold9pt7b);
      //      display.print("Hz: ");
      //      if (frequency > 65 && frequency < 500) {
      //        display.print(frequency);
      //      }
      yield();
      display.display();
    }
  }



  delay(100);

}
