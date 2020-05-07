#include "bulletManager.h"
#include "components.h"
#include "config.h"
#include "collisionFuncs.h"
#include <complex>
#include <tuple>
#include <iostream>
using namespace std;

//************************//
//**   Bullet Methods   **//
//************************//
void Bullet::init(Config* cfg){
    this->rb.init(1.0,0.0,0.0,0.0);
    //overwritten by activate
    this->pnt.init(white,2.0);
    this->trail.init(cfg->bulletTrailSegments,cfg->bulletTrailDecay);

    this->active = false;
}

void Bullet::activate(tuple<int,int,int> headColor,tuple<int,int,int> tailColor, int shooterID,double diameter, double mass, complex<double> pos, complex<double> vel){
    this->shooterID = shooterID;
    this->rb.pos = pos;
    this->rb.vel = vel;
    this->rb.mass =mass;

    this->pnt.changeColor(headColor);
    this->pnt.diameter = diameter;
    this->trail.reset(&this->rb,diameter,headColor,tailColor);

    this->active = true;
}
void Bullet::update(double dt){
    this->rb.update(dt);
    this->trail.update(&this->rb,dt);
}
void Bullet::render(Screen* screen){
    this->trail.render(screen);
    this->pnt.render(screen,&this->rb);
}


//************************//
//**BulletManager Methods**//
//************************//
void BulletManager::init(Config* cfg){
    this->oldestBulletIndex = 0;
    for(int i = 0; i < bulletPoolSize; i++){
        this->bullets[i] = new Bullet();
        this->bullets[i]->init(cfg);
    }
}
void BulletManager::fireBullet(tuple<int,int,int> headColor,tuple<int,int,int> tailColor, int shooterID,double diameter, double mass,complex<double> pos, complex<double> dirVec, double speed){
    //direction vec is now velocity vec
    dirVec *= speed/abs(dirVec);

    this->bullets[this->oldestBulletIndex]->activate(headColor,tailColor, shooterID, diameter, mass, pos, dirVec);
    // change which bullet is considered the oldest
    this->oldestBulletIndex+=1;
    this->oldestBulletIndex%=bulletPoolSize;
}
//unfinished, not using proper system
void BulletManager::checkCollisionPoly(int ID, RigidBody* rb,Polygon* poly,double dt){

    tuple<bool,int,complex<double>> helperReturnVal;
    complex<double> bulletPos;
    complex<double> bulletPosNext;
    double possibleCollisonRad;

    for(int i = 0; i < bulletPoolSize; i++){
        
        //checks if bullet is active and poly isnt the one who fired it
        if(this->bullets[i]->active && ID != this->bullets[i]->shooterID){

            bulletPos = this->bullets[i]->rb.pos;
            bulletPosNext = bulletPos + this->bullets[i]->rb.vel;

            // checks if bullet will be in circle containing polygon
            possibleCollisonRad = abs(rb->pos - bulletPosNext) +abs(rb->vel*(dt+pntCollisionPadTemporal)) + pntCollisionPadSpatial;
            if (possibleCollisonRad > poly->smallestRadius){

                //extensive collision checking
                
            }
            

        }
    }
}
void BulletManager::update(double dt){
    for(int i = 0; i < bulletPoolSize; i++){
        if(this->bullets[i]->active){
            this->bullets[i]->update(dt);
        }
    }

}
void BulletManager::render(Screen* screen){
    for(int i = 0; i < bulletPoolSize; i++){
        if(this->bullets[i]->active){
            this->bullets[i]->render(screen);
        }
    }

}