//
//    FILE: LevelReceiver.ino
//  AUTHOR: Mark Nierzwick
// VERSION: 0.0.0
// PURPOSE: minimal demo
//    DATE: 2021-03-28

//#define SERIAL_OUTPUT // used for debugging

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Math.h>

#if defined(ARDUINO) && ARDUINO >= 100
#define printByte(args)  write(args);
#else
#define printByte(args)  print(args,BYTE);
#endif

#include <Arduino.h>
#include <cc1101.h>
#include <ccpacket.h>
// Attach CC1101 pins to their corresponding SPI pins
// Uno pins:
// CSN (SS) => 10
// MOSI => 11
// MISO => 12
// SCK => 13
// GD0 => A valid interrupt pin for your platform (defined below this)
#if defined(__AVR_ATmega2560__) || defined(__AVR_ATmega1280__)
#define CC1101Interrupt 4 // Pin 19
#define CC1101_GDO0 19
#elif defined(__MK64FX512__)
// Teensy 3.5
#define CC1101Interrupt 9 // Pin 9
#define CC1101_GDO0 9
#else
#define CC1101Interrupt 0 // Pin 2
#define CC1101_GDO0 2
#endif
#define CC1101_DEFVAL_MDMCFG4_1200    0xC5   // Modem configuration. Speed = 1200 bps

CC1101 radio;
byte syncWord[2] = {199, 10};
bool packetWaiting;
void messageReceived() {
  packetWaiting = true;
}

struct LevelData_t
{
  float angleX = 0.0f;
  float angleY = 0.0f;
  float angleZ = 0.0f;
  float temperature = 0.0f;
};

LiquidCrystal_I2C lcd(0x27, 20, 4); // set the LCD address to 0x27 for a 20 chars and 4 line display

void SplashScreen()
{
  uint8_t logo1[8] = {0x00, 0x00, 0x03, 0x06, 0x00, 0x0F, 0x18, 0x00};
  uint8_t logo2[8] = {0x00, 0x00, 0x1F, 0x00, 0x00, 0x1F, 0x01, 0x01};
  uint8_t logo3[8] = {0x00, 0x00, 0x1C, 0x06, 0x12, 0x02, 0x02, 0x04};
  uint8_t logo4[8] = {0x00, 0x00, 0x00, 0x01, 0x03, 0x00, 0x0F, 0x18};
  uint8_t logo5[8] = {0x07, 0x0C, 0x00, 0x1F, 0x00, 0x00, 0x1F, 0x00};
  uint8_t logo6[8] = {0x1F, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x1F, 0x00};
  uint8_t logo7[8] = {0x1F, 0x01, 0x0D, 0x11, 0x11, 0x0A, 0x04, 0x00};
  uint8_t logo8[8] = {0x18, 0x06, 0x01, 0x01, 0x01, 0x16, 0x18, 0x00};
  lcd.createChar(0, logo1);
  lcd.createChar(1, logo2);
  lcd.createChar(2, logo3);
  lcd.createChar(3, logo4);
  lcd.createChar(4, logo5);
  lcd.createChar(5, logo6);
  lcd.createChar(6, logo7);
  lcd.createChar(7, logo8);
  lcd.clear();
  lcd.home();
  lcd.setCursor(2, 0);
  lcd.print("Brownsburg Bands");
  lcd.setCursor(9, 1);
  for (int i = 0; i < 3; i++)
    lcd.printByte(i);
  lcd.setCursor(7, 2);
  for (int i = 3; i < 8; i++)
    lcd.printByte(i);
  lcd.setCursor(0, 3);
  lcd.print("Level Utility     MN");
}

void setup()
{
#ifdef SERIAL_OUTPUT
  Serial.begin(115200);
  Serial.println(__FILE__);
#endif
  radio.init();
  radio.setSyncWord(syncWord);
  radio.setCarrierFreq(CFREQ_433);
  radio.disableAddressCheck();
  radio.setTxPowerAmp(PA_LongDistance);
  radio.writeReg(CC1101_MDMCFG4, CC1101_DEFVAL_MDMCFG4_1200); // Set the bit rate to 1200 for better receiver sensitivity
  attachInterrupt(CC1101Interrupt, messageReceived, FALLING);

  lcd.init();                      // initialize the lcd
  lcd.backlight();
  SplashScreen();
  delay(5000);
}

void displayFloat(uint8_t row, uint8_t col, float x)
{
  char xstr[6];
  dtostrf(x, 3, 1, xstr);
  lcd.setCursor(row, col);
  lcd.print("     ");
  lcd.setCursor(row, col);
  lcd.print(xstr);
}

// Get signal strength indicator in dBm.
// See: http://www.ti.com/lit/an/swra114d/swra114d.pdf
int rssi(char raw) {
  uint8_t rssi_dec;
  // TODO: This rssi_offset is dependent on baud and MHz; this is for 38.4kbps and 433 MHz.
  uint8_t rssi_offset = 74;
  rssi_dec = (uint8_t) raw;
  if (rssi_dec >= 128)
    return ((int)( rssi_dec - 256) / 2) - rssi_offset;
  else
    return (rssi_dec / 2) - rssi_offset;
}

// Get link quality indicator.
int lqi(char raw) {
  return 0x3F - raw;
}

void loop()
{
  static bool noSignal = false;
  LevelData_t dataReceived;
  static unsigned long lastReceivedTime = 0;
  uint8_t buflen = sizeof(dataReceived);

  if (packetWaiting)
  {
    detachInterrupt(CC1101Interrupt);
    packetWaiting = false;
    CCPACKET packet;
    if (radio.receiveData(&packet) > 0)
    {
      if (packet.crc_ok)
      {
        memcpy((char*)&dataReceived, (char*)&packet.data, max(packet.length, sizeof(dataReceived)));
        lastReceivedTime = millis();
        lcd.setCursor(0, 2);
        lcd.print("Angle");
        if (noSignal)
        {
          noSignal = false;
          lcd.setCursor(0, 3);
          lcd.print("                    ");
        }
        displayFloat(0, 3, dataReceived.angleX);
#ifdef SERIAL_OUTPUT
        Serial.println(dataReceived.angleX);
#endif
        float angleRadians = dataReceived.angleX * 2.0 * PI / 360.0;
        float adjustment = 102 * sin(angleRadians);
        lcd.setCursor(15, 2);
        if (adjustment > 0)
          lcd.print("Left ");
        else
          lcd.print("Right");
        displayFloat(15, 3, fabsf(adjustment));
      }
#ifdef SERIAL_OUTPUT
      Serial.println(F("Received packet..."));
      if (!packet.crc_ok)
      {
        Serial.println(F("crc not ok"));
      }
      Serial.print(F("lqi: "));
      Serial.println(lqi(packet.lqi));
      Serial.print(F("rssi: "));
      Serial.print(rssi(packet.rssi));
      Serial.println(F("dBm"));

      if (packet.crc_ok && packet.length > 0)
      {
        Serial.print(F("packet: len "));
        Serial.println(packet.length);
        Serial.println(F("data: "));
        Serial.println((const char *) packet.data);
      }
#endif
    }
    attachInterrupt(CC1101Interrupt, messageReceived, FALLING);
  }
  else
  {
    if (((millis() - lastReceivedTime) > 3000) && !noSignal)
    {
#ifdef SERIAL_OUTPUT
      Serial.println((millis() - lastReceivedTime));
#endif
      lcd.setCursor(0, 3);
      lcd.print("No Signal           ");
      noSignal = true;
    }
  }
}
