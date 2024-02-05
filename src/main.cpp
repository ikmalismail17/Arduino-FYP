#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

// Define pin
const int trigger = 5;
const int echo = 18;
const char* serveraddress = "rivdepmonbackend.vercel.app";
const int serverport = 443;

// Define WiFi connection
#define WIFINAME "Walaoeh"
#define PASSWORD "567894321"

// Define sound speed in cm/uS
#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701

long duration;
float distanceCm;
float distanceInch;

// Function to connect to WiFi
void connectToWifi() {
  // Choosing the WiFi mode
  WiFi.mode(WIFI_STA);

  // Connecting WiFi using the WiFi name and password
  WiFi.begin(WIFINAME, PASSWORD);

  Serial.println("\nConnecting");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.println(".");
    delay(1000);
  }

  Serial.println("Successfully connected to " + String(WIFINAME) + " WiFi");
  Serial.print("WiFi IP is ");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200);
  connectToWifi();
  pinMode(trigger, OUTPUT);
  pinMode(echo, INPUT);
}

void loop() {
  digitalWrite(trigger, LOW);
  delayMicroseconds(2);
  digitalWrite(trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger, LOW);

  duration = pulseIn(echo, HIGH);

  distanceCm = duration * SOUND_SPEED / 2;
  distanceInch = distanceCm * CM_TO_INCH;

  Serial.print("Distance in cm: ");
  Serial.println(distanceCm);
  Serial.print("Distance in Inch: ");
  Serial.println(distanceInch);

  String data = "{\"distanceCm\": " + String(distanceCm) + ", \"distanceInch\": " + String(distanceInch) + "}";
  WiFiClientSecure client;

  client.setInsecure();
  if (client.connect(serveraddress, serverport)) {
    client.println("POST /arduinodata HTTP/1.1");
    client.println("Host: " + String(serveraddress));
    client.println("Content-Type: application/json");
    client.println("Content-Length: " + String(data.length()));
    client.println();
    client.println(data); // Your data

    // Read and print HTTP response status code
    while (client.connected()) {
      String line = client.readStringUntil('\n');
      if (line == "\r") {
        break;
      }
    }

    String statusLine = client.readStringUntil('\n');
    Serial.println("HTTP Status Code: " + statusLine);

    client.stop();
  }

  delay(10000);
}
