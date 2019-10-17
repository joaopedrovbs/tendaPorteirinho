#include <WiFi.h>
#include <ESPmDNS.h>

#define MOSFET_PORT   15

const char* ssid     = "changeHereForYours";
const char* password = "changeHereForYours";

// Create a Wifi Server on port 80
WiFiServer server(80);


void setup() {
  //Start Serial connection for debugging
  Serial.begin(115200);
  //Declare pin as OUTPUT
  pinMode(MOSFET_PORT, OUTPUT);
  delay(10);

  // Display name of WiFi connecting to
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  // Begin a wifi connection with the following ssid and password
  WiFi.begin(ssid, password);

  // While trying to connect print dots on Serial and wait
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }

  // If successfully connected display the IP Address
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
  // Start a MDNS server so the user can access at http://porta.local
  if (!MDNS.begin("porta")) {
      Serial.println("Error setting up MDNS responder!");
      while(1) {
          delay(1000);
      }
  }
  
  // Begin the WebServer
  Serial.println("Server started");
  server.begin();
  // Begin the MDNS server
  Serial.println("mDNS responder started");
  MDNS.addService("http", "tcp", 80);
}

void loop() {
  // Always check for client events
  WiFiClient client = server.available();

  // If has a client proceed
  if (client) {
    Serial.println("New Client.");

    // String to store client request
    String currentLine = "";

    // Only do the work while the client is connected
    while (client.connected()) {
      if (client.available()) {
        // Read the received request and print it through Serial 
        char c = client.read();
        Serial.write(c);
        // If the request ended 
        if (c == '\n') {
          if (currentLine.length() == 0) {
            // Stream to the client the http content
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
            // the content of the HTTP response follows the header:
            client.print("<h2>Opa,</h2><br>");
            client.print("<img src=\"http://oportunidade24horas.com/wp-content/uploads/2018/11/porteiro-manaus.jpg\"><br>");
            client.print("<a href=\"/open\"><h1 >CLICA AQUI PRA ABRIR</h1></a><br>");
            
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
        // Check to see if the client made a request to /open
        if (currentLine.endsWith("GET /open")) {
          // to keep indepotence the code runs pulse after getting the request
          digitalWrite(MOSFET_PORT, HIGH);
          delay(1000);
          digitalWrite(MOSFET_PORT, LOW);
        } 
      }
    }
    // Stop the connection after the while
    client.stop();
    Serial.println("Client Disconnected.");
  }
}
