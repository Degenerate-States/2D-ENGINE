#include "components.h"
#include <math.h>
#include <complex>
#include <SDL.h>
#include <glad/glad.h>
#include <tuple>
#include <iostream>
using namespace std;


//************************//
//** RIGIDBODY METHODS  **//
//************************//
void RigidBody::init(double m, double x, double y,double initRot){
    this->mass = m;
    this->pos = {x,y};
    this->rot = initRot;
    this->rotVel = 0.0;

    //initalizes rotOp
    this->rotOp = {cos(initRot),sin(initRot)};
}
void RigidBody::setPos(double x,double y){
    this->pos = {x,y};

}
void RigidBody::displace(double dx,double dy){
    this->pos += {dx,dx};
}
void RigidBody::setVel(double vx,double vy){
    this->vel = {vx,vy};

}
void RigidBody::applyForce(double fx, double fy,double dt){
    double coeff = dt / this->mass;
    this->vel += {fx*coeff,fy*coeff};
}
void RigidBody::update(double dt){
    this->pos += this->vel*dt;
    this->rot += this->rotVel*dt;

    //updates rotOp
    this->rotOp = {cos(this->rot),sin(this->rot)};
}
void RigidBody::setRot(double theta){
    this->rot = theta;
}
void RigidBody::rotate(double dTheta){
    this->rot += dTheta;
}
void RigidBody::fixedRotate(double rotVel,double dt){
    this->rot += rotVel*dt;
}


//************************//
//**  Screen Methods    **//
//************************//
void Screen::init(SDL_Window* win, SDL_Renderer* rend, double winAspectRatio){
        this->rb = RigidBody();
        this->rb.init(1.0,0.0,0.0,0.0);

        this->window = win;
        this->renderer = rend;
        this->aspectRatio = winAspectRatio;
        this->zoom = 1.0;
}
void Screen::update(double dt){
    this->rb.update(dt);
}
tuple<double,double> Screen::worldToScreen(std::complex<double> val){
    val -= this->rb.pos;
    //multiplies by conj of rotation operator corrisponds to negative rotation
    val *= std::conj(this->rb.rotOp);
    val *= this->zoom;

    // must multiply y coord by aspect ratio 
    return make_tuple(std::real(val), std::imag(val)*(this->aspectRatio));
}
std::complex<double> Screen::screenToWorld(double x, double y){

    std::complex<double> val ={x,y/(this->aspectRatio)};

    val += this->rb.pos;
    val *= this->rb.rotOp;
    val /= this->zoom;

    return val;
}
void Screen::changeZoom(double zoomVel,double dt){
    //keeps zoom from becoming negative
    if(this->zoom + zoomVel*dt > 0.0){
        this->zoom += zoomVel*dt;
    }
}


//************************//
//**  POLYGON Methods   **//
//************************//
void Polygon::appendPoint(std::complex<double> pnt){
    this->assetRE.push_back(pnt);
    this->assetWR.push_back(pnt);
}
void Polygon::init(uint8_t r,uint8_t g,uint8_t b){
    this->red = r;
    this->green = g;
    this->blue = b;

    this->scale = 1.0;
}
void Polygon::update(RigidBody* rb){
    for(int i=0; i < this->assetWR.size(); i++){
        //rotates and scales polygon
        this->assetWR[i] =this->scale * (*rb).rotOp * (this->assetRE)[i];
        //translates polygon
        this->assetWR[i] += (*rb).pos;

    }
}
void Polygon::render(Screen* screen){
    glColor3d(this->red, this->green, this->blue);
    glBegin(GL_LINE_LOOP);
    for(int i=0; i < this->assetWR.size(); i++){
        tuple<double,double> coord = (*screen).worldToScreen(this->assetWR[i]);
        glVertex2d(get<0>(coord), get<1>(coord));

    }
    glEnd();
}