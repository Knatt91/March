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

// These numbers should all be the same
constexpr uint8_t UNIT_COUNT = 25;
Unit AllyUnits[25];
Unit EnemUnits[25];

enum class GameMode : uint8_t
{
	TitleScreen,
	MainGame,
};

GameMode gameMode = GameMode::MainGame;

uint8_t lanePoint = 2;
uint8_t tempo = 10;
uint8_t stepCounter = 0;
uint8_t currentPlayer = 1;

int gold = 300;
int health = 10;
int playerunitcount = 0;
int enemyunitcount = 0;
long score = 0;

bool sound = false;
bool pause = false;

// This is the number of the location in EEPROM where the High score will be saved.
// This game uses the next 10 spaces after the initial location.
constexpr int EEPROM_SAVE_LOCATION = 521;
constexpr int EEPROM_SAVE_SIZE = 10;

void setup()
{
	arduboy.begin();
	arduboy.setFrameRate(30);
	arduboy.initRandomSeed();
 
	for(uint8_t i = 0; i < UNIT_COUNT; ++i )
	{
		AllyUnits[i].setSide(1, i);
		EnemUnits[i].setSide(2, i);
	}     
}

void loop()
{
	if (!arduboy.nextFrame())
		return;

	arduboy.clear();
	arduboy.pollButtons();

	switch(gameMode)
	{
		case GameMode::TitleScreen:
			break;
		case GameMode::MainGame:
			mainGame();
			break;
	}

	arduboy.display();
}

constexpr uint8_t minAllyUnit = 1;
constexpr uint8_t maxAllyUnit = 6;

// This function will draw a single ally unit.
void drawAllyUnit(int x, int y, uint8_t unitType)
{
	static const uint8_t * const unitIcons[] PROGMEM =
	{
		exsplodie, swordie, spearie, axie, halie, bowie, merchant,
	};

	if(unitType <= maxAllyUnit)
	{
		const uint8_t * image = reinterpret_cast<const uint8_t *>(pgm_read_ptr(&unitIcons[unitType]));
		arduboy.drawSlowXYBitmap(x, y, image, 9, 10, WHITE);
	}
}
constexpr uint8_t minEnemUnit = 0;
constexpr uint8_t maxEnemUnit = 6;

// This function will draw a single Enemy unit.
void drawEnemUnit(int x, int y, uint8_t unitType)
{
  static const uint8_t * const unitIcons[] PROGMEM =
  {
    zomb,skele,crab,demon,golem,witch,horror,
  };

  if(unitType <= maxEnemUnit)
  {
    const uint8_t * image = reinterpret_cast<const uint8_t *>(pgm_read_ptr(&unitIcons[unitType]));
    arduboy.drawSlowXYBitmap(x, y, image, 9, 10, WHITE);
  }
}

