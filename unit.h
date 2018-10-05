//#ifndef UNIT_H
#define UNIT_H
#include "bitmaps.h"
class Unit{
  public:
    
    int hp; 
    byte side, dmg, rge, type, spd, spdbuffer;
    byte lane, pos;
    boolean action, active;
    Unit(){
      active = false;
    }
    void kill(){
      hp = 0;
      active = false;
      lane = 0;
      pos = 0;
      type = 0;
      dmg = 0;

    }

    void setSpd(byte inSpd){spd = inSpd;}
    void setSpdBuffer(byte inSB){spdbuffer = inSB;}
    void setHP(int inHP){hp = inHP;}
    void setPos(byte inPos){pos = inPos;}
    void setLane(byte inLane){lane = inLane;}
    void setActivity(boolean inActive){active = inActive;}
    void setAction(boolean inAction){action = inAction;}
    void setDmg(byte inDmg){dmg = inDmg;}
    void setSide(byte inSide){side = inSide;}
    void setRge(byte inRge){rge = inRge;}
    void setType(byte inType){type = inType;}
};

