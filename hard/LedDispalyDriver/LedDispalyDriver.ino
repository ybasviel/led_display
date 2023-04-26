#include <SPI.h>

#define UART_INT 5


#define RCV_BUFF_LEN 1024


//表示テキスト情報
volatile uint16_t len_mat;
volatile uint8_t text[RCV_BUFF_LEN];

uint16_t line_offset;
uint8_t counter;


void setup() {
  Serial.begin(115200);

  attachInterrupt(UART_INT, uart_int, LOW);

  pinMode(UART_INT, INPUT_PULLUP);
}

void loop() {


  line_offset = (line_offset + 1) % len_mat;

  for (uint8_t i = 0 ; i < 24 ; i++) {
    digitalWrite(SS, LOW);
    delay(2);

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

void uart_int(){
  len_mat = 0;
  
  while ( digitalRead(UART_INT) == LOW ) {

    if (Serial.available()) {
      text[len_mat] = Serial.read();

      if (len_mat >= RCV_BUFF_LEN) {
        while( digitalRead(UART_INT) == LOW ){}
        break;
      } else {
        len_mat++;
      }
    }
  }
}
