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
  data = EEPROM.read(0x0);
  data |= ((EEPROM.read(0x1)) << 0x8);
  return data;
}

void GetEEPROMData(uint8_t numbytes, uint32_t* write_back_ptr, uint8_t address){
  for(uint8_t index = 0; index < numbytes; index++){
    write_back_ptr[index] = EEPROM.read(address + index);
  }
  write_back_ptr[index] = '\0';
}
