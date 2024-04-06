// Trainctor Game
// Alex Johnson
// Started: 4/3/2024
// Updated: 4/4/2024

#include <Arduino.h>
#include <U8g2lib.h> // U8g2 for text plus graphics

#define xpin A0
#define ypin A1
#define buttonpin A2

U8G2_SSD1306_128X64_NONAME_F_HW_I2C oled{U8G2_R0};

void(* resetFunc) (void) = 0;//declare reset function at address 0
//global variables for player
int x_pos = 32, y_pos = 32;
int xvel = 0, yvel = 0;
int x_angle = 512, y_angle =512; 

// general game global variables
int score = 0;
int level = 1; 
int scorePerLevel = 4;
int speedIncreasePerLevel = 1; 
const int gap_shrink = 2;


// obstacle global variables
int obst_vel = 2;
const int starting_width = 24; 
int obst_width = 24; 
int x_obst = 128;
int y_obst = random(10, 50);



// player sprite 
#define train_width 16
#define train_height 16
const static unsigned char train_bits[] PROGMEM = {
   0x00, 0x00, 0xfc, 0x00, 0x84, 0x00, 0x84, 0x00, 0x84, 0x38, 0x84, 0x38,
   0xfc, 0x7f, 0xfc, 0x7f, 0xfc, 0x7f, 0xfc, 0x7f, 0xfc, 0x7f, 0xfc, 0x7f,
   0xcc, 0x66, 0xcc, 0x66, 0x78, 0x3c, 0x30, 0x18 };

// drawObstacle 
// given x,y coordinates, and a height h
// prints two boxes seperated by height h,
// centered about x,y
void drawObstacle(int x_pos, int y_pos, int inner_height) {

  const int scr_height = 64;
  const int scr_width = 128;
  const int box_width = 5;
  const int top_box_start = 0;

  int bottom_box_start = y_pos + inner_height;
  int top_box_height = y_pos - (inner_height / 2);
  int bottom_box_height = 64 - top_box_height - (inner_height / 2);

  oled.drawBox(x_pos, top_box_start, box_width, top_box_height);        // top box
  oled.drawBox(x_pos, bottom_box_start, box_width, bottom_box_height);  // bottom box
  oled.sendBuffer();
}



// check_collision
// given player and obstacle x,y coordinates, 
// returns true if a collision between the two, 
// and false otherwise 
bool check_collision(int x_player,int y_player, int x_obst, int y_obst)
{
  if(x_obst <= x_player + train_height && x_obst >= x_player)  
  {
    if (y_player > (y_obst + (obst_width/2))){
      return true;
    }
    else if(y_player < (y_obst-(obst_width/2))){
      return true;
    }
    else {
    return false;
    }
  }
  else
  {
    return false;
  }
}

// move_player
// given "angles" from analog stick,
// moves player accordingly,
// playerSpeed can be adjusted to increase player speed
// angleUpperBounds & angleLowerBounds can be moved closer to 523, 
// in order to shrink joystick dead-zone
void move_player(int x_angle, int y_angle) 
{
  const int playerSpeed = 4;
  const int angleUpperBounds = 700, angleLowerBounds = 400; 

  if(x_angle > angleUpperBounds){ 
    xvel = -playerSpeed;
  } else if (x_angle < angleLowerBounds) { 
    xvel = playerSpeed;
  } else { 
    xvel = 0;
  }

    if(y_angle > angleUpperBounds){ 
    yvel = playerSpeed;
  } else if (y_angle < angleLowerBounds) { 
    yvel = -playerSpeed;
  } else { 
    yvel = 0;
  }


  x_pos = ((x_pos + xvel) & 127);
  y_pos = ((y_pos + yvel) & 63);

}
// printscore
// prints out players score and level to screen, 
// both of which are tracked via global variable
void printscore()
{
  oled.setCursor(10, 10); 
  oled.println(score);
  oled.setCursor(85, 10);
  oled.print("level ");
  oled.println(level); 
}
// startbutton
// pauses until stick is pressed, 
// tells basic information on game objective
void startbutton()
{
  while(digitalRead(A2) == 1)  
  {
    oled.clearBuffer();
    oled.setCursor(10, 25); 
    oled.println("avoid obstacles");
    oled.setCursor(10, 35);
    oled.println("press down on stick");
    oled.setCursor(10, 45); 
    oled.println("to start");
    oled.sendBuffer();
  }
}


void setup(void) {
  pinMode(xpin, INPUT);
  pinMode(ypin, INPUT);
  pinMode(buttonpin, INPUT_PULLUP);
  oled.begin();
  oled.setFlipMode(0); // change bool if it's upside down
  oled.setFont(u8g2_font_ncenB08_tr);  
  startbutton();
}




void loop(void) {
  oled.clearBuffer();
  printscore(); 

  oled.drawXBMP( x_pos, y_pos, train_height, train_width, train_bits);
  drawObstacle(x_obst, y_obst, obst_width);

  if (check_collision(x_pos, y_pos, x_obst, y_obst))
  {
    resetFunc(); //reset to start of program
  }

  x_angle = analogRead(xpin);
  y_angle = analogRead(ypin);

  move_player(x_angle, y_angle);  


  x_obst -= obst_vel;

  // logic for scoring & level
  // score increases each time an obstacle leaves the screen
  // size of gap in obstacle decreases each time an obstacle leaves screen          
  //    every 4 points level increases by 1,
  //      each level increase resets the obstacle gap size to a default
  //      and increases the velocity of the obstacle by                             
  if (x_obst < 0)
  {
    score += 1;
    obst_width -= gap_shrink; 
    if(score % 4 == 0)
    { 
      obst_vel += speedIncreasePerLevel;
      level += 1; 
      obst_width = starting_width; 
    }
    x_obst  = 128;
    y_obst = random(10, 50);
  }
  oled.sendBuffer();
}
