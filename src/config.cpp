#include "config.h"

void Config::init(){
    this->windowSizeX = 1400;
    this->windowSizeY= 800;
    //note vsync will cap this value
    this->fps = 60;

    this->spf= 1.0 /((double) this->fps);
    this->mspf = 1000/(this->fps);
}