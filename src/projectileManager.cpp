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
    this->vibePhase = stats->engBallVibePhase;


    this->explosionTotalTime = stats->engBallExpTimer;
    this->explosionTimeRemaining = -1;
    this->explosionVel = stats->engBallExpVel;
    this->explosionVelVarience= stats->engBallExpVelVarience;

    this->rb.rotVel = stats->engBallRotRate;

    this->innerPoly.init(&assets->innerEngBall,&this->rb,violet);
    this->outerPoly.init(&assets->outerEngBall,&this->rb,blue);

    this->active = false;
    this->exploding = false;
}

void EnergyBall::activate(tuple<int,int,int> innerColor,tuple<int,int,int> outerColor, 
                            int shooterID, complex<double> pos, complex<double> vel){
    this->shooterID = shooterID;
    this->rb.pos = pos;
    this->rb.vel = vel;

    this->innerPoly.color = innerColor;
    this->outerPoly.color = outerColor;

    this->active = true;
    this->exploding = false;
}
void EnergyBall::update(double dt){

    // checks if in process of exploding
    if(this->exploding){
        this->explode(dt);
    }else{
        this->prevPos = this->rb.pos;
        this->rb.update(dt);

        // inner polygon morphing
        double angle = this->vibeFreq * SDL_GetTicks();
        for (int i = 0; i < this->innerPoly.numVertices; i++){

            this->innerPoly.vertexOffsets[i] = this->innerPoly.assetRE[i];
            this->innerPoly.vertexOffsets[i]*= this->vibeAmp*sin(angle + this->vibePhase*M_PI*i/this->innerPoly.numVertices) /abs(this->innerPoly.assetRE[i]);
        }
    }
    this->outerPoly.update();
    this->innerPoly.update();
}
void EnergyBall::render(Screen* screen){
    this->innerPoly.render(screen);
    this->outerPoly.render(screen);
}
void EnergyBall::explode(double dt){
    this->explosionTimeRemaining-=dt;
    
    //fading
    this->innerPoly.lineThickness -= dt*defaultLineThickness/this->explosionTotalTime;
    this->outerPoly.lineThickness -= dt*defaultLineThickness/this->explosionTotalTime;
    if (this->innerPoly.lineThickness<0.01){
        this->innerPoly.lineThickness = 0.01;
    }
    if (this->outerPoly.lineThickness<0.01){
        this->outerPoly.lineThickness = 0.01;
    }
    //vertex mangling
    for(int i = 0; i <this->innerPoly.numVertices; i++){
        //explosion vel times dt, times unitvector outwards
        this->innerPoly.vertexOffsets[i] += this->explosionVel*dt* this->innerPoly.assetRE[i]/abs(this->innerPoly.assetRE[i]);
        this->innerPoly.vertexOffsets[i] += randComplex(explosionVelVarience)*dt;
    }
    for(int i = 0; i <this->outerPoly.numVertices; i++){
        this->outerPoly.vertexOffsets[i] += this->explosionVel*dt* this->outerPoly.assetRE[i]/abs(this->outerPoly.assetRE[i]);
        this->outerPoly.vertexOffsets[i] += randComplex(explosionVelVarience)*dt;
    }

    if (this->explosionTimeRemaining < 0){
        this->exploding = false;
        this->innerPoly.resetVertexOffsets();
        this->outerPoly.resetVertexOffsets();
        this->innerPoly.lineThickness=defaultLineThickness;
        this->outerPoly.lineThickness=defaultLineThickness;
    }
}
void EnergyBall::onCollision(){
    this->active = false;
    this->exploding = true;
    this->explosionTimeRemaining = this->explosionTotalTime;
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

    // check collision with bullets
    tuple<bool,int,complex<double>> helperReturnVal;
    for(int i = 0; i < bulletPoolSize; i++){
        helperReturnVal = willBulletHitPoly(poly,this->bullets[i], rb, ID, dt);
        // if collision occured
        if  (get<0>(helperReturnVal)){
            complex<double> normal = poly->getNormal(get<1>(helperReturnVal));
            complex<double> point = get<2>(helperReturnVal);
            complex<double> riccochetVel = this->bullets[i]->onCollision(ID,point,normal);
            this->collisionSparks(riccochetVel,point);
        }
    }

    // check collision with energy balls
    for(int i = 0; i < engBallPoolSize; i++){
        if (this->engBalls[i]->active && this->engBalls[i]->shooterID != ID){
            bool collisionOccured = isPointInPoly(poly,this->engBalls[i]->rb.pos);
            if (collisionOccured){
                this->engBalls[i]->onCollision();
            }
        }
    }


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
        // if energy ball is active or exploding
        if(this->engBalls[i]->active || this->engBalls[i]->exploding){
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
        //renders if active or exploding
        if(this->engBalls[i]->active || this->engBalls[i]->exploding){
            this->engBalls[i]->render(screen);
        }
    }
}