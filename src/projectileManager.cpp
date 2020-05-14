#include "projectileManager.h"
#include "collisionFuncs.h"

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

    this->pnt.init(&this->rb,white,stats->bulletDiameter);
    this->trail.init(&this->rb,stats->bulletDiameter,stats->bulletTrailSegments,stats->bulletTrailDecay);

    this->riccoVelDamping = stats->riccoBulletVelDamping;
    this->velDamping = stats->bulletVelDamping;
    this->minVel = stats->bulletMinVel;

    this->startDiameter = stats->bulletDiameter;
    
    this->rb.active = false;

    //setsup poly collision callbacks
    this->pnt.setCallBacks(
        bind(&Bullet::onCollision,this,_1,_2),
        bind(&Bullet::getDamage,this)
    );
}

void Bullet::activate(tuple<int,int,int> headColor,tuple<int,int,int> tailColor, int shooterID, 
                        complex<double> pos, complex<double> vel, RigidBody* homingTarget, double homingRate){
    this->rb.pos = pos;
    this->rb.vel = vel;

    this->pnt.changeColor(headColor);
    this->pnt.colliderID = shooterID;
    this->trail.reset(headColor,tailColor);

    this->homingTarget = homingTarget;
    this->homingRate = homingRate;
    this->spawnSpeed = abs(vel);
    this->rb.active = true;
}

void Bullet::update(double dt){
    //TODO; increase efficency of process and have it called periodically rather than constantly 
    // homing
    if(this->homingTarget != NULL){
        if(this->homingTarget->active){
            double angle =smallestAngle(arg(this->homingTarget->pos - this->rb.pos), arg(this->rb.vel));
            //gets angles sign
            angle/=abs(angle);
            complex<double> rotation = {cos(angle*this->homingRate*dt),sin(angle*this->homingRate*dt)};
            this->rb.vel*=rotation;
        }
    }

    double speed = abs(this->rb.vel);
    this->rb.vel -= velDamping*dt*this->rb.vel;
    double newDiameter = this->startDiameter*(speed)/ this->spawnSpeed;
    this->trail.headThickness =newDiameter;
    this->pnt.diameter = newDiameter;

    this->rb.update(dt);
    if(speed < this->minVel){
        this->rb.active = false;
    }
    
}

void Bullet::render(Screen* screen,double dt){
    this->trail.update(dt);
    this->trail.render(screen);
    this->pnt.render(screen);
}

void Bullet::onCollision(int damage, complex<double> collisionNormal){
    //disables homing on hit
    this->homingTarget = NULL;

    // reflects velocity and damps it
    this->rb.vel = this->riccoVelDamping*reflectAboutNormal(collisionNormal , this->rb.vel);
}
int Bullet::getDamage(){
    return 0;
}


//************************//
//**   Spark Methods    **//
//************************//
void Spark::init(Stats* stats){
    this->type = spark;
    this->rb.init(1.0,0.0,0.0,0.0);

    this->pnt.init(&this->rb,white,stats->sparkDiameter);
    this->trail.init(&this->rb,stats->sparkDiameter,stats->bulletTrailSegments,stats->bulletTrailDecay);

    this->velDamping = stats->sparkVelDamping;
    this->startDiameter = stats->sparkDiameter;
    this->minVel = stats->sparkMinVel;
    this->rb.active = false;
}

void Spark::activate(tuple<int,int,int> headColor,tuple<int,int,int> tailColor, 
                    complex<double> pos, complex<double> vel){
    this->rb.pos = pos;
    this->rb.vel = vel;

    this->pnt.changeColor(headColor);
    this->trail.reset(headColor,tailColor);
    this->spawnSpeed = abs(vel);
    this->rb.active = true;
}

void Spark::update(double dt){
    double speed = abs(this->rb.vel);
    this->rb.vel -= velDamping*dt*this->rb.vel;

    double newDiameter = this->startDiameter*(speed)/ this->spawnSpeed;
    this->trail.headThickness =newDiameter;
    this->pnt.diameter = newDiameter;

    this->rb.update(dt);
    if(speed < this->minVel){
        this->rb.active = false;
    }
    
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

    this->pnt.init(&this->rb,black,1);
    this->rb.active = false;
    this->exploding = false;

    //setsup poly collision callbacks
    this->pnt.setCallBacks(
        bind(&EnergyBall::onCollision,this,_1,_2),
        bind(&EnergyBall::getDamage,this)
    );
}

