#include "components.h"
#define _USE_MATH_DEFINES
using namespace std;




//************************//
//** RIGIDBODY METHODS  **//
//************************//
void RigidBody::init(double m, double x, double y,double initRot){
    this->mass = m;
    this->pos = {x,y};
    this->rot = initRot;
    this->rotVel = 0.0;
    this->active = true;
    //initalizes rotOp
    this->rotOp = {cos(initRot),sin(initRot)};
}
void RigidBody::setPos(double x,double y){
    this->pos = {x,y};

}
void RigidBody::displace(double dx,double dy){
    this->pos += {dx,dx};
}
void RigidBody::fixedDisplace(double vx,double vy, double dt){
    this->pos += {vx*dt,vy*dt};
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
    this->rot = fmod(theta,2*M_PI);
}
void RigidBody::rotate(double dTheta){
    this->rot += dTheta;
    this->rot = fmod(this->rot,2*M_PI);
}
void RigidBody::fixedRotate(double rotVel,double dt){
    this->rot += rotVel*dt;
    this->rot = fmod(this->rot,2*M_PI);
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
tuple<double,double> Screen::worldToScreen(complex<double> val){
    val -= this->rb.pos;
    //multiplies by conj of rotation operator corrisponds to negative rotation
    val *= conj(this->rb.rotOp);
    val *= this->zoom;

    // must multiply y coord by aspect ratio 
    return make_tuple(real(val), imag(val)*(this->aspectRatio));
}
complex<double> Screen::screenToWorld(double x, double y){

    complex<double> val ={x,y/(this->aspectRatio)};

    val /= this->zoom;
    val *= this->rb.rotOp;
    val += this->rb.pos;
    
    return val;
}
complex<double> Screen::pixelScreenToWorld(int x, int y){
    double screenX = 2.0*x/windowSizeX - 1.0;
    double screenY = -2.0*y/windowSizeY + 1.0;
    
    return this->screenToWorld(screenX,screenY);
}
void Screen::changeZoom(double zoomVel,double dt){
    //keeps zoom from becoming negative
    if(this->zoom + zoomVel*dt > 0.0){
        this->zoom += zoomVel*dt;
    }
}
void Screen::keys(const Uint8* keys,double dt){
    if (keys[SDL_SCANCODE_UP]){
        this->changeZoom(3.0,dt);
    }
    if (keys[SDL_SCANCODE_DOWN]){
        this->changeZoom(-3.0,dt);
    }
}


//************************//
//**   Point Methods    **//
//************************//
void Point::init(RigidBody* rb, tuple<int,int,int> color, float diameter){
    this->color = color;
    this->rb = rb;
    this->diameter = diameter;
}
void Point::render(Screen* screen){
    if(this->diameter > minLinThickness){
        glColor3ub(get<0>(color),get<1>(color),get<2>(color));
        glPointSize(screen->zoom*this->diameter);

        glBegin(GL_POINTS);
        tuple<double,double> coord = (*screen).worldToScreen(this->rb->pos);
        glVertex2d(get<0>(coord), get<1>(coord));
        glEnd();
    }
}
void Point::changeColor(tuple<int,int,int> color){
    this->color = color;
}



//************************//
//**  Polygon Methods   **//
//************************//
void Polygon::appendPoint(complex<double> pnt){
    this->assetRE.push_back(pnt);
    this->assetWR1.push_back(pnt);
    this->assetWR2.push_back(pnt);

    this->vertexOffsets.push_back(0);
    this->numVertices+=1;

}
void Polygon::loadAsset(vector<complex<double>>* asset,tuple<int,int,int> color){

    this->color = color;
    //loads asset and finds its normals
    this->assetRE.clear();
    this->assetWR1.clear();
    this->assetWR2.clear();
    this->vertexOffsets.clear();
    this->numVertices = 0;
    for(int i = 0; i < asset->size(); i++){

        this->appendPoint((*asset)[i]);
    }
}
complex<double> Polygon::getNormal(int index){
    complex<double> normal = (*this->nextAsset)[(index+1)%this->numVertices] - (*this->nextAsset)[index];
    normal*=this->rotNegative90;
    normal/=abs(normal);
    return normal;
}
tuple<double,complex<double>> Polygon::getContainingCircle(){
    //radius
    double furthest = 0;
    double current;

    //center
    complex<double> center = 0;
    for(int i=0; i<this->numVertices; i++){
        current = abs((*this->nextAsset)[i]);
        if(current>furthest){
            furthest = current;
        }
        center+= (*this->nextAsset)[i];
    }
    center /= this->numVertices;
    return make_tuple(furthest,center);
}
void Polygon::resetVertexOffsets(){
    for(int i=0; i<this->numVertices; i++){
        this->vertexOffsets[i] = 0;
    }
}
void Polygon::init(vector<complex<double>>* asset,RigidBody* rb, tuple<int,int,int> color){
    this->loadAsset(asset,color);
    this->currentAsset = &this->assetWR1;
    this->nextAsset = &this->assetWR2;

    this->rb = rb;
    this->rotNegative90 = {0,-1};
    this->lineThickness = defaultLineThickness;
    this->scale = 1.0;
}
void Polygon::update(){
    for(int i=0; i < this->numVertices; i++){
        //rotates and scales polygon after applying warping from vertex offsets
        (*this->nextAsset)[i] =this->scale * this->rb->rotOp *( (this->assetRE)[i] + this->vertexOffsets[i]);
        //translates polygon
        (*this->nextAsset)[i] += this->rb->pos;

    }
}
void Polygon::render(Screen* screen){
    swap(this->nextAsset,this->currentAsset);
    glColor3ub(get<0>(color),get<1>(color),get<2>(color));

    tuple<double,double> coord;

    glLineWidth(this->lineThickness*screen->zoom);
    glBegin(GL_LINE_LOOP);
    for(int i=0; i < this->currentAsset->size(); i++){
        coord = (*screen).worldToScreen((*this->currentAsset)[i]);
        glVertex2d(get<0>(coord), get<1>(coord));
    }
    glEnd();
}

void Trail::init(RigidBody* rb, double thickness, int numVertices,double decayTime){
    this->rb = rb;

    this->headThickness = thickness;
    
    this->numVertices = numVertices;
    this->decayTime = decayTime;

    this->segmentColors.resize(numVertices-1);

    complex<double> zero = {0,0};
    for(int i = 0; i < numVertices; i++){
        this->vertexPos.push_back(zero);
        this->vetexTimers.push_back(0.0);
    }
}
void Trail::reset(tuple<int,int,int> headColor,tuple<int,int,int> tailColor){
    this->headColor=headColor;
    this->tailColor=tailColor;

    //sets inital pos and vertex decay times(so they all dont dissappear at once)
    this->trailHeadIndex = 0;
    double time;
    for(int i = 0; i < this->numVertices; i++){
        this->vertexPos[i] = this->rb->pos;
        time = this->decayTime - this->decayTime*((double)i)/((double) this->numVertices);
        this->vetexTimers[i] = time;
    }

    // determines colors of each segment as gradient between head and tail color
    int  red,green,blue;
    double colorWeight;
    for(int i = 0; i < this->numVertices-1; i++){
        
        colorWeight = ((double)(i)/(double)(this->numVertices-2));
        red = get<0>(this->headColor)*(1-colorWeight);
        red += get<0>(this->tailColor)*colorWeight;
        green = get<1>(this->headColor)*(1-colorWeight);
        green += get<1>(this->tailColor)*colorWeight;
        blue = get<2>(this->headColor)*(1-colorWeight);
        blue += get<2>(this->tailColor)*colorWeight;
        this->segmentColors[i] = make_tuple(red,green,blue);
    }
    
}
void Trail::update(double dt){
    for(int i = 0; i < this->numVertices; i++){
        this->vetexTimers[i] -= dt;
        
        //if this is true, ith vertex because new trail head
        if (this->vetexTimers[i] <= 0.0){
            // += might be better here, although it might cause timers to accumulate rounding errors
            this->vetexTimers[i] = this->decayTime;
            this->trailHeadIndex = i;
        }
    }
    this->vertexPos[this->trailHeadIndex] = this->rb->pos;
}
void Trail::render(Screen* screen){

    double thickness;
    int index;
    tuple<double,double> coord1;
    tuple<double,double> coord2;

    coord1 = screen->worldToScreen(this->vertexPos[this->trailHeadIndex]);

    for(int i = 1; i < this->numVertices; i++){
        index = (trailHeadIndex+i)%this->numVertices;
        thickness = screen->zoom*(this->vetexTimers[index]/this->decayTime)*this->headThickness;
        coord2 = screen->worldToScreen(this->vertexPos[index]);

        glLineWidth(thickness);

        //sets color to gradient calculated in reset method
        if(thickness > minLinThickness){
            glColor3ub(get<0>(this->segmentColors[i-1]), 
                get<1>(this->segmentColors[i-1]), get<2>(this->segmentColors[i-1]));
        }else{
            glColor3ub(get<0>(black),get<1>(black),get<2>(black));
        }


        glBegin(GL_LINES);
        glVertex2d(get<0>(coord1), get<1>(coord1));
        glVertex2d(get<0>(coord2), get<1>(coord2));
        glEnd();

        coord1 = coord2;
    }
}