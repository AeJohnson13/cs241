// Roomba Main 
// Alex Johnson
// For CS 241
//
// Started: 4/6/2024
// Updated: TBD 

#include "SoftwareSerial.h"
#include <Wire.h>
#include <Adafruit_AMG88xx.h>

const int rxPin = 15;
const int txPin = 14; 
SoftwareSerial Roomba = SoftwareSerial(rxPin, txPin);

Adafruit_AMG88xx amg;

float pixels[AMG88xx_PIXEL_ARRAY_SIZE];
float columnSum[AMG88xx_PIXEL_ARRAY_SIZE/8] = {0,0,0,0,0,0,0,0};
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
// roombaDrive 
// given a velocity and a radius to drive at, 
// 
void roombaDrive(uint16_t Velocity, uint16_t Radius)
{
    uint8_t highVel = (Velocity>>8) & 0xff;
    uint8_t lowVel = (Velocity>>0) & 0xff;
    uint8_t highRad = (Radius>>8) & 0xff;
    uint8_t lowRad = (Radius>>0) & 0xff;
    const int driveBuffSize = 5; 
    uint8_t drivebuff[driveBuffSize] = {ROOMBA_DRIVE_OPCODE, highVel, lowVel, highRad, lowRad};

}
// readColumnSum
// reads the values from thermal sensor,
// sums the columns and places the sums in float array
// columnSum 
void readColumnSum()
{
    for(int i = 0; i < 8; i++)
    { 
        columnSum[i] = 0; 
    }
    
    amg.readPixels(pixels);
    for(int i=1; i<=AMG88xx_PIXEL_ARRAY_SIZE; i++){
      // i-1 
      columnSum[(i-1)%8] += pixels[i-1];     
    }
}
// readColumnSum
// prints the sums of the columns to the serial monitor 
void printSums() 
{
   
    for(int i = 0; i < 8; i++)  // prints out the sums 
    {   
        Serial.print("column  :  ");
        Serial.print(i); 
        Serial.print(" column sum: ");
        Serial.println(columnSum[i]); 
    }
    Serial.println("");
}


// determineState

uint8_t maxColumn()
{
   int maxColumn = -1;
    for(int i = 0; i < 8; i++)  // iterate over sums 
    {   
        float columnMax = 0; 
        if(columnSum[i] > 160 && columnSum[i] > columnMax){  // if a column sum is greater than 60 degrees, 
          columnMax = columnSum[i];
        }
    } 
    return maxColumn;
}


void setup() {
    Serial.begin(19200);
    Serial.println("Begin Serial Comms");
    bool status;

    status = amg.begin();
    if (!status) {
        Serial.println("Could not find a valid AMG88xx sensor, check wiring!");
        while (1);
    }

    roombaFullStart();
}

void loop() {
    readColumnSum();
    int column = maxColumn();     
    if(column != -1) // found column with greater than 160 degrees celsius
    { 
        uint16_t driveangle == 0;                   // TODO: calculate drive angle 
        const uint16_t vel == 300;  
        roombaDrive(vel, driveangle);               // center roomba on column 
    } 
    else                                            // scan for column greater than 160 celsius 
    {
        const uint16_t turnVel = 300;
        roombaDrive(turnVel, -1);
        delay(1000);                                // need to change this timing, so that the roomba can check temperature while scanning 
        roombaDrive(turnVel, 1);
        delay(2000);

    }
    delay(1000);
}
