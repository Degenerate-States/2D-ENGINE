#include "assets.h"
using namespace std;

void Assets::init(){
    colliderID = 0;

    //polygon asset must be defined clockwise, otherwise normals will be inverted
    this->plrAsset = {{-0.05,-0.04},{0.08,0.0},{-0.05,0.04},{-0.02,-0.0}};
    this->gunAsset= {{0.06,0.0},{0.01,0.01},{-0.0,0.02},{-0.02,-0.0},{0.0,-0.02},{0.01,-0.01}};
    this->flameAsset = {{-0.12,-0.0},{-0.05,-0.02},{-0.03,-0.0},{-0.05,0.02}};
    this->box = {{-0.07,-0.08},{0.09,-0.08},{0.09,0.08},{-0.07,0.08}};

    this->swarmerAsset = {{0.05,0.01},{0.01,0.03},{-0.05,-0.0},{0.01,-0.03},{0.05,-0.01},{0.02,-0.01},{0.01,0.0},{0.02,0.01}};

    this->outerEngBall = {{-0.02,-0.04},{0.02,-0.04},{0.05,0.0},{0.02,0.04},{-0.02,0.04},{-0.05,-0.0}};
    this->innerEngBall = {{0.02,0.0},{0.01,0.02},{-0.01,0.02},{-0.02,-0.0},{-0.01,-0.02},{0.01,-0.02}};

    this->testPoly = {{-0.1,-0.1},{0.1,-0.1},{0.1,0.1},{-0.1,0.1}};
    this->testJoints = { jointInfo({0,1}, {0,-0.1}), jointInfo({2,3}, {0,0.1}) };
}
int Assets::getID(){
    this->colliderID++;
    return this->colliderID;
}