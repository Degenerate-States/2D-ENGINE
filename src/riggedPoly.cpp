#include "riggedPoly.h"

//#define RENDER_SPINE true

void Joint::init(Polygon* poly, vector<int>* indices,complex<double> initPos){
    this->numVerticies = indices->size();
    this->poly = poly;
    this->rb.init(1.0,0,0,0);
    this->rb.pos = initPos;
    this->posRelAsset = initPos;
    this->scale = 1;
    for(int i = 0; i<this->numVerticies; i++){
        this->attachedVerts.push_back((*indices)[i]);
    }
}

void Joint::update(double dt){
    this->rb.update(dt);
    int index;
    for(int i=0; i < this->numVerticies; i++){
        index = this->attachedVerts[i];
        //rotates and scales polygon after applying warping from vertex offsets
        (*this->poly->nextAsset)[index] = this->poly->assetRE[index] -this->posRelAsset + this->poly->vertexOffsets[index];
        
        //joint scale overrides polygon scale
        (*this->poly->nextAsset)[index] *= this->scale * this->rb.rotOp;
        //translates polygon
        (*this->poly->nextAsset)[index] += this->rb.pos;
    }
}

void RiggedPoly::init(vector<complex<double>>* polyAsset, vector<jointInfo>* jointData, tuple<int,int,int> color,int ID){
    //Rigidbody reference is set to null, polygons behavior is largly overrided by joint
    this->rb.init(1,0,0,0);
    this->poly.init(polyAsset,&this->rb,color, ID);
    this->numJoints = jointData->size();

    for(int i = 0; i < this->numJoints; i++){
        this->joints.push_back(new Joint());
        // gives reference to poly, reference to vector of indices, and inital posistion of joint
        this->joints[i]->init(&this->poly,&(*jointData)[i].indices, (*jointData)[i].pos);

        //if joint has a visual asset, its initalized and added to visPolys
        if ((*jointData)[i].visualAsset.size() > 0){
            this->visPolys.push_back(new VisualPolygon());
            this->visPolys[i]->init(&(*jointData)[i].visualAsset,&this->joints[i]->rb,color);
            this->visPolys[i]->rebase((*jointData)[i].pos);
        }
    }
    this->numVisPolys = this->visPolys.size();
}

void RiggedPoly::update(double dt){
    //updates joints
    for(int i = 0; i < this->numJoints; i++){
        // gives reference to poly, reference to vector of indices, and inital posistion of joint
        this->joints[i]->update(dt);
    }
    //updates visual polygons
    for (int i = 0; i < this->numVisPolys; i++){
        this->visPolys[i]->update();
    }
    

}

void RiggedPoly::render(Screen* screen){
    this->poly.render(screen);

    //renders visual polygons
    for (int i = 0; i < this->numVisPolys; i++){
        this->visPolys[i]->render(screen);
    }
}



void Snake::init(vector<complex<double>>* polyAsset, vector<jointInfo>* jointData, 
                    tuple<int,int,int> color, double speed, double turnSpeed, int ID,double spf){

    this->rp.init(polyAsset,jointData,color,ID);
    this->speed = speed;
    this->turnSpeed = turnSpeed;
    
    //iterates through segments
    double len;
    double segmentLen;
    
    int index = 0;
    this->vertexIndexOffsets.push_back(index); // head index has no offset
    this->pathTraveled.push_back({0,0});
    for(int i = 1; i < this->rp.numJoints; i++){
        len = 0;
        segmentLen = abs(this->rp.joints[i]->rb.pos - this->rp.joints[i-1]->rb.pos);

        //appends new point to point history path
        while (len < segmentLen){
            this->pathTraveled.push_back({0,0});
            len += speed*spf;
            index +=1;
        }
        // i th vertex is at head index + this offset
        this->vertexIndexOffsets.push_back(index);
        this->segmentLens.push_back(segmentLen);
    }
    this->pathTraveledLen = this->pathTraveled.size();
}
void Snake::spawn(complex<double> pos, double rot,double spf){

    this->headIndex = 0;
    this->rp.joints[this->headIndex]->rb.pos=pos;
    complex<double> tailDirection = {-cos(rot), -sin(rot)};

    //unrolls prevPosistions
    for(int i = 0; i <this->pathTraveledLen; i++){
        this->pathTraveled[i] = pos + (double)i*this->speed*spf*tailDirection;
    }

    for(int i = 0; i <this->rp.numJoints-1; i++){
    }

}

void Snake::update(double turn, double dt){
    //movement
    this->rot += turn*this->turnSpeed*dt;
    this->rp.joints[0]->rb.fixedDisplace(this->speed*cos(this->rot),this->speed*sin(this->rot),dt);
    this->rp.joints[0]->rb.setRot(this->rot);

    this->headIndex -=1;
    this->headIndex = ( this->pathTraveledLen + (this->headIndex% this->pathTraveledLen)) %  this->pathTraveledLen;
    
    //updates path traveled with new point
    this->pathTraveled[headIndex] = this->rp.joints[0]->rb.pos;
    
    //updates Joint Pos
    int index;
    for(int i = 1; i < this->rp.numJoints; i++){
        index = headIndex + this->vertexIndexOffsets[i];
        index %= this->pathTraveledLen;

        this->rp.joints[i]->rb.pos = this->pathTraveled[index];
    }

    //joint rotation and thickness
    complex<double> prevPos;
    complex<double> currentPos;
    complex<double> nextPos;
    double theta;
    double phi;
    double deltaAngle; 

    for(int i = 1; i <this->rp.numJoints-1; i++){
        prevPos = this->rp.joints[i-1]->rb.pos;
        currentPos = this->rp.joints[i]->rb.pos;
        nextPos = this->rp.joints[i+1]->rb.pos;

        theta = arg(nextPos-currentPos);
        phi = arg(currentPos-prevPos);
        
        deltaAngle = smallestAngle(theta,phi);

        //angle setting and turn smoothing
        //angle of joint is set to halfway between the angle of the segments
        this->rp.joints[i]->rb.setRot(M_PI+theta-0.5*deltaAngle);

        //segment must widen to maintain thickness while turning
        this->rp.joints[i]->scale = 1/cos(0.5*deltaAngle);
    }
    this->rp.joints[this->rp.numJoints-1]->rb.setRot(M_PI+theta);

    this->rp.update(dt);
}

void Snake::render(Screen* screen){
    this->rp.render(screen);

    #if RENDER_SPINE
    double thickness;
    int index;
    tuple<double,double> coord1;
    tuple<double,double> coord2;

    coord1 = screen->worldToScreen(this->rp.joints[0]->rb.pos);
    glLineWidth(defaultLineThickness);
    glColor3ub(get<0>(white), get<1>(white), get<2>(white));
    for(int i = 1; i < this->rp.numJoints; i++){
        coord2 = screen->worldToScreen(this->rp.joints[i]->rb.pos);

        glBegin(GL_LINES);
        glVertex2d(get<0>(coord1), get<1>(coord1));
        glVertex2d(get<0>(coord2), get<1>(coord2));
        glEnd();

        coord1 = coord2;
    }
    #endif
}