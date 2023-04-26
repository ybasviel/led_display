#include <freertos/FreeRTOS.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>

HardwareSerial uart(2);

//wifi関連
#define SSID      "ssid"
#define PASSWORD  "password"
#define URL       "url"

//wifiタイムアウト時間 1/10s
#define TIMEOUTPERIOD  50

//テキスト問い合わせ周期 ms
#define UPDATEINTERVAL 5000


//表示テキスト情報
uint16_t len_mat;
uint8_t text[1024];

JSONVar doc;


void display() {
  delay(100);
  uart.print("[");
  for (uint16_t i = 0; i < len_mat; i++) {
    uart.print(text[i]);
    uart.print(",");
  }
  uart.print("]");
}


//サーバー問い合わせ
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


    if ( len_mat != (int)doc["mat_len"]) {
      len_mat = (int)doc["mat_len"];
      isChanged = true;
    }

    for (uint16_t i = 0; i < len_mat; i++) {
      if (text[i] != (int)doc["data"][i]) {
        text[i] = doc["data"][i];
        isChanged = true;
      }
    }

    //変更があればdisplayに反映
    if (isChanged == true) {
      Serial.println("send");
      display();
      isChanged = false;
    }
  }

}




void setup() {
  Serial.begin(115200);
  uart.begin(115200);


  WiFi.begin(SSID, PASSWORD);
  Serial.print("WiFi connecting");
  uint8_t timeout = 0;
  while (1) {
    Serial.print(".");
    delay(100);
    timeout++;

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println(" connected");
      break;
    }
    else if (timeout >= TIMEOUTPERIOD) {
      Serial.println(" timeout! offline mode");
      break;
    }
  }

  delay(3000) ;
}

void loop() {

  checkTextServer();
  delay(UPDATEINTERVAL);
}
