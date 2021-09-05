# TrailerLevel
Wireless level for semi-trailer

This project consists of a sender and receiver originally developed using an Arduino Nano for each side.
The sender utilizes a GY521 sensor with a three-axis accelerometer and three-axis gyroscope.
The receiver contains a 20x4 LCD display with an I2C interface.
The sender and receiver communicate with a cheap 433 MHz radio.

# Dependencies
To build these projects you will need to install the following Arduino libraries:

* RadioHead
* GY521
* LiquidCrystal I2C

# Boxes
The 3D models for the boxes and lids were created in OpenSCAD.
It is recommended to use PETG or ABS for durabiity. PLA is not recommended.
