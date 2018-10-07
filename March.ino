/*
March

MIT License

Copyright (c) 2018 Nolan Gams

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/


#include "Arduboy2.h"
#include "bitmaps.h"
#include "unit.h"

Arduboy2 arduboy;

//These numbers should all be the same
byte UNIT_COUNT = 25;
Unit AllyUnits[25];
Unit EnemUnits[25];

byte gamemode, counter, dif, lanePoint, tempo;
byte leftbuffer, rightbuffer, upbuffer, downbuffer, abuffer, bbuffer, stepCounter, p1turn;
int stepM, gold, health, playerunitcount;
long score;
boolean sound, pause;


int EEPROM_SAVE_LOCATION = 521; //this is the number of the location in EEPROM where the High score will be saved. This game uses the next 10 spaces after the initial location

void setup() {
  for(byte x = 0; x <= UNIT_COUNT; x++ ){
    AllyUnits[x].setSide(1);
    EnemUnits[x].setSide(2);
  }
  
  arduboy.initRandomSeed();
  arduboy.begin();
  arduboy.setFrameRate(30);
  dif = 4;
  lanePoint = 2;
  tempo = 10;
  sound = false;
  p1turn = true;
  score = 0;
  
}

void loop() {
  if (!arduboy.nextFrame()) {
    return;
  }
  arduboy.clear();

  if (dif >= 11) {dif = 1;}
  if (dif <= 0 ) {dif = 10;}

  if (stepCounter == tempo && pause == false) {
    stepCounter = 1;
    stepM++;    
  }
  if (pause == false) { stepCounter++;}

  mainGame();
  
  if (arduboy.notPressed(LEFT_BUTTON ) == true) {leftbuffer = 0;}
  if (arduboy.notPressed(RIGHT_BUTTON) == true) {rightbuffer = 0;}
  if (arduboy.notPressed(UP_BUTTON   ) == true) {upbuffer = 0;}
  if (arduboy.notPressed(DOWN_BUTTON ) == true) {downbuffer = 0;}
  if (arduboy.notPressed(A_BUTTON    ) == true) {abuffer = 0;}
  if (arduboy.notPressed(A_BUTTON    ) == true) {bbuffer = 0;}

  if (lanePoint >= 6) {lanePoint = 1;}
  if (lanePoint <= 0) {lanePoint = 5;}

  arduboy.display();
  //delay(300);
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////Main Game///////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

byte unitSelect = 2;
void mainGame() {
  if (pause == false) {
    arduboy.drawSlowXYBitmap(0, 0, layout, 128, 64, 1);
    arduboy.drawSlowXYBitmap(0, 3 + (10 * (lanePoint - 1)), pointer, 3, 3, 1);
    arduboy.setCursor(38, 55);
    arduboy.print(gold);
    arduboy.setCursor(100, 55);
    arduboy.print(health);
    //this handles the unit image that showes on the right side of the screen
    if (unitSelect >= 8) {unitSelect = 1;}
    if (unitSelect <= 0) {unitSelect = 7;}
    if (unitSelect == 1) {arduboy.drawSlowXYBitmap(107, 13, exsplodie, 9, 10, 1);}
    if (unitSelect == 2) {arduboy.drawSlowXYBitmap(107, 13, swordie, 9, 10, 1);}
    if (unitSelect == 3) {arduboy.drawSlowXYBitmap(107, 13, spearie, 9, 10, 1);}
    if (unitSelect == 4) {arduboy.drawSlowXYBitmap(107, 13, axie, 9, 10, 1);}
    if (unitSelect == 5) {arduboy.drawSlowXYBitmap(107, 13, halie, 9, 10, 1);}
    if (unitSelect == 6) {arduboy.drawSlowXYBitmap(107, 13, bowie, 9, 10, 1);}
    if (unitSelect == 7) {arduboy.drawSlowXYBitmap(107, 13, merchent, 9, 10, 1);}
    //

    //button actions
    if (arduboy.pressed(LEFT_BUTTON) && leftbuffer == 0) {
      leftbuffer = 1; 
      unitSelect--;
    }
    if (arduboy.pressed(RIGHT_BUTTON) && rightbuffer == 0) {
      rightbuffer = 1; 
      unitSelect++;
    }
    if (arduboy.pressed(UP_BUTTON) && upbuffer == 0) {
      upbuffer = 1; 
      lanePoint--;
    }
    if (arduboy.pressed(DOWN_BUTTON) && downbuffer == 0) {
      downbuffer = 1; 
      lanePoint++;
    }
    if (arduboy.pressed(A_BUTTON) && abuffer == 0) {
      abuffer = 1;
      placeUnit();
      if(playerunitcount < UNIT_COUNT && lanePoint > 0 && lanePoint < 6){
       for(byte x = 0; x <= UNIT_COUNT; x++) {
          if( AllyUnits[x].active == false ){
             AllyUnits[x].setType(unitSelect);
             AllyUnits[x].setLane(lanePoint-1);
             AllyUnits[x].setPos(0);
             AllyUnits[x].setActivity(true);
             playerunitcount++;
             break;
         }
        }
       }
    }
    if (arduboy.pressed(B_BUTTON) && abuffer == 0) {
      bbuffer = 1; 
      pause= true;
    }
    if (stepM == 1) {
      stepM = 0; 
      updateGame();
    }
    drawUnits();
    arduboy.setCursor(36, 39);
    arduboy.print(playerunitcount);
  }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////Pause Game///////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  else {
    arduboy.setCursor(0, 3);
    arduboy.print("PAUSED");
    arduboy.setCursor(0, 55);
    arduboy.print("Press 'a' to unpause");
    arduboy.setCursor(0, 15);
    arduboy.print("SCORE: ");
    arduboy.setCursor(36, 15);
    arduboy.print(score);
    arduboy.setCursor(0, 27);
    arduboy.print("GOLD : ");
    arduboy.setCursor(36, 27);
    arduboy.print(gold);

    
    arduboy.setCursor(50, 3);
    if(sound==true){arduboy.print("Sound  <Off>");}
    if(sound==false){arduboy.print("Sound  < On>");}
    
    if (arduboy.pressed(LEFT_BUTTON) and leftbuffer == 0 and sound==true or 
        arduboy.pressed(RIGHT_BUTTON) and rightbuffer == 0 and sound==true) {
          leftbuffer = 1; 
          rightbuffer = 1; 
          sound = false;
    }
    if (arduboy.pressed(LEFT_BUTTON) and leftbuffer == 0 and sound==false or 
        arduboy.pressed(RIGHT_BUTTON) and rightbuffer == 0 and sound==false) {
          leftbuffer = 1; 
          rightbuffer = 1; 
          sound = true;
    }
    
  }

  if (arduboy.pressed(A_BUTTON) and bbuffer == 0 and pause == true) {   
    bbuffer = 1;
    pause = false;
  }
  
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////Functions///////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * upon call, the updateGame function will check to see whose turn it is to move. then it will set p1turn to the next players
 * turn. This function should only be called once a step is initiated. 
 */
