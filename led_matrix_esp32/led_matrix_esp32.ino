#include <SPI.h>
#include <EEPROM.h>


#define TEXTADDR 0x01
#define LENADDR 0x00

volatile uint16_t len_mat;
volatile uint16_t line_offset;
uint8_t rcvIdx;
uint16_t txtIdx;
uint8_t rcvBuff[4];
uint8_t text[1024];

#define sck  18 // CLK
#define SS   5  // CE
#define sdi  23 // MOSI

uint8_t counter;

void print_mat(uint8_t *line, uint16_t n) {

  //line_offsetをずらして横に流す。timerを使うとserialに影響が出るっぽいので
  if(counter >= 63){
    line_offset = (line_offset+1) % len_mat;
    counter = 0;
  }
  counter++;
  
  for (uint8_t i = 0 ; i < 24 ; i++) {
    digitalWrite(SS, LOW);

    if (i < 8)
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
    SPI.transfer( line[(i + line_offset) % n ]);

    digitalWrite(SS, HIGH);
  }
}



void setup() {
  //いろいろリセット
  txtIdx = 0;
  rcvIdx = 0;
  counter = 0;
  
  //SPI.setDataMode(SPI_MODE3);
  //SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE3));
  pinMode(sck, OUTPUT);
  pinMode(sdi, OUTPUT);
  pinMode(SS, OUTPUT);
  SPI.begin() ;                         // ＳＰＩを行う為の初期化
  SPI.setBitOrder(MSBFIRST) ;           // ビットオーダー
  SPI.setClockDivider(SPI_CLOCK_DIV16) ;// クロックをシステムクロックの1/16で使用(16MHz/16)
  SPI.setDataMode(SPI_MODE0) ;          // クロック極性０(LOW)　クロック位相０(LOW)
  digitalWrite(SS, LOW) ;               // SS(CS)ラインをLOWにする

  delay(3000) ; // 3S後開始

  Serial.begin(115200);

  EEPROM.begin(1000);
  //EEPROMから前回の表示内容を読み込み
  len_mat = EEPROM.read(LENADDR);
  for (uint16_t i = 0; i < len_mat; i++) {
    text[i] = EEPROM.read(TEXTADDR + i);
  }

}

void loop() {

  //Serial処理
  while (Serial.available() > 0) {
    uint8_t Buff = Serial.read();

    if (Buff == ']') {
      rcvBuff[rcvIdx] = '\0';
      rcvIdx = 0;

      text[txtIdx++] = atoi( (char *)rcvBuff );


      len_mat = txtIdx;
      txtIdx = 0;


      EEPROM.write(LENADDR, len_mat);
      for (uint16_t i = 0; i < len_mat; i++) {
        EEPROM.write(TEXTADDR + i, text[i]);
      }
      EEPROM.commit();

      counter = 0;
    }
    else if (Buff == ',') {
      rcvBuff[rcvIdx] = '\0';
      rcvIdx = 0;

      text[txtIdx++] = atoi( (char *)rcvBuff );
    }
    else if (Buff == '[' || Buff == '\n' || Buff == '\r') ;
    else {
      rcvBuff[rcvIdx++] = Buff;
    }
  }


  print_mat((uint8_t *)text, len_mat);

}
