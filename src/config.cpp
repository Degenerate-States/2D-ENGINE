#include "config.h"



void Config::init(){
    //note vsync will cap this value
    this->fps = 60;

    this->spf= 1.0 /((double) this->fps);
    this->mspf = 1000/(this->fps);


}

void Stats::init(){
    // appearence
    //bullets
    this->bulletTrailDecay=0.15;
    this->bulletTrailSegments=10;

    //screen
    this->screenVel = 0.5;
    this->screenOffset =  0.2;

    //gameplay
    this->plrTopSpeed = 1;
    this->plrAcceleration = 3;
}
