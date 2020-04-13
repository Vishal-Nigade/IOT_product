#include "defs.h"

void ConnectionHandler(void) {
  switch (connectionState) {
  case CONNECT_TO_WIFI:
    BLYNK_LOG("Connecting to %s.", g_bd.ssid);
    WiFi.begin(g_bd.ssid, g_bd.pass);
    connectionState = AWAIT_WIFI_CONNECTION;
    connectionCounter = 0;
    break;

  case AWAIT_WIFI_CONNECTION:
    if (WiFi.status() == WL_CONNECTED) {
      BLYNK_LOG("Connected to %s", g_bd.ssid);
      connectionState = CONNECT_TO_BLYNK;
    }
    else if (++connectionCounter == 50) {
      wifi.off();
      BLYNK_LOG("Unable to connect to %s. Retry connection.", g_bd.ssid);
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
      reboot_counter = 0;
      wifi.on();
      blynk.on();
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
      wifi.off();
      BLYNK_LOG("Wifi connection lost. Reconnect.");
      Blynk.disconnect();
      WiFi.disconnect();
      connectionState = AWAIT_DISCONNECT;
      connectionCounter = 0;
    }
    else  if (!Blynk.connected()) {
      blynk.off();
      BLYNK_LOG("Blynk server connection lost. Reconnect.");
      Blynk.disconnect();
      connectionState = CONNECT_TO_BLYNK;
    }
    else {
      Blynk.run();
      UpdateEEPROM();
    }
    break;

  case AWAIT_DISCONNECT:
    if (++connectionCounter == 10) {
      connectionState = CONNECT_TO_WIFI;
      ++reboot_counter;
    }
    if(reboot_counter == 10){
      DisEEPROMWifiFlg();
      Serial.println("Rebooring ESP");
      ESP.restart();
      while(1);
    }
    Serial.println("count = ");
    Serial.println(connectionCounter, DEC);
    Serial.println(reboot_counter, DEC);
    break;
  case USER_WIFI_UPDATE_REQ:
    Blynk.disconnect();
    WiFi.disconnect();
    connectionState = AWAIT_DISCONNECT;
    connectionCounter = 0;
    g_bd.update_wifi = 0;
    break;
  }
}



void setup()
{
  Serial.begin(9600);
  pinMode(LED, OUTPUT);
  EEPROM.begin(512);
  digitalWrite(LED, HIGH);

  uint8_t ssid_lengh, pass_lengh;
  uint32_t pre_pgm;
  pre_pgm = GetEEPROMStatus();
  if(pre_pgm & EEPROM_WIFI_VALID){
    ssid_lengh = SSID_LENGH(pre_pgm);
    pass_lengh = PASS_LENGH(pre_pgm);
    Serial.print("SSID lengh = ");
    Serial.println(ssid_lengh);
    Serial.print("PASS lengh = ");
    Serial.println(pass_lengh);
    GetEEPROMData(ssid_lengh, g_bd.ssid, SSID_EEPROM_START_ADDR);
    GetEEPROMData(pass_lengh, g_bd.pass, PASS_EEPROM_START_ADDR);
  }
  
  connectionHandlerTimer.setInterval(100, ConnectionHandler);
  connectionState = CONNECT_TO_WIFI;
}

void loop()
{
  connectionHandlerTimer.run();
}
