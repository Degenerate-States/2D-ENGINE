#include "bulletManager.h"
#include "components.h"
#include "config.h"
#include "collisionFuncs.h"
#include <complex>
#include <tuple>
#include <iostream>
using namespace std;

double randDouble(double max){
    return max* (double)rand()/RAND_MAX;
}

complex<double> randComplex(double maxMag){
    double randAngle = randDouble(2*M_PI);
    complex<double> val = {cos(randAngle),sin(randAngle)};
    //randomizes length
    return randDouble(maxMag)*val;
}


//************************//
//**   Bullet Methods   **//
//************************//
void Bullet::init(Stats* stats){
    this->rb.init(1.0,0.0,0.0,0.0);
    this->prevPos=rb.pos;

    this->pnt.init(&this->rb,white,stats->bulletDiameter);
    this->trail.init(&this->rb,stats->bulletDiameter,stats->bulletTrailSegments,stats->bulletTrailDecay);

    this->active = false;
}

void Bullet::activate(tuple<int,int,int> headColor,tuple<int,int,int> tailColor, int shooterID, complex<double> pos, complex<double> vel){
    this->shooterID = shooterID;
    this->rb.pos = pos;
    this->rb.vel = vel;

    this->pnt.changeColor(headColor);
    this->trail.reset(headColor,tailColor);

    this->active = true;
}
void Bullet::update(double dt){
    this->prevPos = this->rb.pos;
    this->rb.update(dt);
    
}
void Bullet::render(Screen* screen,double dt){
    this->trail.update(dt);
    this->trail.render(screen);
    this->pnt.render(screen);
}
complex<double> Bullet::riccochet(complex<double> normal){
    // undoes update movement, you could also move bullet to collision point, although this may cause double hits
    this->rb.pos = this->prevPos;
    // reflects velocity
    this->rb.vel = reflectAboutNormal(normal , this->rb.vel);
    
    return this->rb.vel;
}

//************************//
//**   Spark Methods    **//
//************************//
void Spark::init(Stats* stats){
    this->rb.init(1.0,0.0,0.0,0.0);

    this->pnt.init(&this->rb,white,stats->sparkDiameter);
    this->trail.init(&this->rb,stats->sparkDiameter,stats->bulletTrailSegments,stats->bulletTrailDecay);

    this->active = false;
}

void Spark::activate(tuple<int,int,int> headColor,tuple<int,int,int> tailColor, 
                    complex<double> pos, complex<double> vel){
    this->rb.pos = pos;
    this->rb.vel = vel;

    this->pnt.changeColor(headColor);
    this->trail.reset(headColor,tailColor);

    this->active = true;
}
void Spark::update(double dt){
    this->rb.update(dt);
    
}
void Spark::render(Screen* screen,double dt){
    this->trail.update(dt);
    this->trail.render(screen);
    this->pnt.render(screen);
}


//************************//
//**BulletManager Methods**//
//************************//
void BulletManager::init(Stats* stats){
    this->riccoSparkSpawnDamping = stats->riccoSparkSpawnDamping;
    this->riccoSparkVelDamping = stats->riccoSparkVelDamping;
    this->sparkVelVarience=stats->sparkVelVarience;

    //initalize bullets
    this->oldestBulletIndex = 0;
    for(int i = 0; i < bulletPoolSize; i++){
        this->bullets[i] = new Bullet();
        this->bullets[i]->init(stats);
    }

    //initalize sparks
    this->oldestSparkIndex = 0;
    for(int i = 0; i < sparkPoolSize; i++){
        this->sparks[i] = new Spark();
        this->sparks[i]->init(stats);
    }
}
void BulletManager::fireBullet(tuple<int,int,int> headColor,tuple<int,int,int> tailColor, int shooterID, 
                                complex<double> pos, complex<double> dirVec, double speed){
    //direction vec is now velocity vec
    dirVec *= speed/abs(dirVec);

    this->bullets[this->oldestBulletIndex]->activate(headColor,tailColor, shooterID, pos, dirVec);
    // change which bullet is considered the oldest
    this->oldestBulletIndex+=1;
    this->oldestBulletIndex%=bulletPoolSize;
}
void BulletManager::fireSpark(tuple<int,int,int> headColor,tuple<int,int,int> tailColor,
                                complex<double> pos, complex<double> vel,double velVarience){

    //applies velocity varience
    vel += randComplex(velVarience);

    this->sparks[this->oldestSparkIndex]->activate(headColor,tailColor, pos, vel);
    // change which bullet is considered the oldest
    this->oldestSparkIndex+=1;
    this->oldestSparkIndex%=sparkPoolSize;
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

                // if collision occured
                if  (get<0>(helperReturnVal)){
                    this->onCollsion(i,ID,get<2>(helperReturnVal),poly->getNormal(get<1>(helperReturnVal)));
                }
        
            }// end of if collision is possilbe

        }// end of if bullet is active

    }// end of for loop
}
void BulletManager::onCollsion(int bulletIndex,int hitID,complex<double> collisionPoint, complex<double> collisionNormal){
    //bullet riccochet
    this->bullets[bulletIndex]->prevPos = collisionPoint;
    this->bullets[bulletIndex]->shooterID = hitID;
    complex<double> riccochetDirection = this->bullets[bulletIndex]->riccochet(collisionNormal);

    //spark spawning
    double riccochetMag = abs(riccochetDirection);
    complex<double> sparkVel = riccochetDirection/this->riccoSparkVelDamping;
    for(int i = 0; i < (int)(riccochetMag/this->riccoSparkSpawnDamping); i++){
        this->fireSpark(orange,red,collisionPoint,sparkVel,this->sparkVelVarience);
    }
}

void BulletManager::update(double dt){
    //bullets
    for(int i = 0; i < bulletPoolSize; i++){
        if(this->bullets[i]->active){
            this->bullets[i]->update(dt);
        }
    }
    //sparks
    for(int i = 0; i < sparkPoolSize; i++){
        if(this->sparks[i]->active){
            this->sparks[i]->update(dt);
        }
    }

}
void BulletManager::render(Screen* screen,double dt){
    for(int i = 0; i < bulletPoolSize; i++){
        if(this->bullets[i]->active){
            this->bullets[i]->render(screen,dt);
        }
    }
    //sparks
    for(int i = 0; i < sparkPoolSize; i++){
        if(this->sparks[i]->active){
            this->sparks[i]->render(screen,dt);
        }
    }
}