void EnergyBall::activate(tuple<int,int,int> innerColor,tuple<int,int,int> outerColor, int shooterID, 
                        complex<double> pos, complex<double> vel, RigidBody* homingTarget, double homingRate){
    this->rb.pos = pos;
    this->rb.vel = vel;

    this->innerPoly.color = innerColor;
    this->outerPoly.color = outerColor;

    this->rb.active = true;
    this->pnt.colliderID = shooterID;

    this->exploding = false;
    this->innerPoly.resetVertexOffsets();
    this->outerPoly.resetVertexOffsets();
    this->innerPoly.lineThickness=defaultLineThickness;
    this->outerPoly.lineThickness=defaultLineThickness;

    this->homingTarget = homingTarget;
    this->homingRate = homingRate;
}

void EnergyBall::update(double dt){

    // checks if in process of exploding
    if(this->exploding){
        this->explode(dt);
    }else{


        //TODO; increase efficency of process and have it called periodically rather than constantly 
        // homing
        if(this->homingTarget != NULL){
            if(this->homingTarget->active){
                double angle =smallestAngle(arg(this->homingTarget->pos - this->rb.pos), arg(this->rb.vel));
                //gets angles sign
                angle/=abs(angle);
                complex<double> rotation = {cos(angle*this->homingRate*dt),sin(angle*this->homingRate*dt)};
                this->rb.vel*=rotation;
            }
        }

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
    }
}

void EnergyBall::onCollision(int damage, complex<double> direction){
    this->rb.active = false;
    this->exploding = true;
    this->explosionTimeRemaining = this->explosionTotalTime;
}
int EnergyBall::getDamage(){
    return 0;
}



//*****************************//
//**ProjectileManager Methods**//
//*****************************//
void ProjectileManager::init(Assets* assets,Stats* stats){
    this->riccoSparkSpawnDamping = stats->riccoSparkSpawnDamping;
    this->riccoSparkVelDamping = stats->riccoSparkVelDamping;
    this->riccoSparkVelVarience=stats->riccoSparkVelVarience;

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
        complex<double> pos, complex<double> vel, double velVarience, RigidBody* homingTarget,double homingRate){
    //applies velocity varience
    vel += randComplex(velVarience);        

    this->bullets[this->oldestBulletIndex]->activate(headColor,tailColor, shooterID, pos, vel,homingTarget,homingRate);
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

void ProjectileManager::fireEngBall(tuple<int,int,int> innerColor,tuple<int,int,int> outerColor, int shooterID, complex<double> pos, 
                        complex<double> vel, double velVarience, RigidBody* homingTarget,double homingRate){
    //applies velocity varience
    vel += randComplex(velVarience);
    this->engBalls[this->oldestEngBallIndex]->activate(innerColor,outerColor, shooterID, pos, vel, homingTarget, homingRate);
    // change which bullet is considered the oldest
    this->oldestEngBallIndex+=1;
    this->oldestEngBallIndex%=engBallPoolSize;
}

void ProjectileManager::checkCollisionPoly(Polygon* poly){

    // check collision with bullets
    tuple<bool,int,complex<double>> helperReturnVal;
    for(int i = 0; i < bulletPoolSize; i++){
        pointPolyCollision(poly, &this->bullets[i]->pnt, pointHitPoly);
    }

    // check collision with energy balls
    for(int i = 0; i < engBallPoolSize; i++){
        pointPolyCollision(poly,&this->engBalls[i]->pnt, pointInPoly);
    }
}

void ProjectileManager::collisionSparks(complex<double> direction,complex<double> point){
    double riccochetMag = abs(direction);
    complex<double> sparkVel = direction*this->riccoSparkVelDamping;
    for(int i = 0; i < (int)(riccochetMag*this->riccoSparkSpawnDamping); i++){
        this->fireSpark(orange,red,point,sparkVel,this->riccoSparkVelVarience);
    }
}

void ProjectileManager::update(double dt){
    //bullets
    for(int i = 0; i < bulletPoolSize; i++){
        if(this->bullets[i]->rb.active){
            this->bullets[i]->update(dt);
        }
    }
    //sparks
    for(int i = 0; i < sparkPoolSize; i++){
        if(this->sparks[i]->rb.active){
            this->sparks[i]->update(dt);
        }
    }
    //energy balls
    for(int i = 0; i < engBallPoolSize; i++){
        // if energy ball is active or exploding
        if(this->engBalls[i]->rb.active || this->engBalls[i]->exploding){
            this->engBalls[i]->update(dt);
        }
    }
}

void ProjectileManager::render(Screen* screen,double dt){
    for(int i = 0; i < bulletPoolSize; i++){
        if(this->bullets[i]->rb.active){
            this->bullets[i]->render(screen,dt);
        }
    }
    //sparks
    for(int i = 0; i < sparkPoolSize; i++){
        if(this->sparks[i]->rb.active){
            this->sparks[i]->render(screen,dt);
        }
    }
    //energy balls
    for(int i = 0; i < engBallPoolSize; i++){
        //renders if active or exploding
        if(this->engBalls[i]->rb.active || this->engBalls[i]->exploding){
            this->engBalls[i]->render(screen);
        }
    }
}