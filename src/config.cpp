#include "config.h"

void Config::init(){
    //note vsync will cap this value
    this->fps = 60;

    this->spf= 1.0 /((double) this->fps);
    this->mspf = 1000/(this->fps);


}

void Stats::init(){
    // APPEARENCE
        //bullets
    this->bulletTrailDecay=0.13;
    this->bulletTrailSegments=8;
    this->bulletDiameter=5;
    this->riccoBulletVelDamping = 0.8;
    this->bulletVelDamping = 0.3;
    this->bulletMinVel = 1;

    //sparks
    this->sparkDiameter = 8;
    this->sparkTrailSegments = 3;
    this->sparkTrailDecay = 0.08;
    this->riccoSparkSpawnDamping = 0.6;
    this->riccoSparkVelDamping = 0.66;
    this->riccoSparkVelVarience = 1;
    this->sparkVelDamping = 3;
    this->sparkMinVel = 0.1;


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
    this->screenOffset =  0.2;
    this->defaultZoom = 0.5;




    //GAMEPLAY

    //guns
    this->mg.init(bullet,1,7,1.5,1,true,100,0);
    this->smg.init(bullet,1,5,2,1,true,50,0);
    this->shotgun.init(bullet,1,4,1.5,7,false,400,0);
    this->autoShotgun.init(bullet,1,4,1.5,5,true,300,0);
    this->sniper.init(bullet,1,10,0,1,false,500,0);
    this->energyRifle.init(energyBall,10,4,0,1,false,150,0);
    this->energyShotgun.init(energyBall,10,2,1,5,false,200,0);

    //player
    this->plrTopSpeed = 2;
    this->plrAcceleration = 4;
    this->plrHealth = 100;
    this->plrContactDamage = 100;

    this->swarmerTopSpeed = 3;
    this->swarmerAcceleration = 3;
    this->swarmerDrag = 1;
    this->swarmerHealth = 50;
    this->swarmerContactDamage = 20;
}
