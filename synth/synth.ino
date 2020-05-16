#include <anyrtttl.h>
#include <binrtttl.h>
#include <pitches.h>

#include <Wire.h>
#include "Adafruit_MCP23017.h"

//#include "pitches.h"

// Basic pin reading and pullup test for the MCP23017 I/O expander
// public domain!

// Connect pin #12 of the expander to Analog 5 (i2c clock)
// Connect pin #13 of the expander to Analog 4 (i2c data)
// Connect pins #15, 16 and 17 of the expander to ground (address selection)
// Connect pin #9 of the expander to 5V (power)
// Connect pin #10 of the expander to ground (common ground)
// Connect pin #18 through a ~10kohm resistor to 5V (reset pin, active low)

// Input #0 is on pin 21 so connect a button or switch from there to ground

Adafruit_MCP23017 mcp;

void setup() {
  mcp.begin();      // use default address 0

  /*
  pinMode(23, OUTPUT);
  pinMode(21, OUTPUT);
  pinMode(20, OUTPUT);
  */

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

  digitalWrite(23, HIGH);
  digitalWrite(21, HIGH);
  digitalWrite(20, HIGH);
  
  if (mcp.digitalRead(7) == 1) { // Button is out
    // Piano Mode
    if (mcp.digitalRead(8) == 0) {
      digitalWrite(13, HIGH);
      tone(22, 110, 20);
    }
    if (mcp.digitalRead(9) == 0) {
      digitalWrite(13, HIGH);
      tone(22, 117, 20);
    }
    if (mcp.digitalRead(10) == 0) {
      digitalWrite(13, HIGH);
      tone(22, 124, 20);
    }
    if (mcp.digitalRead(11) == 0) {
      digitalWrite(13, HIGH);
      tone(22, 131, 20);
    }
    if (mcp.digitalRead(12) == 0) {
      digitalWrite(13, HIGH);
      tone(22, 139, 20);
    }
    if (mcp.digitalRead(13) == 0) {
      digitalWrite(13, HIGH);
      tone(22, 147, 20);
    }
    if (mcp.digitalRead(14) == 0) {
      digitalWrite(13, HIGH);
      tone(22, 156, 20);
    }
    if (mcp.digitalRead(15) == 0) {
      digitalWrite(13, HIGH);
      tone(22, 165, 20);
    }
    if (mcp.digitalRead(0) == 0) {
      digitalWrite(13, HIGH);
      tone(22, 175, 20);
    }
    if (mcp.digitalRead(1) == 0) {
      digitalWrite(13, HIGH);
      tone(22, 185, 20);
    }
    if (mcp.digitalRead(6) == 0) {
      digitalWrite(13, HIGH);
      tone(22, 196, 20);
    }
    if (mcp.digitalRead(3) == 0) {
      digitalWrite(13, HIGH);
      tone(22, 208, 20);
    }
    if (mcp.digitalRead(4) == 0) {
      digitalWrite(13, HIGH);
      tone(22, 220, 20);
    }
    if (mcp.digitalRead(5) == 0) {
      digitalWrite(13, HIGH);
      tone(22, 233, 20);
    }
    if (mcp.digitalRead(2) == 0) {
      digitalWrite(13, HIGH);
      tone(22, 247, 20);
    }

    else {
      digitalWrite(13, LOW);
    }
  }
  else if (mcp.digitalRead(7) == 0) { // Button is in
    // Guitar Mode 

    /* Needs second button to be pressed
    // G
    if (mcp.digitalRead(6) == 0 && mcp.digitalRead(2) == 0) { // Fret 1 G
      digitalWrite(13, HIGH);
      tone(22, 208, 20);
    }
    else if (mcp.digitalRead(6) == 0 && mcp.digitalRead(5) == 0) { // Fret 2 G
      digitalWrite(13, HIGH);
      tone(22, 220, 20);
    }
    else if (mcp.digitalRead(6) == 0 && mcp.digitalRead(4) == 0) { // Fret 3 G
      digitalWrite(13, HIGH);
      tone(22, 233, 20);
    }
    else if (mcp.digitalRead(6) == 0 && mcp.digitalRead(3) == 0) { // Fret 4 G
      digitalWrite(13, HIGH);
      tone(22, 247, 20);
    }
    else if (mcp.digitalRead(6) == 0) { // Open G
      digitalWrite(13, HIGH);
      tone(22, 196, 20);
    }
    
    // D
    if (mcp.digitalRead(13) == 0 && mcp.digitalRead(1) == 0) { // Fret 1 D
      digitalWrite(13, HIGH);
      tone(22, 156, 20);
    }
    else if (mcp.digitalRead(13) == 0 && mcp.digitalRead(0) == 0) { // Fret 2 D
      digitalWrite(13, HIGH);
      tone(22, 165, 20);
    }
    else if (mcp.digitalRead(13) == 0 && mcp.digitalRead(15) == 0) { // Fret 3 D
      digitalWrite(13, HIGH);
      tone(22, 175, 20);
    }
    else if (mcp.digitalRead(13) == 0 && mcp.digitalRead(14) == 0) { // Fret 4 D
      digitalWrite(13, HIGH);
      tone(22, 185, 20);
    }
    else if (mcp.digitalRead(13) == 0) {
      digitalWrite(13, HIGH);
      tone(22, 147, 21);
    }
    
    // A
    if (mcp.digitalRead(8) == 0 && mcp.digitalRead(12) == 0) { // Fret 1 A
      digitalWrite(13, HIGH);
      tone(22, 117, 20);
    }
    else if (mcp.digitalRead(8) == 0 && mcp.digitalRead(11) == 0) { // Fret 2 A
      digitalWrite(13, HIGH);
      tone(22, 124, 20);
    }
    else if (mcp.digitalRead(8) == 0 && mcp.digitalRead(10) == 0) { // Fret 3 A
      digitalWrite(13, HIGH);
      tone(22, 131, 20);
    }
    else if (mcp.digitalRead(8) == 0 && mcp.digitalRead(9) == 0) { // Fret 4 A
      digitalWrite(13, HIGH);
      tone(22, 139, 20);
    }
    else if (mcp.digitalRead(8) == 0) {
      digitalWrite(13, HIGH);
      tone(22, 110, 22);
    }
    else {
      digitalWrite(13, LOW);
    }
    */
    // Doesn't need second button
    // G
    if (mcp.digitalRead(2) == 0) { // Fret 1 G
      digitalWrite(13, HIGH);
      tone(22, 196, 20);
    }
    else if (mcp.digitalRead(5) == 0) { // Fret 2 G
      digitalWrite(13, HIGH);
      tone(22, 208, 20);
    }
    else if (mcp.digitalRead(4) == 0) { // Fret 3 G
      digitalWrite(13, HIGH);
      tone(22, 220, 20);
    }
    else if ( mcp.digitalRead(3) == 0) { // Fret 4 G
      digitalWrite(13, HIGH);
      tone(22, 233, 20);
    }
    else if (mcp.digitalRead(6) == 0) { // Open G
      digitalWrite(13, HIGH);
      tone(22, 247, 20);
    }
    
    // D
    if (mcp.digitalRead(1) == 0) { // Fret 1 D
      digitalWrite(13, HIGH);
      tone(22, 147, 20);
    }
    else if (mcp.digitalRead(0) == 0) { // Fret 2 D
      digitalWrite(13, HIGH);
      tone(22, 156, 20);
    }
    else if (mcp.digitalRead(15) == 0) { // Fret 3 D
      digitalWrite(13, HIGH);
      tone(22, 165, 20);
    }
    else if (mcp.digitalRead(14) == 0) { // Fret 4 D
      digitalWrite(13, HIGH);
      tone(22, 175, 20);
    }
    else if (mcp.digitalRead(13) == 0) {
      digitalWrite(13, HIGH);
      tone(22, 185, 21);
    }
    
    // A
    if (mcp.digitalRead(12) == 0) { // Fret 1 A
      digitalWrite(13, HIGH);
      tone(22, 110, 20);
    }
    else if (mcp.digitalRead(11) == 0) { // Fret 2 A
      digitalWrite(13, HIGH);
      tone(22, 117, 20);
    }
    else if (mcp.digitalRead(10) == 0) { // Fret 3 A
      digitalWrite(13, HIGH);
      tone(22, 124, 20);
    }
    else if (mcp.digitalRead(9) == 0) { // Fret 4 A
      digitalWrite(13, HIGH);
      tone(22, 131, 20);
    }
    else if (mcp.digitalRead(8) == 0) {
      digitalWrite(13, HIGH);
      tone(22, 139, 22);
    }
    else {
      digitalWrite(13, LOW);
    }


  }
  //Serial.print(mcp.digitalRead(8));
  //delay(1000);

}
