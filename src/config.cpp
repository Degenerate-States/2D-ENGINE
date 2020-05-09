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
    this->bulletTrailDecay=0.13;
    this->bulletTrailSegments=8;

    //screen
    this->screenVel = 0.5;
    this->screenOffset =  0.2;
    this->defaultZoom = 0.7;

    //gameplay
    this->plrTopSpeed = 2;
    this->plrAcceleration = 4;
    this->plrBulletVel = 5;
}
