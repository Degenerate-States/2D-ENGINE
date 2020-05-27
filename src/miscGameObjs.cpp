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
    #if COLLISION_PRINT
    cout<<"Debug: Box hit"<<endl;
    #endif
}
int Box::getDamage(){
    return 0;
}


void SnakeTest::init(Assets* assets,double spf){
    this->snake.init(&assets->testSnakePoly,&assets->testSnakeJoints,red,1,3,assets->getID(),spf);
    this->snake.spawn({-1,-1},0,spf);
    //setsup poly collision callbacks
    this->snake.rp.poly.setCallBacks(
        bind(&SnakeTest::onCollision,this,_1,_2),
        bind(&SnakeTest::getDamage,this)
    );
}
void SnakeTest::update(double dt){
    double turn = sin(0.005*SDL_GetTicks());
    
    this->snake.update(turn,dt);
}
void SnakeTest::render(Screen* screen){
    this->snake.render(screen);
}
void SnakeTest::onCollision(int damage, complex<double> direction){
    #if COLLISION_PRINT
    cout<<"Debug: snake test hit"<<endl;
    #endif
}
int SnakeTest::getDamage(){
    return 0;
}



void SkeletonTest::init(Assets* assets){
    this->skele.init(&assets->sunPoly,&assets->sunJoints,&assets->sunLinks,red,assets->getID());
    this->skele.spawn({1,1},0);
    //setsup poly collision callbacks
    this->skele.rp.poly.setCallBacks(
        bind(&SkeletonTest::onCollision,this,_1,_2),
        bind(&SkeletonTest::getDamage,this)
    );
}
void SkeletonTest::update(double dt){
    this->skele.rb->setRot(M_PI*sin(0.001*SDL_GetTicks()));
    this->skele.update(dt);
}
void SkeletonTest::render(Screen* screen){
    this->skele.render(screen);
}
void SkeletonTest::onCollision(int damage, complex<double> direction){
    #if COLLISION_PRINT
    cout<<"Debug: skeleton test hit"<<endl;
    #endif
}
int SkeletonTest::getDamage(){
    return 0;
}