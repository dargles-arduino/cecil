/**
 * Program: cecil
 * Purpose:
 *   CECIL is an assembly programming language which runs on a SIM40 
 *   microcontroller. The SIM40 has 1K of 10 bit wide memory and runs 
 *   machine code. Both the CECIL language and the SIM40 CPU are simplified
 *   designs which are constructed to make learning the relevant concepts 
 *   easier. Nevertheless, both CECIL and the SIM40 are sufficiently 
 *   full designs that they are capable of being implemented in practice.
 *   This version is designed to run on an ESP32 (e.g. Node32S).
 * @author: David Argles, d.argles@gmx.com
 */

/* Program identification */ 
#define PROG    "Cecil"
#define VER     "1.0"
#define BUILD   "13jul2023 @00:46h"

/* Necessary includes */
#include "flashscreen.h"
#include <WiFiManager.h> // See https://github.com/tzapu/WiFiManager
#include "sim40.h"
#include "compiler.h"
#include "webServer.h"

/* Global "defines" - may have to look like variables because of type */
long int baudrate = 115200;     // Baudrate for serial output

/* ----- Initialisation ------------------------------------------------- */

/* Global stuff that must happen outside setup() */
bool        InetConnected;
WiFiServer  server(80);
WiFiClient  client;
String      simStatus = "halted"; // halted/compiling/running; can be clear temporarily, just to get the info around
String      prevStatus = "halted";
sim40       sim;
compiler    Compiler;
int         values[] = {1,11,37,32,31,37,0,2,38,5,3,523,65,66,23,0}; // Note: this is a program to add 2 nos.
int         valuesSize;

void setup() {
  // Start up the serial output port
  Serial.begin(baudrate);
  // Serial.setDebugOutput(true);

  // Send program details to serial output
  flashscreen flash;
  flash.message(PROG, VER, BUILD);

  //WiFiManager, Local intialization, only used here
  WiFiManager wm;

  // Uncomment to wipe stored credentials: e.g. for testing
  // wm.resetSettings();

  // Get the WiFi going; get local WiFi credentials if necessary
  InetConnected = wm.autoConnect(PROG); // anonymous ap

  if(InetConnected){
    // Start up the web server
    server.begin();
  }

  // put your setup code here, to run once:
  valuesSize = (sizeof(values)/sizeof(values[0]));
  if(!sim.loadMem(0,values, valuesSize)) Serial.println("Oops! Memory write failed");
  Serial.println(sim.displayMem(0,valuesSize));
  sim.setStartVector(0);
  sim.running=true;
  simStatus = "running";
}

void loop() {
  // put your main code here, to run repeatedly:
  if(simStatus != prevStatus){
    Serial.println("Sim status: "+simStatus);
    prevStatus = simStatus;
  }
  
  // check for incoming web clients
  client = server.available();
  if(client) {
    String memDump = sim.displayMem(0, Compiler.endLoc);
    simStatus = serviceWebRequest(client, Compiler.program, memDump, sim.output, simStatus);
  }

  if(simStatus=="compiling"){
    //Serial.println("Updated program is:");
    //Serial.println(progUpdate);
    Compiler.program = progUpdate;
    if(Compiler.compile()){
      // Compilation was successful
      Serial.println("Compiled successfully");
      if(!sim.loadMem(Compiler.startLoc, Compiler.code , Compiler.endLoc)) Serial.println("Oops! Memory write failed");
      sim.output += Compiler.output;
    }
    else Serial.println("Failed to compile");
    simStatus = "halted";
  }
  if(simStatus=="clear"){
    sim.output = "";
    simStatus = prevStatus;
  }
  if((simStatus == "running") && (prevStatus != "running"))sim.beginRun();
  if(sim.running){
    sim.doInstruction();
    if(!sim.running)simStatus = "halted";
  }

  delay(100);
}
