#include "miscGameObjs.h"
#include "components.h"
#include "config.h"
#include "assets.h"
using namespace std;

void Box::init(Assets* assets){
    this->rb.init(1.,0.5,0,0);
    this->rb.rotVel = 1;
    this->ID = 2;
    this->poly.init(&(assets->box),&this->rb,white);

}
void Box::update(Screen* screen, double dt){
    this->rb.update(dt);
    double time = SDL_GetTicks();
    double sinStuff =0.05 * sin( time/500);
    double negSinStuff = -sinStuff;
    this->poly.vertexOffsets[0] = {-sinStuff,-sinStuff};
    this->poly.vertexOffsets[2] = {sinStuff,sinStuff};
    this->poly.vertexOffsets[1] = {negSinStuff,-negSinStuff};
    this->poly.vertexOffsets[3] = {-negSinStuff,negSinStuff};
    this->poly.update();
}
void Box::render(Screen* screen){
    this->poly.render(screen);
}

void RiggedTest::init(Assets* assets){
    this->rp.init(&assets->testPoly,&assets->testJoints,white);
    this->ID = 3;

    this->rp.joints[0]->rb.setPos(-0.5,0);
    this->rp.joints[1]->rb.setPos(-0.5,0);
}
void RiggedTest::update(double dt){

    this->rp.joints[0]->rb.setRot(sin(0.001*SDL_GetTicks()));
    this->rp.joints[0]->rb.fixedDisplace(0,-0.1*sin(0.001*SDL_GetTicks()),dt);
    this->rp.joints[1]->rb.setRot(cos(0.001*SDL_GetTicks()));
    this->rp.joints[1]->rb.fixedDisplace(0,-0.1*cos(0.001*SDL_GetTicks()),dt);
    this->rp.update(dt);
}
void RiggedTest::render(Screen* screen){
    this->rp.render(screen);
}