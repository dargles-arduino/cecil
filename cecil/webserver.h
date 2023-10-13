/**
 * Routines to handle web access
 * 
 * !NOTE! - this class requires serial output to have been started up first.
 * 
 * @author  David Argles, d.argles@gmx.com
 * @version 06July2023 12:30h
 * @param   WiFiClient  client  The incoming WiFi client
 */

String  progUpdate = "";
//bool    trace = true;
String  webCmd;

void tidyProgram(String program){
  int ptr;
  // Cut the front of the GET line
  ptr = program.indexOf("?program=") + 9;
  progUpdate = program.substring(ptr, program.length()); //program.indexOf(terminator);
  // Now replace "+" chars with tabs
  progUpdate.replace("+", " "); 
  progUpdate.replace("%0D%0A", "\n"); 
  progUpdate.replace("%3B", ";");
  progUpdate = progUpdate.substring(0,progUpdate.indexOf("HTTP/"));
  //Serial.println(progUpdate);
  return;
}

/**
 * sendHead()
 * 
 * Sends the HTTP headers and initial HTML that applies for any page we might 
 * wish to return to the requester.
 */
void sendHead(WiFiClient client, bool simStatus, bool redirect){
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
  if(redirect) client.println("    <meta http-equiv=\"refresh\" content=\"3; url='/'\">");
  String line = "    <title>";
  line = line + PROG + "</title>";
  client.println(line);
  client.println("    <style>body{font-family: Arial, Helvetica, sans-serif; background-color: cyan;}</style>");
  client.println("  </head>");
  client.println("  <body>");
  client.println("    <h1>CECIL</h1>");
  return;
}

/**
 * sendBody()
 * 
 * Sends the HTML for the default CECIL page 
 */
void sendBody(WiFiClient client, String program, String memory, String registers, String videoOutput, bool simStatus)
{
  client.print("    <p>Current status of SIM40: <strong>");
  if(simStatus)client.print("running");
  else client.print("halted");
  client.println("</strong></p>");
  client.println("    <section>");
  client.println("      <h2>Program</h2>");
  client.println("      <form action=\"compile\" method=\"get\">");
  client.println("        <pre><textarea name=\"program\" rows=\"15\" cols=\"48\">");
  client.println(program);
  client.println("        </textarea></pre>");
  client.println("        <input type=\"submit\" value=\"Compile\">");
  client.println("      </form>");
  client.println("    </section>");
  client.println("    <section>");
  client.println("      <h2>Memory</h2>");
  client.println("      <h3>Main SIM memory</h3>");
  client.println("      <pre><textarea name=\"code\" rows=\"5\" cols=\"48\">");
  client.println(memory);
  client.println("      </textarea></pre>");
  client.println("      <h3>Registers</h3>");
  client.println("      <pre><textarea name=\"code\" rows=\"8\" cols=\"48\">");
  client.println(registers);
  client.println("      </textarea></pre>");
  if(simStatus){
    client.println("      <form action=\"halt\" method=\"get\">");
    client.println("        <input type=\"submit\" value=\"Halt\">");
  }
  else{
    client.println("      <form action=\"run\" method=\"get\">");
    client.println("        <input type=\"submit\" value=\"Run\">");
  }
  client.println("      </form>");
  client.println("      <h2>Video Output</h2>");
  client.println("      <form action=\"clear\" method=\"get\">");
  client.println("        <pre><textarea name=\"output\" rows=\"15\" cols=\"48\">");
  client.println(videoOutput);
  client.println("        </textarea></pre>");
  client.println("        <input type=\"submit\" value=\"Clear\">");
  client.println("      </form>");
  client.println("    </section>");
  return;
}

/**
 * sendResponseBody()
 * 
 * Sends the body HTML if the SIM status has been changed
 */
void sendResponseBody(WiFiClient client, bool simStatus){
  client.println("      <p>Status of SIM40 is now: <strong>");
  if(simStatus)client.print("running");
  else client.print("halted");
  client.println("</strong></p>");
  client.println("      <p>Returning to main page shortly</p>");
  client.println("      <p>Or <a href=\"/\"><button>Return</button></a> manually</p>");
  return;
}

/**
 * sendTail()
 * 
 * Sends the final HTML to end the page for any web page
 */
void sendTail(WiFiClient client)
{
  // Send the final HTML
  client.println("  </body>");
  client.println("</html>");
  return;
}

String serviceWebRequest(WiFiClient client, String program, String memory, String registers, String videoOutput, bool simStatus)
{
    webCmd = "none";
    if(trace)Serial.print("Servicing new client: ");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        if(trace)Serial.write(c);           // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          if (currentLine.startsWith("Referer:"))Serial.println("Requesting "+ currentLine);
          if (currentLine.startsWith("GET /compile?program="))tidyProgram(currentLine);
//Serial.println(currentLine); // Trace; lets you see what's coming in via the GET parameter
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so break out & send a response:
          if (currentLine.length() == 0) {
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
        // Check to see what the client is requesting:
        if (currentLine.endsWith("GET /compile")) {
          Serial.println("\nStarting compilation");
          tidyProgram(currentLine);
          webCmd = "compile";
        }
        if (currentLine.endsWith("GET /run")) {
          Serial.println("\nBeginning program run");
          webCmd = "run";
        }
        if (currentLine.endsWith("GET /halt")) {
          Serial.println("\nTerminating program run");
          webCmd = "halt";
        }
        if (currentLine.endsWith("GET /clear")) {
          Serial.println("\nClearing output");
          webCmd = "clear";
        }
      }
    }
    // We need to send a response before closing the connection:

    // Send the headers, then the content of the HTTP response
    // If there's no button pressed, we want the default page
    if(webCmd == "none"){
     sendHead(client, simStatus, false); // Don't redirect
     sendBody(client, program, memory, registers, videoOutput, simStatus);
    }
    // But if a button's been pressed, we want to acknowledge the action, then redirect
    else{
     sendHead(client, simStatus, true); // Do redirect after 3 seconds
     sendResponseBody(client, simStatus);
    }
    // Now round off the HTML
    sendTail(client);
    // The HTTP response ends with another blank line:
    client.println();
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
    return webCmd;
}
