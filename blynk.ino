BLYNK_WRITE (D0)
{
  int value = param.asInt();
  if (value == 1) {    // SW0
    digitalWrite(LED, HIGH);
  }else{
    digitalWrite(LED, LOW);
  }
}

BLYNK_WRITE (V0)
{
  String str = param.asStr();
  uint8_t lengh = str.length();
  int index;
  for(index =0;lengh;lengh--,index++){
    g_bd.ssid[index] = str.charAt(index);
  }
  g_bd.ssid[index] = '\0';
  Serial.print("SSID: " );
  Serial.println(g_bd.ssid);
}

BLYNK_WRITE (V1)
{
 String str = param.asStr();
  uint8_t lengh = str.length();
  int index;
  for(index =0;lengh;lengh--,index++){
    g_bd.pass[index] = str.charAt(index);
  }
  g_bd.pass[index] = '\0';
  Serial.print("PASS: " );
  Serial.println(g_bd.pass);
}

BLYNK_WRITE (V2)
{
  int value = param.asInt();
  g_bd.update_wifi = true; 
}

BLYNK_WRITE (V6)
{
  uint8_t led_val = param.asInt();
  Serial.println(led_val);
  digitalWrite(LED,led_val);
}
