#define BLYNK_PRINT Serial // Defines the object that is used for printing
//#define BLYNK_DEBUG        // Optional, this enables more detailed prints

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <EEPROM.h>
//#include <SimpleTimer.h>

#define EEPROM_WIFI_VALID   0x1
#define SSID_LENGH(val)     ((val >> 1) 0x1F)

SimpleTimer connectionHandlerTimer;
struct board_info{
  char auth[40];
  char ssid[20];
  char pass[20];
  uint32_t ssidlen;
  uint32_t passlen;
  uint8_t update_wifi;
}g_bd;
// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "rVfX8DIkZvMjNfoPgAIiTS0TEu7CvUXr";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "SAYALI";
char pass[] = "sayali@123";

//Pin Declaration
const int LED = D0;

typedef enum {
  CONNECT_TO_WIFI,
  AWAIT_WIFI_CONNECTION,
  CONNECT_TO_BLYNK,
  AWAIT_BLYNK_CONNECTION,
  MAINTAIN_CONNECTIONS,
  AWAIT_DISCONNECT
} CONNECTION_STATE;

CONNECTION_STATE connectionState;
uint8_t connectionCounter;

void ConnectionHandler(void) {
  switch (connectionState) {
  case CONNECT_TO_WIFI:
    BLYNK_LOG("Connecting to %s.", ssid);
    WiFi.begin(ssid, pass);
    connectionState = AWAIT_WIFI_CONNECTION;
    connectionCounter = 0;
    break;

  case AWAIT_WIFI_CONNECTION:
    if (WiFi.status() == WL_CONNECTED) {
      BLYNK_LOG("Connected to %s", ssid);
      connectionState = CONNECT_TO_BLYNK;
    }
    else if (++connectionCounter == 50) {
      BLYNK_LOG("Unable to connect to %s. Retry connection.", ssid);
      WiFi.disconnect();
      connectionState = AWAIT_DISCONNECT;
      connectionCounter = 0;
    }
    break;

  case CONNECT_TO_BLYNK:
    BLYNK_LOG("Attempt to connect to Blynk server.");
    Blynk.config(auth);
    Blynk.connect();
    connectionState = AWAIT_BLYNK_CONNECTION;
    connectionCounter = 0;
    break;

  case AWAIT_BLYNK_CONNECTION:
    if (Blynk.connected()) {
      BLYNK_LOG("Connected to Blynk server.");
      connectionState = MAINTAIN_CONNECTIONS;
    }
    else if (++connectionCounter == 50) {
      BLYNK_LOG("Unable to connect to Blynk server. Retry connection.");
      Blynk.disconnect();
      WiFi.disconnect();
      connectionState = AWAIT_DISCONNECT;
      connectionCounter = 0;
    }
    break;

  case MAINTAIN_CONNECTIONS:
    if (WiFi.status() != WL_CONNECTED) {
      BLYNK_LOG("Wifi connection lost. Reconnect.");
      Blynk.disconnect();
      WiFi.disconnect();
      connectionState = AWAIT_DISCONNECT;
      connectionCounter = 0;
    }
    else  if (!Blynk.connected()) {
      BLYNK_LOG("Blynk server connection lost. Reconnect.");
      Blynk.disconnect();
      connectionState = CONNECT_TO_BLYNK;
    }
    else {
      Blynk.run();
    }
    break;

  case AWAIT_DISCONNECT:
    if (++connectionCounter == 10) {
      connectionState = CONNECT_TO_WIFI;
    }
    break;
  }
}

void setup()
{
  Serial.begin(9600);
  pinMode(LED, OUTPUT);
  EEPROM.begin(512);
  digitalWrite(LED, LOW);

  uint8_t ssid_lengh;
  uint16_t pre_prgm = GetEEPROMStatus();
  if(pre_pgm & EEPROM_WIFI_VALID){
    ssid_lengh = SSID_LENGH(pre_pgm);
    pass_lengh = PASS_LENGH(pre_pgm);
    GetEEPROMData(ssid_lengh, g_bd.ssid, SSID_EEPROM_START_ADDR);
    GetEEPROMData(ssid_lengh, g_bd.ssid, PASS_EEPROM_START_ADDR);
  }
    
  connectionHandlerTimer.setInterval(100, ConnectionHandler);
  connectionState = CONNECT_TO_WIFI;
}

void loop()
{
  connectionHandlerTimer.run();
}
