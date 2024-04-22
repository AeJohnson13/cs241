// Roomba Main
// Alex Johnson
// For CS 241
//
// Started: 4/6/2024
// Updated: 4/22/2024


// Roomba Serial Comms Manual
// https://cdn.hackaday.io/files/1747287475562752/Roomba_SCI_manual.pdf


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
#define ROOMBA_CONTROL_OPCODE        130
#define ROOMBA_FULL_OPCODE           132
#define ROOMBA_POWER_OPCODE          133
#define ROOMBA_DRIVE_OPCODE          137
#define ROOMBA_FORWARD_OPCODE        32768

long startTime = 0; 
int temperaturethresh = 172;  //default value 


// buffers for startTone, 
const int songLength = 3;
const int songBufflength = 3 + (2 * songLength);
const uint8_t mysong[songBufflength] =
                         {140, 0, 3,    // song begins on next line
                          68, 16,                // note, length, in 1/64ths of second up to 255
                          72, 16,
                          75, 16};
const uint8_t playSong[2] = {141, 0};



// startTones
// defines a song on the roomba and plays it, song is defined in buffer mysong,
// and buffer to play song is playSong
void startTone(){
    Roomba.write(mysong, 9);
    delay(100);
    Roomba.write(playSong, 2);
    delay(800);
}



// roombaFullStart
// defines a buffer that contains the roomba start opcode and full mode opcode,
// sets up tx and rx pins, aswell as sending the start buffer to the roomba
//      CONTAINS three seconds of hard coded delaly including that within startTones()
void roombaFullStart(){
    const int startBuffSize = 2;
    uint8_t startbuff[startBuffSize] = {ROOMBA_START_OPCODE, ROOMBA_FULL_OPCODE};
    delay(1000); // time at start for some reason
    pinMode(rxPin, INPUT);
    pinMode(txPin, OUTPUT);
    Roomba.begin(19200);
    delay(1000);
    Roomba.write(startbuff, startBuffSize); 
    delay(100);
    startTone();
}




// roombaDrive
// given a velocity and a radius to drive at,
//    Velocity (-500 – 500 mm/s)
//    Radius (-2000 – 2000 mm)
//    
//    Special cases:
//    Straight = 32768 : defined as ROOMBA_FORWARD_OPCODE
//    Turn in place clockwise = -1
//    Turn in place counter-clockwise = 1
// converts 116 bit velocity and 16 bit radius into four 8 bit unsigned integers,
// and sends them to roomba along with the drive opcode
void roombaDrive(uint16_t Velocity, uint16_t Radius)
{
    uint8_t highVel = (Velocity>>8) & 0xff;
    uint8_t lowVel = (Velocity>>0) & 0xff;
    uint8_t highRad = (Radius>>8) & 0xff;
    uint8_t lowRad = (Radius>>0) & 0xff;
    const int driveBuffSize = 5;
    uint8_t drivebuff[driveBuffSize] = {ROOMBA_DRIVE_OPCODE, highVel, lowVel, highRad, lowRad};
    Roomba.write(drivebuff, driveBuffSize);
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


// maxColumn
// returns the number of the column with the highest sum,
// or -1 if none of the columns are warmer than the defined temperature
// threshhold, which is approx (room temp * 8) 
int maxColumn()
{
    readColumnSum();
    int maxColumn = -1;
    float columnMax = 0;
    //const int temperaturethresh = 172; // degrees celsius
    for(int i = 0; i < 8; i++)
    {      
        if( columnSum[i] >= columnMax && columnSum[i] > temperaturethresh)
        {
          columnMax = columnSum[i];
          maxColumn = i;
        }
       
    }
    return maxColumn;
}


//angle value must be between 2000 and -2000 mm
//    The longer radii make
//    Roomba drive straighter; shorter radii make it turn more. A Drive
//    command with a positive velocity and a positive radius will make
//    Roomba drive forward while turning toward the left. A negative
//    radius will make it turn toward the right.

// turnAngle
//    given a column number 0-7, will return
//    a value for the roomba to turn at

uint16_t turnAngle(uint8_t column) {
    const int turnRate = 100; 
    if (column > 4) {
        return turnRate;
    } 
    else if (column < 3) {
        return -turnRate;
    }
    else {
        return ROOMBA_FORWARD_OPCODE;
    }
}


// roombaON
// puleses roomba device detect pin for 500 ms, 
// this will turn on the roomba if it is turned off
void roombaOn()
{
    const int deviceDetectPin = 2;
    pinMode(deviceDetectPin, OUTPUT);
    digitalWrite(deviceDetectPin, HIGH);
    delay(500);
    digitalWrite(deviceDetectPin, LOW);
    delay(500);
    digitalWrite(deviceDetectPin, HIGH);
    delay(500);
}

// findThresh
// returns the average temperature across thermal sensor at startup
// for good performace point away from thermal object at startup 
int findThresh()
{
  readColumnSum();
  int thresh = 0; 
  for( int i = 0; i< 8; i++)
  {
    thresh += columnSum[i];
  }
  return ((thresh/8) + 15);
}


void setup() 
{

    roombaOn();
    bool status;

    status = amg.begin();  // will get stuck here if thermal sensor isn't connected
    if (!status) {
        while (1);
    }
    temperaturethresh = findThresh(); 
    
    roombaFullStart();
    startTime = millis(); 
}


bool spin = 0;

void loop() {
    const uint16_t vel = 150;  
    int column = maxColumn();  


    if(column != -1) // found column with greater than 160 degrees celsius
    {
        uint16_t driveangle = turnAngle(column);              
        roombaDrive(vel, driveangle);               // center roomba on column
    }
    else if(!(spin))                                  // scan for column greater than 160 celsius
    {
        spin = !(spin);
        roombaDrive(vel, -1);
    }


// turn off after 30ish seconds
    const long offTime = 30; //seconds
    const long offTimeMillis = offTime * 1000;
    long elapsed = millis() - startTime; 
    if (elapsed > offTimeMillis)
    {
        Roomba.write(ROOMBA_POWER_OPCODE);  //Shutoff code 
        while(1);
    }

    delay(200); // delay to slow down how fast the loop runs in in accordance with ag8338 refresh rate
}


