#include <ESP8266WiFi.h>

#define BUTTON 4

const char* ssid = "SSID";
const char* password = "SSID_PASSWORD";

int buttonState = 0;
int relayPin = 2; // GPIO13
int ledPin = 5; // status led pin
WiFiServer server(80);

IPAddress ip(192, 168, 0, 111); 
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);

int stat = 0;
// timer limit in seconds
int timerSeconds = 60 * 60;
int timer = 0;
 
void setup() {
  Serial.begin(115200);
  delay(5);

  WiFi.mode(WIFI_STA);
  WiFi.config(ip, gateway, subnet);

  pinMode(ledPin, OUTPUT);
  
 
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH);

  pinMode(BUTTON, INPUT_PULLUP);
 
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
 
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
 
  // Start the server
  server.begin();
  Serial.println("Server started");
 
  // Print the IP address
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
 
}


int value = LOW;
void loop() {
  
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("disconnected amk");

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
  }
  
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    buttonState = digitalRead(BUTTON);
      
    if (buttonState == 0) {
      delay(1000);
      Serial.println("pressed");
      if (stat == 1) {
        value = HIGH;
        timer = 0;
        stat = 0;
        digitalWrite(relayPin, HIGH);
        digitalWrite(ledPin, LOW);
      } else {
        value = HIGH;
        stat = 1;
        digitalWrite(relayPin, LOW);
        digitalWrite(ledPin, HIGH);
      }
      return;
    } else {
      
      if (stat == 1 && timer < timerSeconds) {
        Serial.println(timer);
        timer++;
        delay(1000);
      }
      if (timer >= timerSeconds) {
        Serial.println("turned off by timer");
        digitalWrite(relayPin, HIGH);
        digitalWrite(ledPin, LOW);
        value = HIGH;
        timer = 0;
        stat = 0;
      }

    }
    
    return;
  }
 
  // Wait until the client sends some data
  Serial.println("new client");
  while(!client.available()){
    delay(1);
  }
 
  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();
 
  // Return the response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/plain");
  client.println(""); //  do not forget this one
  
  if (request.indexOf("power=0") != -1)  {
    digitalWrite(relayPin, HIGH);
    digitalWrite(ledPin, LOW);
    value = HIGH;
    stat = 0;
    timer = 0;
    client.print("Off");
  }
  if (request.indexOf("power=1") != -1)  {
    digitalWrite(relayPin, LOW);
    digitalWrite(ledPin, HIGH);
    value = LOW;
    stat = 1;
    client.print("On");
  }
  if (request.indexOf("getState") != -1)  {
    if (stat == 1) {
      client.print("On");
    } else {
      client.print("Off");
    }
  }
  if (request.indexOf("getTimer") != -1)  {
    client.print(timerSeconds - timer);
  }
  
 
  delay(1);
  Serial.println("Client disonnected");
  Serial.println("");
 
}


