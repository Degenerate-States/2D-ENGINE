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
    this->prevPos=rb.pos;
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
    this->prevPos = this->rb.pos;
    this->rb.update(dt);
    
}
void Bullet::render(Screen* screen,double dt){
    this->trail.update(&this->rb,dt);
    this->trail.render(screen);
    this->pnt.render(screen,&this->rb);
}
void Bullet::riccochet(complex<double> normal,double dt){
    // undoes update movement, you could also move bullet to collision point, although this may cause double hits
    this->rb.pos = this->prevPos;
    // reflects velocity
    this->rb.vel = reflectAboutNormal(normal , this->rb.vel);
    // updates bullet
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
void BulletManager::checkCollisionPoly(int ID, RigidBody* rb,Polygon* poly,double dt){

    tuple<bool,int,complex<double>> helperReturnVal;
    complex<double> bulletPos;
    complex<double> bulletPosNext;
    double possibleCollisonRad;

    for(int i = 0; i < bulletPoolSize; i++){
        
        //checks if bullet is active and poly isnt the one who fired it
        if(this->bullets[i]->active && ID != this->bullets[i]->shooterID){
            bulletPos = this->bullets[i]->prevPos;
            bulletPosNext = this->bullets[i]->rb.pos;

            // checks if bullet will be in circle containing polygon
            possibleCollisonRad = abs(rb->pos - bulletPosNext) +abs(rb->vel*(dt+pntCollisionPadTemporal)) + pntCollisionPadSpatial;
            if (possibleCollisonRad < poly->getSmallestRadius()){
                //extensive collision checking
                helperReturnVal = willBulletHitPoly(poly,bulletPos, bulletPosNext);

                // bullet riccochet
                if  (get<0>(helperReturnVal)){
                    this->bullets[i]->prevPos = get<2>(helperReturnVal);
                    this->bullets[i]->shooterID = ID;
                    this->bullets[i]->riccochet( poly->getNormal(get<1>(helperReturnVal)) , dt);
                }
                
            }// end of if collision is possilbe

        }// end of if bullet is active

    }// end of for loop
}
void BulletManager::update(double dt){
    for(int i = 0; i < bulletPoolSize; i++){
        if(this->bullets[i]->active){
            this->bullets[i]->update(dt);
        }
    }

}
void BulletManager::render(Screen* screen,double dt){
    for(int i = 0; i < bulletPoolSize; i++){
        if(this->bullets[i]->active){
            this->bullets[i]->render(screen,dt);
        }
    }
}