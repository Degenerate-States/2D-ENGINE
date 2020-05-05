#define _USE_MATH_DEFINES
#include "player.h"
#include "assets.h"
#include "bulletManager.h"
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

void Gun::init(int ID, Assets* assets,BulletManager* bulletMan){
    this->ID = ID;
    this->bulletMan = bulletMan;
    this->rb.init(1.,0,0,0.0);
    this->poly.init(&(assets->gunAsset),white);
}
void Gun::update(Screen* screen,RigidBody*rb,double dt){
    int x,y;
    this->rb.pos = rb->pos;
    SDL_GetMouseState(&x, &y);
    this->rb.setRot(std::arg(screen->pixelScreenToWorld(x,y) - this->rb.pos));
    this->rb.update(dt);
    this->poly.update(&this->rb);
}
void Gun::render(Screen* screen){
    this->poly.render(screen);
}
void Gun::events(SDL_Event* event, Screen* screen, double dt){
    if(event->type == SDL_MOUSEBUTTONDOWN){
        if (event->button.button == SDL_BUTTON_LEFT){
            std::complex<double> fireDirection = screen->pixelScreenToWorld(event->button.x,event->button.y) - this->rb.pos;

            this->bulletMan->fireBullet(white,orange, this->ID, 5.0, 0.01, this->rb.pos, fireDirection, 2);
        }
    }
}



void Player::init(Assets* assets,BulletManager* bulletMan){
    this->rb.init(1.,0,0,0.0);

    this->poly.init(&(assets->plrAsset),white);
    this->gun.init(this->ID,assets,bulletMan);
}
void Player::update(Screen* screen,double dt){
    this->rb.update(dt);
    this->poly.update(&(this->rb));
    this->gun.update(screen,&this->rb, dt);
}
void Player::render(Screen* screen){
    this->poly.render(screen);
    this->gun.render(screen);
}
void Player::events(SDL_Event* event, Screen* screen,double dt){
    this->gun.events(event,screen,dt);
}
void Player::keys(const Uint8* keys,double dt){
    std::complex<double> direction = 0.0;

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