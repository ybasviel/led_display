#include "LedDisplay.h"

HardwareSerial uart(2);

LedDisplay::LedDisplay(){
  uart.begin(115200);
  this->content = &getContentBuffer;

  /* ここでeepromから前回のデータを読み込み */
}

void LedDisplay::send(){
  delay(100);
  Serial.print("[");
  uart.print("[");

  for (uint16_t i = 0; i < this->content->len; i++) {
    Serial.print(this->content->data[i]);
    Serial.print(",");

    uart.print(this->content->data[i]);
    uart.print(",");
  }
  Serial.print("]");
  uart.print("]");
}
