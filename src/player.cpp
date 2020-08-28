#include "player.h"


void Flame::init(RigidBody* plrRb, Assets* assets,ProjectileManager* projMan){
    this->plrRb = plrRb;
    this->skele.init(&assets->flameAsset,&assets->flameJoints,&assets->flameLinks,red,-1);
    this->projMan = projMan;
}

void Flame::update(Screen* screen, double dt){
    this->skele.rb->setRot(this->plrRb->rot);
    this->skele.rb->pos = this->plrRb->pos;

    double flameLen = fmod(-0.001*SDL_GetTicks(),0.12) +0.08;
    this->skele.rp.poly.vertexOffsets[0] = flameLen;

    this->skele.update(dt);
}

void Flame::render(Screen* screen){
    this->skele.render(screen);
}

void Player::init(Assets* assets,ProjectileManager* projMan, Stats* stats){
    this->rb.init(1.,0,0,0.0);
    this->relScreenPos = 0.0;
    this->screenOffset = stats->screenOffset;
    this->screenVel = stats->screenVel;

    this->topSpeed = stats->plrTopSpeed;
    this->acceleration = stats->plrAcceleration;

    this->poly.init(&assets->plrAsset,&this->rb,white,assets->getID());

    // TEMP: gunBar
    this->gunBar.push_back(stats->mg);
    this->gunBar.push_back(stats->smg);
    this->gunBar.push_back(stats->shotgun);
    this->gunBar.push_back(stats->autoShotgun);
    this->gunBar.push_back(stats->sniper);
    this->gunBar.push_back(stats->energyRifle);
    this->gunBar.push_back(stats->energyShotgun);

    this->gun.init(&this->rb, assets, projMan, &gunBar[0], this->poly.colliderID);
    this->flame.init(&this->rb,assets,projMan);

    this->contactDamage = stats->plrContactDamage;
    this->startHealth = stats->plrHealth;
    //setsup poly collision callbacks
    this->poly.setCallBacks(
        bind(&Player::onCollision,this,_1,_2),
        bind(&Player::getDamage,this)
    );
}
void Player::spawn(complex<double> pos){
    this->rb.active = true;
    this->rb.pos = pos;
    this->health = this->startHealth;
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
        direction += complex<double>(1,0);
    }
    if (keys[SDL_SCANCODE_A]){
        direction +=complex<double>(-1,0);
    }
    if (keys[SDL_SCANCODE_W]){
        direction +=complex<double>(0,1);
    }
    if (keys[SDL_SCANCODE_S]){
        direction +=complex<double>(0,-1);
    }

    // TEMP
    if (keys[SDL_SCANCODE_1]){
        this->gun.equip(&gunBar[0]);
    }
    if (keys[SDL_SCANCODE_2]){
        this->gun.equip(&gunBar[1]);
    }
    if (keys[SDL_SCANCODE_3]){
        this->gun.equip(&gunBar[2]);
    }
    if (keys[SDL_SCANCODE_4]){
        this->gun.equip(&gunBar[3]);
    }
    if (keys[SDL_SCANCODE_5]){
        this->gun.equip(&gunBar[4]);
    }
    if (keys[SDL_SCANCODE_6]){
        this->gun.equip(&gunBar[5]);
    }
    if (keys[SDL_SCANCODE_7]){
        this->gun.equip(&gunBar[6]);
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
    this->health-=damage;
    #if COLLISION_PRINT
    cout<<"Debug: Player hit...   Health:"<<" "<<this->health<<endl;
    #endif
}
int Player::getDamage(){
    return this->contactDamage;
}
void Player::die(){
    this->rb.active = false;
    #if COLLISION_PRINT
    cout<<"Debug: Player Died";
    #endif
}