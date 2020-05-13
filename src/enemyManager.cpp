#include "enemyManager.h"

void Swarmer::init(Assets* assets,RigidBody* plrRB, Stats* stats){
    this->ID = assets->getID();
    this->rb.init(1.,0,0,0.0);

    this->topSpeed = stats->swarmerTopSpeed;
    this->acceleration = stats->swarmerAcceleration;

    this->poly.init(&assets->swarmerAsset,&this->rb,red);
    this->rb.active = false;
}
void Swarmer::spawn(complex<double> pos, complex<double> vel){
    this->rb.active = true;
    this->rb.pos = pos;
    this->rb.vel = vel;

}
void Swarmer::die(){
    this->rb.active = false;
}
void Swarmer::update(double dt){
    // accelerates in direction of player
    complex<double> direction = this->plrRB->pos - this->rb.pos;
    direction *= this->acceleration/abs(direction);
    this->rb.applyForce(real(direction),imag(direction),dt);

    //points in direction of motion
    this->rb.setRot(arg(this->rb.vel));

    //clamps velocity
    if (abs(this->rb.vel) > this->topSpeed){
        this->rb.vel *= this->topSpeed/abs(this->rb.vel);
    }

    this->rb.update(dt);
    this->poly.update();
}
void Swarmer::render(Screen* screen){
    this->poly.render(screen);
}




void EnemyManager::init(Assets* assets, ProjectileManager* projMan,RigidBody* plrRB, Stats* stats){

}

void EnemyManager::spawnSwarmer(complex<double> pos){

}
void EnemyManager::update(Screen* screen, double dt){

}
void EnemyManager::render(Screen* screen){

}


