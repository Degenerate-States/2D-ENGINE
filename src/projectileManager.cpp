#include "projectileManager.h"
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
    this->type = bullet;
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
complex<double> Bullet::onCollision(int hitID,complex<double> collisionPoint, complex<double> collisionNormal){
    this->prevPos = collisionPoint;
    this->shooterID = hitID;
    // undoes update movement, you could also move bullet to collision point, although this may cause double hits
    this->rb.pos = this->prevPos;

    // reflects velocity
    this->rb.vel = reflectAboutNormal(collisionNormal , this->rb.vel);
    
    //returns riccochet direction
    return this->rb.vel;
}


//************************//
//**   Spark Methods    **//
//************************//
void Spark::init(Stats* stats){
    this->type = spark;
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
//*  EnergyBall Methods **//
//************************//
void EnergyBall::init(Assets* assets,Stats* stats){    
    this->type = energyBall;
    this->rb.init(1.0,0.0,0.0,0.0);
    this->prevPos=rb.pos;

    this->vibeAmp = stats->engBallVibrateAmplitude;
    this->vibeFreq = stats->engBallVibrateFreq;

    this->rb.rotVel = stats->engBallRotRate;

    this->innerPoly.init(&assets->innerEngBall,&this->rb,violet);
    this->outerPoly.init(&assets->outerEngBall,&this->rb,blue);

    this->active = false;
}

void EnergyBall::activate(tuple<int,int,int> innerColor,tuple<int,int,int> outerColor, 
                            int shooterID, complex<double> pos, complex<double> vel){
    this->shooterID = shooterID;
    this->rb.pos = pos;
    this->rb.vel = vel;

    this->innerPoly.color = innerColor;
    this->outerPoly.color = outerColor;

    this->active = true;
}
void EnergyBall::update(double dt){
    this->prevPos = this->rb.pos;
    this->rb.update(dt);

    // inner polygon morphing
    double angle = this->vibeFreq * SDL_GetTicks();
    int numVertices = this->innerPoly.vertexOffsets.size();
    for (int i = 0; i < this->innerPoly.vertexOffsets.size(); i++){
        this->innerPoly.vertexOffsets[i] = this->innerPoly.assetRE[i];
        this->innerPoly.vertexOffsets[i]*= this->vibeAmp*sin(angle + 3.5*M_PI*i/numVertices) /abs(this->innerPoly.assetRE[i]);
    }

    this->outerPoly.update();
    this->innerPoly.update();
}
void EnergyBall::render(Screen* screen){
    this->innerPoly.render(screen);
    this->outerPoly.render(screen);
}



//*****************************//
//**ProjectileManager Methods**//
//*****************************//
void ProjectileManager::init(Assets* assets,Stats* stats){
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

    //initalizes energy balls
    this->oldestEngBallIndex = 0;
    for(int i = 0; i < engBallPoolSize; i++){
        this->engBalls[i] = new EnergyBall();
        this->engBalls[i]->init(assets,stats);
    }
}
void ProjectileManager::fireBullet(tuple<int,int,int> headColor,tuple<int,int,int> tailColor, int shooterID, 
                                complex<double> pos, complex<double> vel, double velVarience){
    //applies velocity varience
    vel += randComplex(velVarience);        

    this->bullets[this->oldestBulletIndex]->activate(headColor,tailColor, shooterID, pos, vel);
    // change which bullet is considered the oldest
    this->oldestBulletIndex+=1;
    this->oldestBulletIndex%=bulletPoolSize;
}
void ProjectileManager::fireSpark(tuple<int,int,int> headColor,tuple<int,int,int> tailColor,
                                complex<double> pos, complex<double> vel,double velVarience){

    //applies velocity varience
    vel += randComplex(velVarience);

    this->sparks[this->oldestSparkIndex]->activate(headColor,tailColor, pos, vel);
    // change which bullet is considered the oldest
    this->oldestSparkIndex+=1;
    this->oldestSparkIndex%=sparkPoolSize;
}
void ProjectileManager::fireEngBall(tuple<int,int,int> innerColor,tuple<int,int,int> outerColor,int shooterID, 
                                        complex<double> pos, complex<double> vel,double velVarience){
    //applies velocity varience
    vel += randComplex(velVarience);

    this->engBalls[this->oldestEngBallIndex]->activate(innerColor,outerColor, shooterID, pos, vel);
    // change which bullet is considered the oldest
    this->oldestEngBallIndex+=1;
    this->oldestEngBallIndex%=engBallPoolSize;
}
//TODO: clean up following method and make it acecessable for use with other projectile types
//      this can be done by moving the collision heuristic to willBulletHitPoly. in general
//      having such collision functions in the collision helpers file seems like the best approch
void ProjectileManager::checkCollisionPoly(int ID, RigidBody* rb,Polygon* poly,double dt){

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
                    complex<double> normal = poly->getNormal(get<1>(helperReturnVal));
                    complex<double> point = get<2>(helperReturnVal);
                    complex<double> riccochetVel = this->bullets[i]->onCollision(ID,point,normal);
                    this->collisionSparks(riccochetVel,point);
                }
        
            }// end of if collision is possilbe

        }// end of if bullet is active

    }// end of bullet loop
}
void ProjectileManager::collisionSparks(complex<double> direction,complex<double> point){
    double riccochetMag = abs(direction);
    complex<double> sparkVel = direction/this->riccoSparkVelDamping;
    for(int i = 0; i < (int)(riccochetMag/this->riccoSparkSpawnDamping); i++){
        this->fireSpark(orange,red,point,sparkVel,this->sparkVelVarience);
    }
}

void ProjectileManager::update(double dt){
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
    //energy balls
    for(int i = 0; i < engBallPoolSize; i++){
        if(this->engBalls[i]->active){
            this->engBalls[i]->update(dt);
        }
    }

}
void ProjectileManager::render(Screen* screen,double dt){
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
    //energy balls
    for(int i = 0; i < engBallPoolSize; i++){
        if(this->engBalls[i]->active){
            this->engBalls[i]->render(screen);
        }
    }
}