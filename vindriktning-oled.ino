#include <Wire.h>
#include "SSD1306Wire.h"

SSD1306Wire display(0x3c, SDA, SCL); 

#define BAUD 9600       
#define RXPIN 4
#define TXPIN 5

uint8_t serialRxBuf[80];
uint8_t rxBufIdx = 0;

int pm1 = 0;
int pm2 = 0;
int pm9 = 0;

unsigned long timepast = millis();

const int sendinterval = 500;

void setup() {
  display.init();
  display.flipScreenVertically();
  Serial1.begin(BAUD, SERIAL_8N1, RXPIN, TXPIN); 
}

void loop() {
  getSensorData();
  
  delay(500);

  display.clear();
  
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0, 0, "Particulate Matter");
  display.drawString(0, 16, "PM 1.0 ");
  display.drawString(0, 32, "PM 2.5 ");
  display.drawString(0, 48, "PM 10 ");

  display.setTextAlignment(TEXT_ALIGN_RIGHT);
  display.drawString(128, 0, "ug/m3");
  display.drawString(128, 16, String(pm1));
  display.drawString(128, 32, String(pm2));
  display.drawString(128, 48, String(pm9));


  display.display();
  delay(250);
}

int getSensorData() {
  uint8_t rxBufIdx = 0;
  uint8_t checksum = 0;

  // Sensor Serial aushorchen
  while ((Serial1.available() && rxBufIdx < 127) || rxBufIdx < 20) 
  {
    serialRxBuf[rxBufIdx++] = Serial1.read();
  }

  for (uint8_t i = 0; i < 20; i++) 
  {
    checksum += serialRxBuf[i];
  }

  // Header und Prüfsumme checken
  if (serialRxBuf[0] == 0x16 && serialRxBuf[1] == 0x11 && serialRxBuf[2] == 0x0B && checksum == 0)
    {
      pm2 = (serialRxBuf[5]  << 8 | serialRxBuf[6]);
      pm1 = (serialRxBuf[9]  << 8 | serialRxBuf[10]);
      pm9 = (serialRxBuf[13] << 8 | serialRxBuf[14]);
    }

  else
    {
      return (-1);
    }
}

