#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>

//define pin
const int trigger = 5;
const int echo = 18;
const char* serveraddress = "192.168.0.17";
const int serverport = 3000;

//define wifi connection
#define WIFINAME "Jimboi"
#define PASSWORD "zakwanobey"

//define sound speed in cm/uS
#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701

long duration;
float distanceCm;
float distanceInch;

//funtion to connect to wifi
void connectToWifi(){

  //choosing the wifi mode
  WiFi.mode(WIFI_STA);

  //connecting wifi using the wifiname and password
  WiFi.begin(WIFINAME, PASSWORD);

  Serial.println("\nConnecting");

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.println(".");
    delayMicroseconds(1000);

  }

  Serial.println("Successfully connected to "+String(WIFINAME)+" Wifi");
  Serial.print("Wifi IP is ");
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

  distanceCm = duration * SOUND_SPEED/2;
  
  distanceInch = distanceCm * CM_TO_INCH;

  Serial.print("Distance in cm: ");
  Serial.println(distanceCm);
  Serial.print("Distance in Inch: ");
  Serial.println(distanceInch);


  String data = "{\"distanceCm\": " + String(distanceCm) + ", \"distanceInch\": " + String(distanceInch) + "}";
  WiFiClient client;
  if (client.connect(serveraddress, serverport)) {
    client.println("POST /arduinodata HTTP/1.1");
    client.println("Host: " + String(serveraddress));
    client.println("Content-Type: application/json");
    client.println("Content-Length: " + String(data.length()));
    client.println();
    client.println(data); // Your data
    client.stop();
  }

  delay(10000);
}