#include "miscGameObjs.h"
#include "components.h"
#include "config.h"
#include "assets.h"
using namespace std;

void Box::init(Assets* assets){
    this->rb.init(1.,0.5,0,0);
    this->rb.rotVel = 1;
    this->ID = 2;
    this->poly.init(&(assets->box),white);
}
void Box::update(Screen* screen, double dt){
    this->rb.update(dt);
    this->poly.update(&(this->rb));
}
void Box::render(Screen* screen){
    this->poly.render(screen);
}