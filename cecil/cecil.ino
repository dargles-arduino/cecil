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
#define BUILD   "22jul2023 @15:10h"

bool    trace = true; // To be truly global, this needs to come here
//bool    tmpBool;

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
String      webCommand = "none"; // none/halt/compile/run/clear; to get the info around
String      prevWebCommand = "none";
sim40       sim;
compiler    Compiler;
int         values[] = {1,11,37,32,31,37,0,2,38,5,3,523,65,66,23,0}; // Note: this is a program to add 2 nos.
int         valuesSize;
int         sv;  // For temporary startVector

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
  sim.setRunStatus(true);
  //simStatus = "running";
}

void loop() 
{
  // put your main code here, to run repeatedly:
  //Serial.print("Sim status: ");
  //Serial.println(simStatus);
  /*if(simStatus != prevStatus)
  {
    if(trace)Serial.println("Sim status changed, now: "+simStatus);
    prevStatus = simStatus;
  }*/
  webCommand = "none";
  // check for incoming web clients
  client = server.available();
  if(client) 
  {
    Serial.printf("sim.running is: %i", sim.getRunStatus());
    String memDump = sim.displayMem(0, 23);
    String regs = sim.getRegs();
    webCommand = serviceWebRequest(client, Compiler.program, memDump, regs, sim.output, sim.getRunStatus());
    Serial.printf("webCommand set by web client, now= %s\n",webCommand);
    Serial.printf("And sim.running is: %i", sim.getRunStatus());
  }
  client.stop();
  
  if(webCommand=="compile")
  {
    //Serial.println("Updated program is:");
    //Serial.println(progUpdate);
    Compiler.program = progUpdate;
    if((sv=Compiler.compile(sim.getStartVector()))!=-1)
    {
      // Compilation was successful
      Serial.println("Compiled successfully");
      sim.setStartVector(sv);
      if(!sim.loadMem(Compiler.startLoc, Compiler.code , Compiler.endLoc)) Serial.println("Oops! Memory write failed");
    }
    else Serial.println("Failed to compile");
    sim.output += Compiler.output;
    sim.setRunStatus(false);
  }
  if(webCommand=="clear")
  {
    sim.output = "";
    webCmd = "none";
  }
  if(webCommand == "run")
  {
    sim.setRunStatus(sim.beginRun());
    Serial.printf("sim.getRunStatus() is now: %i\n", sim.getRunStatus());
  }
  if(webCommand == "halt")
  {
    sim.setRunStatus(false);
    Serial.printf("sim.getRunStatus() is now: %i\n", sim.getRunStatus());
  }
  //Serial.printf("sim.getRunStatus() is: %i\n", sim.getRunStatus());
  while(sim.getRunStatus())
  {
    sim.doInstruction();
    //if(!sim.getRunStatus())simStatus = "halted";
  }
  delay(100);
}
