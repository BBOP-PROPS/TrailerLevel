//
//    FILE: LevelSender.ino
//  AUTHOR: Mark Nierzwick
// VERSION: 0.0.0
// PURPOSE: minimal demo
//    DATE: 2021-03-28

//#define SERIAL_OUTPUT // used for debugging

#include "GY521.h" // 3-axis accelerometer and gyro

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

GY521 sensor(0x68);

uint32_t counter = 0;

struct LevelData_t
{
  float angleX = 0.0f;
  float angleY = 0.0f;
  float angleZ = 0.0f;
  float temperature = 0.0f;
};
LevelData_t dataToSend;

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
  
  Wire.begin();
  while (sensor.wakeup() == false)
  {
#ifdef SERIAL_OUTPUT
    Serial.println("\tCould not connect to GY521");
#endif
    delay(1000);
  }

  sensor.setAccelSensitivity(0);  // 2g
  sensor.setGyroSensitivity(0);   // 250 degrees/s
  sensor.setThrottle();
#ifdef SERIAL_OUTPUT
  Serial.println("start...");
#endif
  
  // set callibration values from calibration sketch.
  sensor.axe = 0;
  sensor.aye = 0;
  sensor.aze = 0;
  sensor.gxe = 0;
  sensor.gye = 0;
  sensor.gze = 0;
}

void loop()
{
  if (sensor.read() == GY521_OK)
  {
    // For some reason the GY521 sends invalid numbers the first time
    if (!isnan(sensor.getAngleX()))
    {
      // Use an IIR filter for the accelerometer values
      dataToSend.angleX = 0.3 * sensor.getAngleX() + 0.7 * dataToSend.angleX;
      dataToSend.angleY = 0.3 * sensor.getAngleY() + 0.7 * dataToSend.angleY;
      dataToSend.angleZ = 0.3 * sensor.getAngleZ() + 0.7 * dataToSend.angleZ;
      dataToSend.temperature = sensor.getTemperature();
    }
  }

#ifdef SERIAL_OUTPUT
  if (counter % 10 == 0)
  {
    Serial.println("\tangleX\tangleY\tangleZ\ttemperature");
  }

  Serial.print(counter);
  Serial.print('\t');
  Serial.print(dataToSend.angleX);
  Serial.print('\t');
  Serial.print(dataToSend.angleY);
  Serial.print('\t');
  Serial.print(dataToSend.angleZ);
  Serial.print('\t');
  Serial.print(dataToSend.temperature);
  Serial.println();
#endif

  if (counter % 5 == 0)
  {
    detachInterrupt(CC1101Interrupt);
    CCPACKET packet;
    // We also need to include the 0 byte at the end of the string
    packet.length = sizeof(dataToSend);
    memcpy((char *)packet.data, (char *)&dataToSend, packet.length);
    radio.sendData(packet);
    attachInterrupt(CC1101Interrupt, messageReceived, FALLING);
  }
  counter++;
  delay(100);
}
