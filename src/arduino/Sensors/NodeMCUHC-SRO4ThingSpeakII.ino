/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp8266-nodemcu-hc-sr04-ultrasonic-arduino/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/

#ifdef ESP32
  #include <WiFi.h>
#else
  #include <ESP8266WiFi.h>
#endif

#include <ThingSpeak.h>

WiFiClient client;

long myChannelNumber = 1764130;
const char myWriteAPIKey[] = "7F1U0PTLV5PNO5E2";

const char wifiName[] = "Manaspb";
const char password[] = "manas123";
const char ip[] = "184.106.153.149";

const int trigPin = 12;
const int echoPin = 14;

//define sound velocity in cm/uS
#define SOUND_VELOCITY 0.034
#define CM_TO_INCH 0.393701

long duration;
float distanceCm;
float distanceInch;

void setup() {
  Serial.begin(115200); // Starts the serial communication

  WiFi.begin(wifiName,password);
  while(WiFi.status() != WL_CONNECTED)
  {
      delay(200);
      Serial.print("..");
  }

  Serial.println();
  Serial.println("NodeMCU is connected:");
  Serial.println(WiFi.localIP());

  ThingSpeak.begin(client);
  
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
}

void loop() {
  
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
   
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  
  // Calculate the distance
  distanceCm = duration * SOUND_VELOCITY/2;
  
  // Convert to inches
  distanceInch = distanceCm * CM_TO_INCH;
  
  // Prints the distance on the Serial Monitor
  Serial.print("Distance (cm): ");
  Serial.println(distanceCm);
  Serial.print("Distance (inch): ");
  Serial.println(distanceInch);

  if(client.connect(ip,80)){
  ThingSpeak.setField(1,distanceCm);
  ThingSpeak.writeFields(myChannelNumber,myWriteAPIKey);
  client.stop();
  delay(3000);
  }
}
