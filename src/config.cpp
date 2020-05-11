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


    //energy ball
    this->engBallRotRate = 1.5;
    this->engBallVibrateFreq = 0.01;
    this->engBallVibrateAmplitude = 0.02;
    //phase offset between first and last verticies of engBall
    this->engBallVibePhase = 3.5;
    //in seconds
    this->engBallExpTimer = 0.1;
    this->engBallExpVel = 2;
    //varience is an upper bound
    this->engBallExpVelVarience = 0;


    //screen
    //screen vel unused
    this->screenVel = 0.5;

    this->screenOffset =  0.2;
    this->defaultZoom = 0.5;


    //gameplay
    this->plrTopSpeed = 2;
    this->plrAcceleration = 4;
    this->plrBulletVel = 2;
    this->plrShotVarience= 0.5;
}
