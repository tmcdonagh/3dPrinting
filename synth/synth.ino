#include <anyrtttl.h>
#include <binrtttl.h>
#include <pitches.h>

#include <Wire.h>
#include "Adafruit_MCP23017.h"

Adafruit_MCP23017 mcp;

int buttonPushCounter = 0;   // counter for the number of button presses
int buttonState = 0;         // current state of the button
int lastButtonState = 0;     // previous state of the button

// frets[buttonPos][guitarString][fret]

void printArray ( const int [][3][5] );


// Fret Frequencies
int frets[4][3][5] = {0};







void setup() {

  // When buttonPushCounter == 0 uses first 3 strings of guitar
  frets[0][0][0] = 82; // Open E String
  frets[0][0][1] = 87;
  frets[0][0][2] = 93;
  frets[0][0][3] = 98;
  frets[0][0][4] = 104;
  frets[0][1][0] = 110; // Open A String
  frets[0][1][1] = 117;
  frets[0][1][2] = 124;
  frets[0][1][3] = 131;
  frets[0][1][4] = 139;
  frets[0][2][0] = 147; // Open D String
  frets[0][2][1] = 156;
  frets[0][2][2] = 165;
  frets[0][2][3] = 175;
  frets[0][2][4] = 185;

  // When buttonPushCounter == 1 uses second 3 strings of guitar
  frets[1][0][0] = 110; // Open A String
  frets[1][0][1] = 117;
  frets[1][0][2] = 124;
  frets[1][0][3] = 131;
  frets[1][0][4] = 139;
  frets[1][1][0] = 147; // Open D String
  frets[1][1][1] = 156;
  frets[1][1][2] = 165;
  frets[1][1][3] = 175;
  frets[1][1][4] = 185;
  frets[1][2][0] = 196; // Open G String
  frets[1][2][1] = 208;
  frets[1][2][2] = 220;
  frets[1][2][3] = 233;
  frets[1][2][4] = 247;

  // When buttonPushCounter == 2 uses third 3 strings of guitar
  frets[2][0][0] = 147; // Open D String
  frets[2][0][1] = 156;
  frets[2][0][2] = 165;
  frets[2][0][3] = 175;
  frets[2][0][4] = 185;
  frets[2][1][0] = 196; // Open G String
  frets[2][1][1] = 208;
  frets[2][1][2] = 220;
  frets[2][1][3] = 233;
  frets[2][1][4] = 247;
  frets[2][2][0] = 247; // Open B String
  frets[2][2][1] = 262;
  frets[2][2][2] = 277;
  frets[2][2][3] = 294;
  frets[2][2][4] = 311;

  // When buttonPushCounter == 2 uses last 3 strings of guitar
  frets[3][0][0] = 196; // Open G String
  frets[3][0][1] = 208;
  frets[3][0][2] = 220;
  frets[3][0][3] = 233;
  frets[3][0][4] = 247;
  frets[3][1][0] = 247; // Open B String
  frets[3][1][1] = 262;
  frets[3][1][2] = 277;
  frets[3][1][3] = 294;
  frets[3][1][4] = 311;
  frets[3][2][0] = 330; // Open E String
  frets[3][2][1] = 350;
  frets[3][2][2] = 370;
  frets[3][2][3] = 392;
  frets[3][2][4] = 415;


  mcp.begin();      // use default address 0


  pinMode(23, OUTPUT);
  pinMode(21, OUTPUT);
  pinMode(20, OUTPUT);
  pinMode(14, OUTPUT);



  mcp.pinMode(0, INPUT);
  mcp.pullUp(0, HIGH);  // turn on a 100K pullup internally
  mcp.pinMode(1, INPUT);
  mcp.pullUp(1, HIGH);
  mcp.pinMode(2, INPUT);
  mcp.pullUp(2, HIGH);
  mcp.pinMode(3, INPUT);
  mcp.pullUp(3, HIGH);
  mcp.pinMode(4, INPUT);
  mcp.pullUp(4, HIGH);
  mcp.pinMode(5, INPUT);
  mcp.pullUp(5, HIGH);
  mcp.pinMode(6, INPUT);
  mcp.pullUp(6, HIGH);
  mcp.pinMode(7, INPUT);
  mcp.pullUp(7, HIGH);

  mcp.pinMode(8, INPUT);
  mcp.pullUp(8, HIGH);
  mcp.pinMode(9, INPUT);
  mcp.pullUp(9, HIGH);
  mcp.pinMode(10, INPUT);
  mcp.pullUp(10, HIGH);
  mcp.pinMode(11, INPUT);
  mcp.pullUp(11, HIGH);
  mcp.pinMode(12, INPUT);
  mcp.pullUp(12, HIGH);
  mcp.pinMode(13, INPUT);
  mcp.pullUp(13, HIGH);
  mcp.pinMode(14, INPUT);
  mcp.pullUp(14, HIGH);
  mcp.pinMode(15, INPUT);
  mcp.pullUp(15, HIGH);

  pinMode(13, OUTPUT);  // use the p13 LED as debugging

}



