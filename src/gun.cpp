#include "gun.h"

void Gun::init(RigidBody* rb, Assets* assets,ProjectileManager* projMan, projectileType fireType, int ID, double bulletVel,double shotVarience){
    this->type = fireType;
    this->ID = ID;
    this->projMan = projMan;
    this->bulletVel = bulletVel;
    this->velVarience = shotVarience;
    this->rb.init(1.,0,0,0.0);
    this->shooterRb = rb;
    this->poly.init(&(assets->gunAsset),&this->rb,white);
}
void Gun::update(Screen* screen,double dt){
    int x,y;
    this->rb.pos = shooterRb->pos;
    SDL_GetMouseState(&x, &y);
    this->rb.setRot(arg(screen->pixelScreenToWorld(x,y) - this->rb.pos));
    this->rb.update(dt);
    this->poly.update();
}
void Gun::render(Screen* screen){
    this->poly.render(screen);
}
void Gun::fire(complex<double> fireDirection){
    //fire direction is now velocity vector
    fireDirection*=this->bulletVel/abs(fireDirection);
    switch(this->type){
        case (bullet): 
            this->projMan->fireBullet(white,orange, this->ID, this->rb.pos, fireDirection, this->velVarience);
        break;
        
        case(spark):
            this->projMan->fireSpark(orange,red, this->rb.pos, fireDirection, this->velVarience);
        break;
    }
}