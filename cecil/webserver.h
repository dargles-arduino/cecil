/**
 * Class definition for webserver
 * 
 * The webserver class handles anything to do with our webserver
 * @author  David Argles, d.argles@gmx.com
 * @version 21Feb2019, updated for ESP32 18Aug2021
 */

#include <WiFi.h>       // Include the ESP32 WiFi header file
#include <WiFiClient.h>
#include <WiFiAP.h>
#include "compiler.h"

#define LED_BUILTIN 2

const char *ssid = "Cecil";
const char *password = "Cecil123";

WiFiServer server(80);  // Initialize the ESP webserver on Port 80
compiler   myCompiler;

class webserver
{
  private:

  void  buildPageTop(){
    pageTop += "<!DOCTYPE html>\n";
    pageTop += "<html>\n";
    pageTop += "  <head>\n";
    pageTop += "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
    pageTop += "  </head>\n";
    pageTop += "  <body>\n";
    pageTop += "    <h3>CECIL</h3>\n"; 
    pageTop += "    <form action='form.php' method='get'>\n";
    return;
  }

  void  buildPageBottom(){
    pageBottom += "      <input type='submit' value='Compile'>\n";
    pageBottom += "      <input type='submit' value='Run'>\n";
    pageBottom += "    </form>\n"; 
    pageBottom += "  </body>\n";
    pageBottom += "</html>\n";
    return;
  }

  public:
  
  bool live = false;
  
  //String ipAddress = "Server IP is: ";  // Set up our ipAddress message

  String pageTop = "";
  String pageContent = "";
  String pageBottom = "";
  
  // The constructor
  webserver(){
  }

  void begin(){
    Serial.println("Starting up web server:");
    WiFi.softAP(ssid, password);     // Provide the (SSID, password); . 
    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(myIP);
    server.begin();
    Serial.println("Server started");
    //IPAddress HTTPS_ServerIP = WiFi.softAPIP(); // Obtain the IP of the Server 
    //ipAddress += HTTPS_ServerIP;
    live = true;
    buildPageTop();
    buildPageBottom();
    return;
  }

  void pageAdd(String content){
    pageContent += content;
  }

  void servePage(String buffContents){
    WiFiClient client = server.available();
    if(client) 
    {
      pageContent += "      <textarea id=\"program\" width = \"70\" height = \"80\">";
      pageContent += myCompiler.program;
      pageContent += "</textarea>\n";
      myCompiler.compile();
      pageContent += myCompiler.output;
      if(myCompiler.compiled)pageContent += "Program compiled\n";

      client.flush();             // Clear previous info in the stream 
      client.print(pageTop);      // Send the page top to the client 
      client.print(pageContent);  // Send the page content to the client 
      client.print(pageBottom);   // Send the page bottom to the client 
    }
    client.stop();
  }
};
