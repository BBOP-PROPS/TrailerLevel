//
//    FILE: LevelSender.ino
//  AUTHOR: Mark Nierzwick
// VERSION: 0.0.0
// PURPOSE: minimal demo
//    DATE: 2021-03-28

//#define SERIAL_OUTPUT // used for debugging

#include "GY521.h" // 3-axis accelerometer and gyro
#include "RH_ASK.h" // Radio Head Amplitude Shift Key

RH_ASK transmitter;

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
  Serial.begin(115200);
  Serial.println(__FILE__);

  delay(500); // Let power settle before initializing
  
  Wire.begin();
  while (sensor.wakeup() == false)
  {
    Serial.println("\tCould not connect to GY521");
    delay(1000);
  }

  while (!transmitter.init())
  {
    Serial.println("Radio init failed");
    delay(1000);
  }

  sensor.setAccelSensitivity(0);  // 2g
  sensor.setGyroSensitivity(0);   // 250 degrees/s
  sensor.setThrottle();
  Serial.println("start...");
  
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
    transmitter.send((uint8_t *)&dataToSend, sizeof(dataToSend));
  }
  counter++;
  delay(100);
}
