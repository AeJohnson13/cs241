#include "SoftwareSerial.h" 
const int rxPin = 15;
const int txPin = 14; 
SoftwareSerial Roomba = SoftwareSerial(rxPin, txPin);

// https://cdn.hackaday.io/files/1747287475562752/Roomba_SCI_manual.pdf
// https://forum.arduino.cc/t/help-programming-irobot-create-cant-make-it-work/422357


const int startSize = 2; 
uint8_t startbuff[startSize] = {128, 132};



//**************************************** buffers for hot cross buns 
const int songLength = 3;
const int songBufflength = 3 + (2 * songLength);
uint8_t mysong[songBufflength] = 
                         {140, 0, 3,    // song begins on next line
                          52, 32,                // note, length, in 1/64ths of second up to 255
                          50, 32, 
                          48, 32}; 



const int songLength2 = 8;   
const int songBuffLength2 = 3 + (2 * songLength2);
uint8_t mysong1[songBuffLength2] = 

                          {140, 1, 8, 
                          48, 16,
                          48, 16, 
                          48, 16, 
                          48, 16, 
                          50, 16,
                          50, 16, 
                          50, 16, 
                          50, 16};

uint8_t playSong[2] = {141, 0};
uint8_t playSong1[2] = {141, 1};
//*****************************************
void playDatSong()
{
  Roomba.write(mysong, 9);
  Roomba.write(mysong1, 19);
  delay(100);
  Roomba.write(playSong, 2); 
  delay(2000);
  Roomba.write(playSong, 2); 
  delay(2000);
  Roomba.write(playSong1, 2);
  delay(2150);
  Roomba.write(playSong, 2); 
}



uint8_t lightTestBuff[4] = {139, 24, 128, 128}; 
void lightTest() 
{ 
  Roomba.write(lightTestBuff, 4);
}

void roombaStartup(){
  delay(2000); // time at start for some reason 
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  Roomba.begin(19200);
  delay(2000);
  Roomba.write(startbuff, startSize);
}

void setup() {
 roombaStartup();
 //lightTest();
 playDatSong();'
 //Roomba.write(133);  //Shutoff code 

}

void loop() {
 
}
