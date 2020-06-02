#include "gun.h"

extern Audio* audio;

void Gun::init(RigidBody* shooterRb, Assets* assets,ProjectileManager* projMan, GunStats* stats, int ID){
    this->ID = ID;
    this->projMan = projMan;
    this->rb.init(1.,0,0,0.0);
    this->shooterRb = shooterRb;
    this->poly.init(&(assets->gunAsset),&this->rb,white);
    this->equip(stats);
}

void Gun::equip(GunStats* stats) {
    //TODO: work the following into stat system
    this->type = stats->type;
    this->baseDamage = stats->baseDamage;
    this->projVel = stats->projVel;
    this->velVarience = stats->projVarience;
    this->fullAuto = stats->fullAuto;
    this->coolDown = stats->coolDown;
    this->projNum = stats->projNum;
    this->homingRate =stats->homingRate;
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

void Gun::fire(complex<double> fireDirection, RigidBody* target){
    if(SDL_GetTicks() - this->lastFired >this->coolDown){
        this->lastFired = SDL_GetTicks();
        //fire direction is now velocity vector
        fireDirection*=this->projVel/abs(fireDirection);

        // overrides target if no homing
        if(this-> homingRate == 0){
            target = NULL;
        }

        //fires "numshots" bullets
        for(int i = 0; i < this->projNum; i++){
            // its more efficet to put loop inside switch, but less legable 
            switch(this->type){
                case (bullet): 
                    this->projMan->fireBullet(white,orange, this->ID,this->baseDamage, this->rb.pos, fireDirection, this->velVarience,target,this->homingRate);
                    #if SOUND
                    audio->playSound(PEW1, VOLUME);
                    #endif
                break;

                case(spark):
                    this->projMan->fireSpark(orange,red, this->rb.pos, fireDirection, this->velVarience);
                    #if SOUND
                    audio->playSound(PEW1, VOLUME);
                    #endif
                break;

                case(energyBall):
                    this->projMan->fireEngBall(orange,red,this->ID,this->baseDamage, this->rb.pos, fireDirection, this->velVarience,target,this->homingRate);
                    #if SOUND
                    audio->playSound(PEW1, SDL_MIX_MAXVOLUME/2);
                    #endif
                break;
            }

        }
    }

}