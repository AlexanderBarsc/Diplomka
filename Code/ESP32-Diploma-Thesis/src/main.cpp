#include <WiFi.h>
#include "Adafruit_HTU21DF.h"
#include "ESP32Setup.h"

boolean GetCredentialsFromApServer();

// Replace with your network credentials
const char* ssid     = "ESP32-Access-Point";
const char* password = "123456789";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;
String WifiSSID;
String WifiPassword;

void setup() {
  Serial.begin(115200);
  // Connect to Wi-Fi network with SSID and password
  Serial.print("Setting AP (Access Point)…");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  
  server.begin();
}

void loop()
{
  if(GetCredentialsFromApServer() == true)
  {

  }

}

boolean GetCredentialsFromApServer()
{
  WiFiClient client = server.available();  

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            client.println("<!DOCTYPE html><html>");
            client.println("<body><h1>ESP32 Web Server</h1>");
            client.println("<form action=\"/action_page.php\">");
            client.println("<label for=\"ssid\">SSID:</label>");
            client.println("<input type=\"text\" id=\"ssid\" name=\"ssid\"><br><br>");
            client.println("<label for=\"password\">Password:</label>");
            client.println("<input type=\"text\" id=\"password\" name=\"password\"><br><br>");
            client.println("<input type=\"submit\" value=\"Submit\">");
            client.println("</form>");
            client.println("</body></html>");
      
            client.println();
            break;
          } else { // if you got a newline, then clear currentLine

            if(currentLine.startsWith("GET"))
            {
              Serial.println("Hit");
            }
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }

    header = "";
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }

  return false;

}