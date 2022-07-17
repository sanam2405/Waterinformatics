/*
  Flow of the code
  1 - Put WiFi in STA Mode
  2 - Initialise ESPNOW
  3 - Set Role to Combo
  4 - Add peer device
  5 - Define Send Callback Function
  6 - Define Receive Callback Function
*/

// Water-Flow sensor ID is 1
#include <Wire.h>
#ifdef ESP32
  #include <WiFi.h>
#else
  #include <ESP8266WiFi.h>
#endif
#include <espnow.h>

// REPLACE WITH THE MAC Address of your receiver
uint8_t broadcastAddress[] = {0xC8, 0xC9, 0xA3, 0x69, 0xDE, 0x1D};

#define SENSOR  D4
long currentMillis = 0;
long previousMillis = 0;
int interval = 1000;
float calibrationFactor = 4.5;
volatile byte pulseCount;
byte pulse1Sec = 0;
float flowRate;
unsigned int flowMilliLitres;
unsigned long totalMilliLitres;
void IRAM_ATTR pulseCounter()
{
  pulseCount++;
}

typedef struct struct_message {
  int id;
  char msg[50];
  unsigned long totalMillisFlown;
  float distanceInCm;
  float distanceInInch;
} struct_message;

// Create a struct_message called outgoingReadings to hold outgoing data
struct_message outgoingReadings;

// Create a struct_message called incomingReadings to hold incoming data
//struct_message incomingReadings;

// Variable to store if sending data was successful
String success;

// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("Last Packet Send Status: "); 
  if (sendStatus == 0) {
    Serial.println("Delivery success");
  }
  else {
    Serial.println("Delivery fail");
  }
}

// Callback when data is received
//void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
//  memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));
//  Serial.print("Bytes received: ");
//  Serial.println(len);
//  Serial.print("Data: "); Serial.println(incomingReadings.msg);
//}


void setup() {
  // Init Serial Monitor
  Serial.begin(115200);
  
  pinMode(SENSOR, INPUT_PULLUP);
  pulseCount = 0;
  flowRate = 0.0;
  flowMilliLitres = 0;
  totalMilliLitres = 0;
  previousMillis = 0;
  attachInterrupt(digitalPinToInterrupt(SENSOR), pulseCounter, FALLING);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initialising ESP-NOW");
    return;
  }

  // Set ESP-NOW Role
  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Transmitted packet
  esp_now_register_send_cb(OnDataSent);

  // Register peer
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_COMBO, 1, NULL, 0);

  

  // Register for a callback function that will be called when data is received
  // esp_now_register_recv_cb(OnDataRecv);
}

void loop() {

  currentMillis = millis();
  if (currentMillis - previousMillis > interval) {
    pulse1Sec = pulseCount;
    pulseCount = 0;
    flowRate = ((1000.0 / (millis() - previousMillis)) * pulse1Sec) / calibrationFactor;
    previousMillis = millis();
    flowMilliLitres = (flowRate / 60) * 1000;
    totalMilliLitres += flowMilliLitres;

  strcpy(outgoingReadings.msg, "Hello from Side NodeMCU wih WaterFlow Sensor");
  outgoingReadings.totalMillisFlown = totalMilliLitres;
  outgoingReadings.id = 1;
    
  esp_now_send(broadcastAddress, (uint8_t *) &outgoingReadings, sizeof(outgoingReadings));

  delay(2000);
  }
}
