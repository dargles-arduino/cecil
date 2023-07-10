/**
 * Routines to handle web access
 * 
 * !NOTE! - this class requires serial output to have been started up first.
 * 
 * @author  David Argles, d.argles@gmx.com
 * @version 06July2023 12:30h
 * @param   WiFiClient  client  The incoming WiFi client
 */

void sendHead(WiFiClient client, String program, String memory, String simStatus)
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
  client.println("      <form action=\"compile\" method=\"get\">");
  client.println("        <pre><textarea name=\"program\" rows=\"15\" cols=\"60\">");
  client.println(program);
  client.println("        </textarea></pre>");
  client.println("        <input type=\"submit\" value=\"Compile\">");
  client.println("      </form>");
  client.println("    </section>");
  client.println("    <section>");
  client.println("      <h2>Object Code</h2>");
  client.println("      <pre><textarea name=\"code\" rows=\"5\" cols=\"60\">");
  client.println(memory);
  client.println("      </textarea></pre>");
  if(simStatus == "halted"){
    client.println("      <form action=\"run\" method=\"get\">");
    client.println("        <input type=\"submit\" value=\"Run\">");
  }
  else{
    client.println("      <form action=\"halt\" method=\"get\">");
    client.println("        <input type=\"submit\" value=\"Halt\">");
  }
  client.println("      </form>");
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

String serviceWebRequest(WiFiClient client, String program, String memory, String simStatus)
{
    Serial.print("New Client: ");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        // Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // Send the headers
            sendHead(client, program, memory, simStatus);
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

        if (currentLine.startsWith("Requester"))Serial.println("Requesting "+ currentLine);
        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith("GET /compile")) {
          Serial.println("Starting compilation");
          simStatus = "compiling";
        }
        if (currentLine.endsWith("GET /run")) {
          Serial.println("Beginning program run");
          simStatus = "running";
        }
        if (currentLine.endsWith("GET /halt")) {
          Serial.println("Terminating program run");
          simStatus = "halted";
        }
        if (currentLine.endsWith("GET /H")) {
          Serial.println("Turning LED on");
          digitalWrite(BUILTIN_LED, HIGH);               // GET /H turns the LED on
        }
        if (currentLine.endsWith("GET /L")) {
          Serial.println("Turning LED off");
          digitalWrite(BUILTIN_LED, LOW);                // GET /L turns the LED off
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
    return simStatus;
}
