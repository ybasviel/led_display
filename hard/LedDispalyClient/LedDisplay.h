#pragma once

#include <Arduino.h>


//表示テキスト情報
typedef struct {
  uint16_t len;
  uint8_t* data;
} Content;



class LedDisplay {
public:
  LedDisplay();

  void send();

  void resetContent();

  Content* content;
  
  Content getContentBuffer = {
    0, data
  };

private:
  uint8_t data[1024];
};