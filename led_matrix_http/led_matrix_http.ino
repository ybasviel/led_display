#include <freertos/FreeRTOS.h>
#include <SPI.h>
#include <EEPROM.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

//wifi関連
#define SSID      "ssid"
#define PASSWORD  "pass"
#define URL       "json source url"
//wifiタイムアウト時間 1/10s
#define TIMEOUTPERIOD  50

//eeprom関連
#define TEXTADDR    0x02
#define VERSIONADDR 0x01 //もう使ってないけど
#define LENADDR     0x00

//テキスト問い合わせ周期 ms
#define UPDATEINTERVAL 5000

//SPI関連
#define SCK   18
#define SS    5  
#define MOSI  23 

//表示テキスト情報
uint16_t len_mat;
uint8_t text[1024];

//json受け取るバッファー…?
StaticJsonDocument<4096> doc; 
//最大100文字*8バイト*3桁数字といろいろ足したらこのくらいバッファーが必要かな…
//文字間隔とかも考えたら最大100文字くらいにしとくのが安全かも

//LED表示
void display(void* arg) {
  uint16_t line_offset;
  uint8_t counter;

  while (1) {

    if (counter >= 63) {
      line_offset = (line_offset + 1) % len_mat;
      counter = 0;
    }
    counter++;

    for (uint8_t i = 0 ; i < 24 ; i++) {
      digitalWrite(SS, LOW);

      if (i < 8) //ここもっといい書き方ないかな
      {
        SPI.transfer( 1 << i );
        SPI.transfer(0);
        SPI.transfer(0);
      }
      else if (i < 16)
      {
        SPI.transfer(0);
        SPI.transfer( 1 << (i - 8) );
        SPI.transfer(0);
      }
      else {
        SPI.transfer(0);
        SPI.transfer(0);
        SPI.transfer( 1 << (i - 16) );
      }
      SPI.transfer( text[(i + line_offset) % len_mat ]);

      digitalWrite(SS, HIGH);
    }
  }
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

    DeserializationError error = deserializeJson(doc, body.c_str());
    // Test if parsing succeeds.
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }


    if ( len_mat != (int)doc["mat_len"]) {
      len_mat = (int)doc["mat_len"];
      EEPROM.write(LENADDR, len_mat);
    }

    for (uint16_t i = 0; i < len_mat; i++) {
      if (text[i] != doc["data"][i]) {
        text[i] = doc["data"][i];
        EEPROM.write(TEXTADDR + i, text[i]);
      }
    }
    EEPROM.commit();


  }

}


void setup() {
  Serial.begin(115200);


  WiFi.begin(SSID, PASSWORD);
  Serial.print("WiFi connecting");
  uint8_t timeout = 0;
  while (1) {
    Serial.print(".");
    delay(100);
    timeout++;

    if(WiFi.status() == WL_CONNECTED){
      Serial.println(" connected");
      break;
    }
    else if(timeout >= TIMEOUTPERIOD){
      Serial.println(" timeout! offline mode");
      break;
    }
  }


  pinMode(SCK, OUTPUT);
  pinMode(MOSI, OUTPUT);
  pinMode(SS, OUTPUT);
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV16);
  SPI.setDataMode(SPI_MODE0);
  digitalWrite(SS, LOW);


  delay(3000) ; // 3S後開始


  EEPROM.begin(1000);
  //EEPROMから前回の表示内容を読み込み
  len_mat = EEPROM.read(LENADDR);
  for (uint16_t i = 0; i < len_mat; i++) {
    text[i] = EEPROM.read(TEXTADDR + i);
  }


  xTaskCreatePinnedToCore(display, "Task_0", 4096, NULL, 1, NULL, 1);

}

void loop() {

  checkTextServer();
  delay(UPDATEINTERVAL);
}
