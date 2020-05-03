#define _USE_MATH_DEFINES
#include "player.h"
#include "assets.h"
#include "components.h"
#include "sdl.h"
#include <complex>
#include <vector>
#include <cmath>
#include <iostream>
using namespace std;

//theta-phi except wrappes around angle seem between 0 and 2phi, (note also returns sign)
double smallestAngle(double theta, double phi){
    double delta = std::abs(theta-phi);
    double oppSign = (-1)*(theta-phi) / delta;
    if (2*M_PI -delta > delta){
        return theta-phi;
    }
    else{
        return oppSign*(2*M_PI -delta);
    }
}


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
    std::complex<double> direction=0;

    if (keys[SDL_SCANCODE_D]){
        direction += {1,0};
    }
    if (keys[SDL_SCANCODE_A]){
        direction +={-1,0};
    }
    if (keys[SDL_SCANCODE_W]){
        direction +={0,1};
    }
    if (keys[SDL_SCANCODE_S]){
        direction +={0,-1};
    }

    if (std::abs(direction)!=0.0){
        direction/=std::abs(direction);
        double rotRate = 10*smallestAngle(std::arg(direction),this->rb.rot);
        this->rb.fixedRotate(rotRate,dt);

        this->rb.fixedDisplace(std::real(direction),std::imag(direction),dt);
    }



    if (keys[SDL_SCANCODE_Q]){
        this->rb.fixedRotate(3.0,dt);
    }
    if (keys[SDL_SCANCODE_E]){
        this->rb.fixedRotate(-3.0,dt);
    }
}