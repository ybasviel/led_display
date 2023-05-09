#include <freertos/FreeRTOS.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>

#include "LedDisplay.h"
#include "debugMsg.h"


// wifi関連
#define SSID      "ssid"
#define PASSWORD  "password"
#define URL       "url"

// wifiタイムアウト時間 1/10s
#define TIMEOUTPERIOD  100

// テキスト問い合わせ周期 ms
#define UPDATEINTERVAL 5000

// Json Buffer
JSONVar doc;


// Led Display Driver
LedDisplay display;


// サーバー問い合わせ
void checkTextServer() {

  HTTPClient http;
  http.begin(URL);
  int httpCode = http.GET();

  Serial.printf("Response: %d", httpCode);
  Serial.println();

  if (httpCode == HTTP_CODE_OK) {
    String body = http.getString();
    Serial.print("Response Body: ");
    Serial.println(body);

    doc = JSON.parse(body);

    boolean isChanged = false;


    if ( display.content->len != (uint16_t)doc["mat_len"]) {
      display.content->len = (uint16_t)doc["mat_len"];
      isChanged = true;
    }

    for (uint16_t i = 0; i < display.content->len; i++) {
      if (display.content->data[i] != (uint8_t)doc["data"][i]) {
        display.content->data[i] = doc["data"][i];
        isChanged = true;
      }
    }

    //変更があればdisplayに反映
    if (isChanged == true) {
      Serial.println("send");
      display.send();
      isChanged = false;
    }
  }

}




void setup() {
  Serial.begin(115200);

  delay(1000);
  display.content = &searchWifi;
  display.send();
  delay(1000);

  WiFi.begin(SSID, PASSWORD);
  Serial.print("WiFi connecting");
  uint8_t timeout = 0;

  //display.content.data = (uint8_t*)searchWifi;
  //display.content.len = 148;
  
  
  while (1) {
    Serial.print(".");
    delay(100);
    timeout++;

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println(" connected");
      
      //display.content.data = (uint8_t*)onlineMode;
      //display.content.len = 84;
      display.content = &onlineMode;
      
      break;
    }
    else if (timeout >= TIMEOUTPERIOD) {
      Serial.println(" timeout! offline mode");
      
      //display.content.data = (uint8_t*)offlineMode;
      //display.content.len = 92;
      display.content = &offlineMode;

      break;
    }
  }

  display.send();
  delay(4500);
  display.content = &display.getContentBuffer;
}

void loop() {
  checkTextServer();
  delay(UPDATEINTERVAL);
}
