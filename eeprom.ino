/*
 * EEPROM NODEMCU HEADER
 * 
 * EEPROM address 0x0=
 * ________________________________________________________________________________________________
 *    7     |     6    |      5     |     4     |     3     |     2     |       1     |     0     |
 * ------------------------------------------------------------------------------------------------
 * pass_len1|pass_len0 |  ssid_len4 | ssid_len3 | ssid_len2 | ssid_len1 | ssid_len0   | WiFi_cofg |
 * ------------------------------------------------------------------------------------------------
 * 
 * EEPROM address 0x1=
 * ________________________________________________________________________________________________
 *    7     |     6    |      5     |     4     |     3     |     2     |       1     |     0     |
 * ------------------------------------------------------------------------------------------------
 *   SW4    |    SW3   |     SW2    |    SW1    |    SW0    | pass_len4 | pass_len3   | pass_len2 |
 * ------------------------------------------------------------------------------------------------
 * 
 * 
 */

uint16_t GetEEPROMStatus(void){
  uint16_t data = 0;
  data = EEPROM.read(EEPROM_HEADER0);
  data |= ((EEPROM.read(EEPROM_HEADER1)) << 0x8);
  return data;
}

void DisEEPROMWifiFlg(void){
  char data[1];
  GetEEPROMData(1, data, EEPROM_HEADER0);
  data[0] &= ~(0x1);
  PutEEPROMData(1, data, EEPROM_HEADER0);
  delay(10);
}


void GetEEPROMData(uint8_t numbytes, char* write_back_ptr, uint8_t address){
  uint8_t index;
  for(index = 0; index < numbytes; index++){
    write_back_ptr[index] = EEPROM.read(address + index);
    delay(10);
  }
  write_back_ptr[index] = '\0';
  Serial.print("Write back ptr = ");
  Serial.println(write_back_ptr);
  EEPROM.commit();
}

void PutEEPROMData(uint8_t numbytes, char * write_ptr, uint8_t address){
  uint8_t index;
  for(index = 0; index < numbytes; index++){
    EEPROM.write(address + index, write_ptr[index]);
    delay(10);
  }
  EEPROM.commit();
}

void UpdateEEPROM(void){
  if(g_bd.update_wifi){
    blynk.off();
    wifi.off();
    uint8_t ssid_lengh, pass_lengh;
    uint32_t prg_header;
    char header_arr[2] = "";
  
    ssid_lengh = stringLength(g_bd.ssid);
    pass_lengh = stringLength(g_bd.pass);
    prg_header = pass_lengh;
    prg_header = (prg_header << 6) | (ssid_lengh << 1) | (EEPROM_WIFI_VALID);
    
    header_arr[0] = (uint8_t)(prg_header & 0xFF);
    header_arr[1] = (uint8_t)((prg_header >> 8) & 0xFF);
    Serial.print("USSID lengh = ");
    Serial.println(ssid_lengh);
    Serial.print("UPASS lengh = ");
    Serial.println(pass_lengh);
    Serial.print("Header0 = ");
    Serial.println(header_arr[0], HEX);
    Serial.print("Header1 = ");
    Serial.println(header_arr[1], HEX);
    PutEEPROMData(ssid_lengh, g_bd.ssid, SSID_EEPROM_START_ADDR);
    PutEEPROMData(pass_lengh, g_bd.pass, PASS_EEPROM_START_ADDR);
    PutEEPROMData(2, header_arr, EEPROM_HEADER0);
    connectionState = USER_WIFI_UPDATE_REQ;
    EEPROM.commit();
  }
}
