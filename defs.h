#define BLYNK_PRINT Serial // Defines the object that is used for printing
//#define BLYNK_DEBUG        // Optional, this enables more detailed prints

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <EEPROM.h>
//#include <SimpleTimer.h>

extern void stringCopy(const char * src,char * dest);

WidgetLED wifi(V4);
WidgetLED blynk(V3);

#define EEPROM_WIFI_VALID         0x1
#define SSID_LENGH(val)           ((val >> 1) & 0x1F)
#define PASS_LENGH(val)           ((val >> 6) & 0x1F)

#define EEPROM_HEADER0            0x0
#define EEPROM_HEADER1            0x1
#define SSID_EEPROM_START_ADDR    0x2
#define PASS_EEPROM_START_ADDR    0x16

SimpleTimer connectionHandlerTimer;
class board_info {
public:
  char auth[40];
  char ssid[20];
  char pass[20];
  uint32_t ssidlen;
  uint32_t passlen;
  uint8_t update_wifi;

  board_info(){
    stringCopy("rVfX8DIkZvMjNfoPgAIiTS0TEu7CvUXr",auth);
    stringCopy("SAYALI",ssid);
    stringCopy("sayali@123",pass);
    
  }
} g_bd;
// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "rVfX8DIkZvMjNfoPgAIiTS0TEu7CvUXr";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "SAYALI";
char pass[] = "sayali@123";

//
uint8_t wifi_connected = 0;

//Pin Declaration
const int LED = D0;

typedef enum {
  CONNECT_TO_WIFI,
  AWAIT_WIFI_CONNECTION,
  CONNECT_TO_BLYNK,
  AWAIT_BLYNK_CONNECTION,
  MAINTAIN_CONNECTIONS,
  AWAIT_DISCONNECT,
  USER_WIFI_UPDATE_REQ
} CONNECTION_STATE;

CONNECTION_STATE connectionState;
uint8_t connectionCounter;
uint8_t reboot_counter;