// This function will draw both ally and enemy units on the field.
void drawUnits(void)
{
	for(uint8_t i = 0; i < UNIT_COUNT; ++i)
  {
		if(AllyUnits[i].active)
    {
			drawAllyUnit((AllyUnits[i].pos * 5) + 4 , AllyUnits[i].lane * 10, AllyUnits[i].type);
    }
    if(EnemUnits[i].active)
    {
      drawEnemUnit((EnemUnits[i].pos * 5) + 4 , EnemUnits[i].lane * 10, EnemUnits[i].type);
    }
  }
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////Opening Menu//////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////Main Game///////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint8_t unitSelect = 1;

void mainGame()
{
	if (!pause)
	{
		arduboy.drawSlowXYBitmap(0, 0, layout, 128, 64, 1);
		arduboy.drawSlowXYBitmap(0, 3 + (10 * (lanePoint - 1)), pointer, 3, 3, 1);
		arduboy.setCursor(38, 55);
		arduboy.print(gold);
		arduboy.setCursor(100, 55);
		arduboy.print(health);

		// This handles the unit image that showes on the right side of the screen
		drawAllyUnit(107, 13, unitSelect);

		//button actions
		if (arduboy.justPressed(LEFT_BUTTON))
		{
			if(unitSelect > minAllyUnit)
				--unitSelect;
			else
				unitSelect = maxAllyUnit;
		}
	
		if (arduboy.justPressed(RIGHT_BUTTON))
		{
			if(unitSelect < maxAllyUnit)
				++unitSelect;
			else
				unitSelect = minAllyUnit;
		}

		if (arduboy.justPressed(UP_BUTTON))
		{
			if(lanePoint > 1)
				--lanePoint;
			else
				lanePoint = 5;
		}
	
		if (arduboy.justPressed(DOWN_BUTTON))
		{
			if(lanePoint < 5)
				++lanePoint;
			else
				lanePoint = 1;
		}
	
		if (arduboy.justPressed(A_BUTTON))
		{
			placeUnit();
		}

		if (arduboy.justPressed(B_BUTTON))
			pause = true;

		if (stepCounter < tempo)
		{
			++stepCounter;
		}
		else
		{
			stepCounter = 0;
			updateGame();
		}

		drawUnits();
		arduboy.setCursor(36, 39);
		//arduboy.print(playerunitcount);
	}
	else
	{
		arduboy.setCursor(0, 3);
		arduboy.print(F("PAUSED"));
		arduboy.setCursor(0, 55);
		arduboy.print(F("Press 'a' to unpause"));
		arduboy.setCursor(0, 15);
		arduboy.print(F("SCORE: "));
		arduboy.setCursor(36, 15);
		arduboy.print(score);
		arduboy.setCursor(0, 27);
		arduboy.print(F("GOLD : "));
		arduboy.setCursor(36, 27);
		arduboy.print(gold);
		
		arduboy.setCursor(50, 3);
		arduboy.print(F("Sound "));
		if(sound)
			arduboy.print(F("<Off>"));
		else
			arduboy.print(F("< On>"));
		
		if (arduboy.justPressed(LEFT_BUTTON) || arduboy.justPressed(RIGHT_BUTTON))
			sound = !sound;

		if (arduboy.justPressed(A_BUTTON))
			pause = false;
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////Game Over///////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////Functions///////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// This function will check to see whose turn it is to move.
// Then it will set currentPlayer to the next player's turn.
// This function should only be called once a step is initiated.
void updateGame()
{
	switch(currentPlayer)
	{
		case 1:
			updatePlayerUnits();
      playerAttack();
			currentPlayer = 2;
			break;
		case 2:
			updateComputerUnits();	
      //enemyAttack();
      enemyHandler();
			currentPlayer = 1;
			break;
	}
}

void playerAttack()
{
  for(uint8_t i = 0; i < UNIT_COUNT; ++i)
    {
      for(uint8_t n = 0; n < UNIT_COUNT; ++n)
      {
        if(!EnemUnits[n].active)
          continue;
        if(EnemUnits[n].lane == AllyUnits[i].lane &&
           EnemUnits[n].pos - 1 < AllyUnits[i].pos + AllyUnits[i].rge)
        {
          EnemUnits[n].poke(AllyUnits[i].dmg);
        }
      }
    }
}

void enemyAttack()
{
  for(uint8_t i = 0; i < UNIT_COUNT; ++i)
    {
      for(uint8_t n = 0; n < UNIT_COUNT; ++n)
      {
        if(!AllyUnits[n].active)
          continue;
        if(AllyUnits[n].lane == EnemUnits[i].lane &&
           AllyUnits[n].pos+1 > EnemUnits[i].pos - EnemUnits[i].rge)
        {
          AllyUnits[n].poke(EnemUnits[i].dmg);
        }
      }
    }  
}
// This function will do a for loop that will update the player's units.
void updatePlayerUnits()
{
	for(uint8_t i = 0; i < UNIT_COUNT; ++i)
		if(AllyUnits[i].active)
		{
			if(AllyUnits[i].pos == 15)
			{
				AllyUnits[i].kill();
				--playerunitcount;
			}
     else if(checkSquareForUnit(AllyUnits[i].lane,AllyUnits[i].pos+1,0))
        continue;
		 else
				AllyUnits[i].setPos(AllyUnits[i].pos + 1);
		}
}
// This function will do a for loop hat will update the enemy units.
void updateComputerUnits(void)
{
    for(uint8_t i = 0; i < UNIT_COUNT; ++i)
    if(EnemUnits[i].active)
    {
      if(EnemUnits[i].pos == 0)
      {
        EnemUnits[i].kill();
        --enemyunitcount;
      }
      else if(checkSquareForUnit(EnemUnits[i].lane,EnemUnits[i].pos-1,0))
        continue;
      else
        EnemUnits[i].setPos(EnemUnits[i].pos - 1);
    }
}
// This function will place a unit on the field in the lane that the player wants.
// If will first check if the maximum amount of units have been reached (playerunitcount < UNIT_COUNT).
void placeUnit(void)
{
	if((playerunitcount < UNIT_COUNT) && (lanePoint > 0) && (lanePoint < 6) && !checkSquareForUnit(lanePoint - 1 , 0 , 0))
		for(uint8_t i = 0; i < UNIT_COUNT; ++i)
		{
			if(AllyUnits[i].active)
				continue;

			AllyUnits[i].setType(unitSelect);
			AllyUnits[i].setLane(lanePoint - 1);
			AllyUnits[i].setPos(0);
			AllyUnits[i].setActivity(true);
			++playerunitcount;
			break;
		}
}
// This function checks a square to see if a particular unit is in it.
bool checkSquareForUnit(uint8_t lane, uint8_t pos, uint8_t team)
{
	for(uint8_t i = 0; i < UNIT_COUNT; ++i)
		//if ( AllyUnits[i].pos == pos && AllyUnits[i].lane == lane)
			  switch(team)
        {
          case 0:
            if ( AllyUnits[i].pos == pos && AllyUnits[i].lane == lane || EnemUnits[i].pos == pos && EnemUnits[i].lane == lane)
              return true;
              break;
          case 1:
            if( AllyUnits[i].pos == pos && AllyUnits[i].lane == lane )
              return true;
              break;
          case 2:
            if( EnemUnits[i].pos == pos && EnemUnits[i].lane == lane )
              return true;
              break;
        }
	return false;
}

// This handles the spawning of the enemy units. 

uint8_t enemIntencity = 1;
uint8_t eventCountdown = 0;
uint8_t enemUnitSpawnBuffer = 1;
uint8_t enemUnitSpawnCounter = 0;
void enemyHandler()
{
  uint8_t d100 = random(100);
    if(enemUnitSpawnCounter == enemUnitSpawnBuffer && enemyunitcount < UNIT_COUNT)
    {
      spawnEnemy();
      enemUnitSpawnCounter = 0;
    }
    else
    {
      ++enemUnitSpawnCounter;
    }
}

void spawnEnemy()
{
    for(uint8_t i = 0; i < UNIT_COUNT; ++i)
    {
      if(EnemUnits[i].active)
        continue;

      EnemUnits[i].setType(0);
      //EnemUnits[i].setLane(random(5));
      EnemUnits[i].setLane(2);
      EnemUnits[i].setPos(15);
      EnemUnits[i].setActivity(true);
      ++enemyunitcount;
      break;
    }  
}
