/**
 * Routines to handle web access
 * 
 * !NOTE! - this class requires serial output to have been started up first.
 * 
 * @author  David Argles, d.argles@gmx.com
 * @version 06July2023 12:30h
 * @param   WiFiClient  client  The incoming WiFi client
 */

#include "compiler.h"

compiler CecilProg;

 void sendHead(WiFiClient client)
{
  // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
  // and a content-type so the client knows what's coming, then a blank line:
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html");
  client.println();
  // Now send the initial HTML
  client.println("<!DOCTYPE html>");
  client.println("<html lang=\"en\">");
  client.println("  <head>");
  client.println("    <meta charset=\"UTF-8\">");
  client.println("    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">");
  String line = "    <title>";
  line = line + PROG + "</title>";
  client.println(line);
  client.println("  </head>");
  client.println("  <body>");
  client.println("    <h1>CECIL</h1>");
  client.println("    <section>");
  client.println("      <h2>Program</h2>");
  client.println("      <pre>");
  client.println(CecilProg.program);
  client.println("      </pre>");
  client.println("    </section>");

  return;
}

void sendTail(WiFiClient client)
{
  // Send the final HTML
  client.println("  </body>");
  client.println("</html>");
  return;
}

void serviceWebRequest(WiFiClient client)
{
    Serial.println("New Client.");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // Send the headers
            sendHead(client);
            // the content of the HTTP response follows the header:
            client.print("Click <a href=\"/H\">here</a> to turn the LED on pin 5 on.<br>");
            client.print("Click <a href=\"/L\">here</a> to turn the LED on pin 5 off.<br>");
            sendTail(client);
            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith("GET /H")) {
          digitalWrite(BUILTIN_LED, HIGH);               // GET /H turns the LED on
        }
        if (currentLine.endsWith("GET /L")) {
          digitalWrite(BUILTIN_LED, LOW);                // GET /L turns the LED off
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
}
