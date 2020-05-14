#include "miscGameObjs.h"


void Box::init(Assets* assets){
    this->rb.init(1.,0.5,0,0);
    this->rb.rotVel = 1;
    this->poly.init(&(assets->box),&this->rb,white,assets->getID());


    //setsup poly collision callbacks
    this->poly.setCallBacks(
        bind(&Box::onCollision,this,_1,_2),
        bind(&Box::getDamage,this)
    );
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
void Box::onCollision(int damage, complex<double> direction){
    cout<<"Debug: Box hit"<<endl;
}
int Box::getDamage(){
    return 0;
}

void RiggedTest::init(Assets* assets){
    this->rp.init(&assets->testPoly,&assets->testJoints,white,assets->getID());

    this->rp.joints[0]->rb.setPos(-0.5,0);
    this->rp.joints[1]->rb.setPos(-0.5,0);
    
    //setsup poly collision callbacks
    this->rp.poly.setCallBacks(
        bind(&RiggedTest::onCollision,this,_1,_2),
        bind(&RiggedTest::getDamage,this)
    );
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
void RiggedTest::onCollision(int damage, complex<double> direction){
    cout<<"Debug: rigged test hit"<<endl;
}
int RiggedTest::getDamage(){
    return 0;
}