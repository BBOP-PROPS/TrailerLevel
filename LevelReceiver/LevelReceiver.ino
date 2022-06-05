//
//    FILE: LevelReceiver.ino
//  AUTHOR: Mark Nierzwick
// VERSION: 0.0.0
// PURPOSE: minimal demo
//    DATE: 2021-03-28

//#define SERIAL_OUTPUT // used for debugging

#include "RH_ASK.h" // Radio Head Amplitude Shift Key
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Math.h>

#if defined(ARDUINO) && ARDUINO >= 100
#define printByte(args)  write(args);
#else
#define printByte(args)  print(args,BYTE);
#endif

RH_ASK receiver;

//#define DEBUG_COUNTER
#ifdef DEBUG_COUNTER
float debugCounter = 0.0f;
#endif

struct LevelData_t
{
  float angleX = 0.0f;
  float angleY = 0.0f;
  float angleZ = 0.0f;
  float temperature = 0.0f;
};

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 20 chars and 4 line display

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
  Serial.begin(115200);
  Serial.println(__FILE__);

  delay(500); // Let power settle before initializing
  
  lcd.init();                      // initialize the lcd 
  lcd.backlight();

  if (!receiver.init())
    Serial.println("init failed");

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

void loop()
{
  static bool noSignal = false;
  LevelData_t dataReceived;
  static unsigned long lastReceivedTime = 0;
  uint8_t buflen = sizeof(dataReceived);

  if (receiver.recv((uint8_t*)&dataReceived, &buflen))
  {
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
#ifdef DEBUG_COUNTER
    debugCounter += 1.0f;
    displayFloat(15, 2, debugCounter);
#endif
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