void loop() {

  /*
    digitalWrite(23, HIGH);
    digitalWrite(21, HIGH);
    digitalWrite(20, HIGH);
  */


  // read the pushbutton input pin:
  buttonState = !(mcp.digitalRead(7));

  // compare the buttonState to its previous state
  if (buttonState != lastButtonState) {
    // if the state has changed, increment the counter
    if (buttonState == HIGH) {
      // if the current state is HIGH then the button went from off to on:
      buttonPushCounter++;
    }
    // Delay a little bit to avoid bouncing
    delay(50);
  }
  // save the current state as the last state, for next time through the loop
  lastButtonState = buttonState;


  // turns on the LED every four button pushes by checking the modulo of the
  // button push counter. the modulo function gives you the remainder of the
  // division of two numbers:
  if (buttonPushCounter == 4) {
    buttonPushCounter = 0;
  }
  
    if (buttonPushCounter == 0) {
    digitalWrite(20, HIGH);
    digitalWrite(21, LOW);
    digitalWrite(23, LOW);
    digitalWrite(14, LOW);




    }
    else if (buttonPushCounter == 1) {
    digitalWrite(20, LOW);
    digitalWrite(21, HIGH);
    digitalWrite(23, LOW);
    digitalWrite(14, LOW);

    }
    else if (buttonPushCounter == 2) {
    digitalWrite(20, LOW);
    digitalWrite(21, LOW);
    digitalWrite(23, HIGH);
    digitalWrite(14, LOW);

    }
    else if (buttonPushCounter == 3) {
    digitalWrite(20, LOW);
    digitalWrite(21, LOW);
    digitalWrite(23, LOW);
    digitalWrite(14, HIGH);


    }
  

  // Guitar Mode
  // Top Row
  if (mcp.digitalRead(2) == 0) { // Open
    digitalWrite(13, HIGH);
    tone(22, frets[buttonPushCounter][2][0], 20);
  }
  else if (mcp.digitalRead(5) == 0) { // Fret 1
    digitalWrite(13, HIGH);
    tone(22, frets[buttonPushCounter][2][1], 20);
  }
  else if (mcp.digitalRead(4) == 0) { // Fret 2
    digitalWrite(13, HIGH);
    tone(22, frets[buttonPushCounter][2][2], 20);
  }
  else if (mcp.digitalRead(3) == 0) { // Fret 3
    digitalWrite(13, HIGH);
    tone(22, frets[buttonPushCounter][2][3], 20);
  }
  else if ( mcp.digitalRead(6) == 0) { // Fret 4
    digitalWrite(13, HIGH);
    tone(22, frets[buttonPushCounter][2][4], 20);
  }


  // Middle Row
  else if (mcp.digitalRead(1) == 0) { // Open
    digitalWrite(13, HIGH);
    tone(22, frets[buttonPushCounter][1][0], 20);
  }
  else if (mcp.digitalRead(0) == 0) { // Fret 1
    digitalWrite(13, HIGH);
    tone(22, frets[buttonPushCounter][1][1], 20);
  }
  else if (mcp.digitalRead(15) == 0) { // Fret 2
    digitalWrite(13, HIGH);
    tone(22, frets[buttonPushCounter][1][2], 20);
  }
  else if (mcp.digitalRead(14) == 0) { // Fret 3
    digitalWrite(13, HIGH);
    tone(22, frets[buttonPushCounter][1][3], 20);
  }
  else if (mcp.digitalRead(13) == 0) { // Fret 4
    digitalWrite(13, HIGH);
    tone(22, frets[buttonPushCounter][1][4], 20);
    
  }


  // Bottom Row
  else if (mcp.digitalRead(12) == 0) { // Open
    digitalWrite(13, HIGH);
    tone(22, frets[buttonPushCounter][0][0], 20);
  }
  else if (mcp.digitalRead(11) == 0) { // Fret 1
    digitalWrite(13, HIGH);
    tone(22, frets[buttonPushCounter][0][1], 20);
  }
  else if (mcp.digitalRead(10) == 0) { // Fret 2
    digitalWrite(13, HIGH);
    tone(22, frets[buttonPushCounter][0][2], 20);
  }
  else if (mcp.digitalRead(9) == 0) { // Fret 3
    digitalWrite(13, HIGH);
    tone(22, frets[buttonPushCounter][0][3], 20);
  }
  else if (mcp.digitalRead(8) == 0) { // Fret 4
    digitalWrite(13, HIGH);
    tone(22, frets[buttonPushCounter][0][4], 20);
  }

  else {
    digitalWrite(13, LOW);
  }



  //Serial.print(mcp.digitalRead(8));
  //Serial.print(" ");
  //Serial.print(frets[0][1][4]);
  //delay(1000);

}