void updateGame() {

  switch(p1turn){
      case 1:
        updatePlayerUnits();
        p1turn = 2;
        break;
      case 2:
        updateComputerUnits();   
        p1turn = 1;
        break;  
 }
}

/*
 * This function will do a for loop that will update the players units
 * 
 */
 void updatePlayerUnits(){
    for(byte x = 0; x <= UNIT_COUNT; x++) {
      if( AllyUnits[x].active == true){
       if(AllyUnits[x].pos == 15){
          AllyUnits[x].kill();
          playerunitcount--;
       }
       else{
          AllyUnits[x].setPos(AllyUnits[x].pos + 1);
       }
      }
    }
  
 }
 /*
  * This function will do a for loop hat will update the enemy units 
  * 
  */
  void updateComputerUnits(){

    
  }

 /*
  * This function will place a unit on the field in the lane that the player wants.  
  * If will first check if the maximum amount of units have been reached (playerunitcount < UNIT_COUNT). 
  * It will then check if there is already a unit there (enemy or ally).
  * 
  */
  void placeUnit(){
    
    /*if(playerunitcount < UNIT_COUNT && lanePoint > 0 && lanePoint < 6){
       for(byte x = 0; x <= UNIT_COUNT; x++) {
          
          
          if( AllyUnits[x].active == false ){
             AllyUnits[x].setType(unitSelect);
             AllyUnits[x].setLane(lanePoint-1);
             AllyUnits[x].setPos(0);
             AllyUnits[x].setActivity(true);
             playerunitcount++;
             break;
         }
        }
       }*/
    
  }
  
  /*
   * This function checks a square to see if a particular unit is in it.
   * 
   */
   boolean checkSquareForUnit(byte inlane, byte inpos, byte inteam){
      boolean isTrue = false;
      for(byte x = 0; x <= UNIT_COUNT; x++){
        if ( AllyUnits[x].pos == inpos && AllyUnits[x].lane == inlane && inteam == 1){
          isTrue = true;
        }
        if ( EnemUnits[x].pos == inpos && EnemUnits[x].lane == inlane && inteam == 2){
          isTrue = true;
        }
      }
    
      return isTrue;
   }
   /*
    * This function will draw both ally and enemy units on the field. 
    * 
    */
    void drawUnits(){
      for(byte x = 0; x <= UNIT_COUNT; x++) {
        if ( AllyUnits[x].type == 2) {arduboy.drawSlowXYBitmap(AllyUnits[x].pos*5+4, AllyUnits[x].lane*10, swordie, 9, 10, 1);}
        if ( AllyUnits[x].type == 3) {arduboy.drawSlowXYBitmap(AllyUnits[x].pos*5+4, AllyUnits[x].lane*10, spearie, 9, 10, 1);}
        if ( AllyUnits[x].type == 4) {arduboy.drawSlowXYBitmap(AllyUnits[x].pos*5+4, AllyUnits[x].lane*10, axie, 9, 10, 1);}
        if ( AllyUnits[x].type == 5) {arduboy.drawSlowXYBitmap(AllyUnits[x].pos*5+4, AllyUnits[x].lane*10, halie, 9, 10, 1);}
        if ( AllyUnits[x].type == 6) {arduboy.drawSlowXYBitmap(AllyUnits[x].pos*5+4, AllyUnits[x].lane*10, bowie, 9, 10, 1);}
        if ( AllyUnits[x].type == 7) {arduboy.drawSlowXYBitmap(AllyUnits[x].pos*5+4, AllyUnits[x].lane*10, merchent, 9, 10, 1);}
      
      }
      
    }


   
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////Game Over///////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
