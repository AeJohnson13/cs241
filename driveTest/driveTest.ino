//driveTest 
//Alex Johnson

#include "SoftwareSerial.h"
const int rxPin = 15;
const int txPin = 14; 
SoftwareSerial Roomba = SoftwareSerial(rxPin, txPin);

#define ROOMBA_START_OPCODE          128
#define ROOMBA_BAUD_OPCODE           129
#define ROOMBA_CONTROL_OPCODE        130
#define ROOMBA_SAFE_OPCODE           131
#define ROOMBA_FULL_OPCODE           132
#define ROOMBA_POWER_OPCODE          133
// 134, 135, 136 = Spot, Clean, Max - Not Using These 
#define ROOMBA_DRIVE_OPCODE          137
#define ROOMBA_FORWARD_OPCODE        32768

void roombaFullStart(){
    const int startBuffSize = 2;
    uint8_t startbuff[startBuffSize] = {128, 132};
    delay(2000); // time at start for some reason 
    pinMode(rxPin, INPUT);
    pinMode(txPin, OUTPUT);
    Roomba.begin(19200);
    delay(2000);
    Roomba.write(startbuff, startBuffSize);
}

void drive(uint16_t Velocity, uint16_t Radius)
{
  uint8_t highVel = (Velocity>>8) & 0xff;
  uint8_t lowVel = (Velocity>>0) & 0xff;
  uint8_t highRad = (Radius>>8) & 0xff;
  uint8_t lowRad = (Radius>>0) & 0xff;

    const int driveBuffSize = 5; 
    uint8_t drivebuff[driveBuffSize] = {ROOMBA_DRIVE_OPCODE, highVel, lowVel, highRad, lowRad};

}


void setup() 
{
    roombaFullStart();
    drive(300, ROOMBA_FORWARD_OPCODE);
    delay(3000);
    drive(0, ROOMBA_FORWARD_OPCODE);
}

void loop() {
 
}