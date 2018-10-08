#pragma once

#include "bitmaps.h"

#include <stdint.h>

class Unit
{
public:
	int hp; 
	uint8_t side;
	uint8_t dmg;
	uint8_t rge;
	uint8_t type;
	uint8_t spd;
	uint8_t spdbuffer;
	uint8_t lane;
	uint8_t pos;
	bool action;
	bool active = false;

	void kill(void)
	{
		hp = 0;
		active = false;
		lane = 0;
		pos = 0;
		type = 0;
		dmg = 0;
	}

	void setSpd(uint8_t inSpd){spd = inSpd;}
	void setSpdBuffer(uint8_t inSB){spdbuffer = inSB;}
	void setHP(int inHP){hp = inHP;}
	void setPos(uint8_t inPos){pos = inPos;}
	void setLane(uint8_t inLane){lane = inLane;}
	void setActivity(bool inActive){active = inActive;}
	void setAction(bool inAction){action = inAction;}
	void setDmg(uint8_t inDmg){dmg = inDmg;}
	void setSide(uint8_t inSide){side = inSide;}
	void setRge(uint8_t inRge){rge = inRge;}
	void setType(uint8_t inType){type = inType;}
};

