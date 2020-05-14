#include "player.h"


void Flame::init(RigidBody* plrRb, Assets* assets,ProjectileManager* projMan){
    this->plrRb = plrRb;
    this->rb.init(1.0,0,0,0);
    this->poly.init(&assets->flameAsset,&this->rb,red);
    this->projMan = projMan;
}

void Flame::update(Screen* screen, double dt){
    this->rb.setRot(this->plrRb->rot);
    this->rb.pos = this->plrRb->pos;

    double flameLen = fmod(-0.001*SDL_GetTicks(),0.12) +0.08;
    this->poly.vertexOffsets[0] = flameLen;


    this->rb.update(dt);
    this->poly.update();
}

void Flame::render(Screen* screen){
    this->poly.render(screen);
}

void Player::init(Assets* assets,ProjectileManager* projMan, Stats* stats){
    this->rb.init(1.,0,0,0.0);
    this->relScreenPos = 0.0;
    this->screenOffset = stats->screenOffset;
    this->screenVel = stats->screenVel;

    this->topSpeed = stats->plrTopSpeed;
    this->acceleration = stats->plrAcceleration;

    this->poly.init(&assets->plrAsset,&this->rb,white,assets->getID());

    this->gun.init(&this->rb, assets, projMan, bullet, &stats->plrGun, this->poly.colliderID);
    this->flame.init(&this->rb,assets,projMan);

    //setsup poly collision callbacks
    this->poly.setCallBacks(
        bind(&Player::onCollision,this,_1,_2),
        bind(&Player::getDamage,this)
    );
}

void Player::update(Screen* screen,double dt){
    //points in direction of motion
    this->rb.setRot(arg(this->rb.vel));

    //clamps velocity
    if (abs(this->rb.vel) > this->topSpeed){
        this->rb.vel *= this->topSpeed/abs(this->rb.vel);
    }

    this->rb.update(dt);
    this->poly.update();
    this->gun.update(screen, dt);
    this->flame.update(screen,dt);
}

void Player::render(Screen* screen){
    this->poly.render(screen);
    this->gun.render(screen);
    this->flame.render(screen);
}

void Player::events(SDL_Event* event, Screen* screen,double dt){
    //semi auto firing
    // if gun is semi auto right mouse was pressed
    if((!this->gun.fullAuto) && event->type == SDL_MOUSEBUTTONDOWN){
        if (event->button.button == SDL_BUTTON_LEFT){
            complex<double> fireDirection = screen->pixelScreenToWorld(event->button.x,event->button.y) - this->rb.pos;
            
            this->gun.fire(fireDirection);
        }
    }
}

void Player::keys(const Uint8* keys,Screen* screen,double dt){
    complex<double> direction = 0.0;
    // TODO: move key specifics to the Keys callback in game.cpp
    if (keys[SDL_SCANCODE_D]){
        direction += {1,0};
    }
    if (keys[SDL_SCANCODE_A]){
        direction += {-1,0};
    }
    if (keys[SDL_SCANCODE_W]){
        direction += {0,1};
    }
    if (keys[SDL_SCANCODE_S]){
        direction += {0,-1};
    }

    if (abs(direction)!=0.0){
        direction*= this->acceleration/abs(direction);
        this->rb.applyForce(real(direction),imag(direction),dt);
    }

    //full auto firing
    if(this->gun.fullAuto){
        int x,y;
        if (SDL_GetMouseState(&x,&y) & SDL_BUTTON(SDL_BUTTON_LEFT)){
            complex<double> fireDirection = screen->pixelScreenToWorld(x,y) - this->rb.pos;

            this->gun.fire(fireDirection);
        }
    }
}
void Player::setScreenPos(Screen* screen, double dt){
    // direction of motion is from where screen is to where player is trusting times offset size
    this->relScreenPos = this->screenOffset*this->rb.vel/this->topSpeed;
    // sets screen posisiton to player posistion plus relative posisiton
    screen->rb.pos = this->rb.pos + this->relScreenPos;
}
void Player::onCollision(int damage, complex<double> direction){

}
int Player::getDamage(){
    return 0;
}