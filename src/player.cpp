#include "player.h"
#include "assets.h"
#include "sdl.h"


void Player::init(Assets* assets){
    this->rb.init(1.,0,0,0.0);

    this->poly.init(255,255,255);
    assets->polygonLoader(&(this->poly),&(assets->plrAsset));
}
void Player::update(double dt){
    this->rb.update(dt);
    this->poly.update(&(this->rb));
}
void Player::render(Screen* screen){
    this->poly.render(screen);
}
void Player::events(SDL_Event* event,double dt){

}
void Player::keys(const Uint8* keys,double dt){
    if (keys[SDL_SCANCODE_D]){
        this->rb.applyForce(10,0,dt);
    }
    if (keys[SDL_SCANCODE_A]){
        this->rb.applyForce(-10,0,dt);
    }
    if (keys[SDL_SCANCODE_W]){
        this->rb.applyForce(0,10,dt);
    }
    if (keys[SDL_SCANCODE_S]){
        this->rb.applyForce(0,-10,dt);
    }
    if (keys[SDL_SCANCODE_Q]){
        this->rb.fixedRotate(3.0,dt);
    }
    if (keys[SDL_SCANCODE_E]){
        this->rb.fixedRotate(-3.0,dt);
    }
}