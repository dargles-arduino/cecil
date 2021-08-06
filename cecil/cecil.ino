/**
 * Program: cecil
 * Purpose:
 *   Cecil is a simulation of an early teaching microcontroller. It has 1K of 
 *   10 bit wide memory and runs machine code.
 * @author: David Argles, d.argles@gmx.com
 */

/* Program identification */ 
#define PROG    "Cecil"
#define VER     "1.0"
#define BUILD   "06aug2021 @05:46h"

/* Necessary includes */
#include "flashscreen.h"
#include "sim40.h"

/* Global "defines" - may have to look like variables because of type */
long int baudrate = 115200;     // Baudrate for serial output

/* ----- Initialisation ------------------------------------------------- */

/* Global stuff that must happen outside setup() */
flashscreen flash;
sim40       sim;

void setup() {
  // Start up the serial output port
  Serial.begin(baudrate);
  // Serial.setDebugOutput(true);

  // Send program details to serial output
  flash.message(PROG, VER, BUILD);

  // put your setup code here, to run once:
  int values[] = {32,43,81};
  sim.loadMem(4,values);
  sim.displayMem(0,24);
}

void loop() {
  // put your main code here, to run repeatedly:

}
