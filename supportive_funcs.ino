uint8_t stringLength(char * str){
  uint8_t str_len = 0;
  for(;*str;str++,str_len++);
  return str_len;
}

void stringCopy(const char * src,char * dest){
  
  for(;*src;src++,dest++){
    *dest = *src;
    
  }
  *dest = *src;
}
