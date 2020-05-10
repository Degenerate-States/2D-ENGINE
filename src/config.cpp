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
    this->bulletDiameter=5;

    //sparks
    this->sparkDiameter = 3;
    this->sparkTrailSegments = 3;
    this->sparkTrailDecay = 0.05;
    this->riccoSparkSpawnDamping = 2;
    this->riccoSparkVelDamping = 2.5;
    this->sparkVelVarience = 0.5;

    //screen
    //screen vel unused
    this->screenVel = 0.5;

    this->screenOffset =  0.2;
    this->defaultZoom = 0.5;


    //gameplay
    this->plrTopSpeed = 2;
    this->plrAcceleration = 4;
    this->plrBulletVel = 5;
    this->plrShotVarience= 0.5;
}
