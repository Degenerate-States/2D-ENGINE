#include "config.h"



void Config::init(){
    //note vsync will cap this value
    this->fps = 60;

    this->spf= 1.0 /((double) this->fps);
    this->mspf = 1000/(this->fps);

    //bullets
    this->bulletTrailDecay=0.15;
    this->bulletTrailSegments=8;
}
