#include "riggedPoly.h"


void Joint::init(Polygon* poly, vector<int>* indices,complex<double> initPos){
    this->numVerticies = indices->size();
    this->poly = poly;
    this->posRelAsset = initPos;
    this->rb.init(1.0,0,0,0);
    for(int i = 0; i<this->numVerticies; i++){
        this->attachedVerts.push_back((*indices)[i]);
    }
}
void Joint::update(double dt){
    this->rb.update(dt);
    for(int i=0; i < this->numVerticies; i++){
        int index = this->attachedVerts[i];
        //rotates and scales polygon after applying warping from vertex offsets
        (*this->poly->nextAsset)[index] = this->poly->assetRE[index] - this->posRelAsset + this->poly->vertexOffsets[index];
        (*this->poly->nextAsset)[index] *= this->poly->scale * this->rb.rotOp;
        //translates polygon
        (*this->poly->nextAsset)[index] += this->rb.pos + this->posRelAsset;
    }
}

void RiggedPoly::init(vector<complex<double>>* polyAsset, vector<jointInfo>* jointData, tuple<int,int,int> color){
    //Rigidbody reference is set to null, polygons behavior is largly overrided by joint
    this->poly.init(polyAsset,NULL,color);
    this->numJoints = jointData->size();

    for(int i = 0; i < this->numJoints; i++){
        this->joints.push_back(new Joint());
        // gives reference to poly, reference to vector of indices, and inital posistion of joint
        this->joints[i]->init(&this->poly,&(*jointData)[i].indices, (*jointData)[i].pos);
    }
}

void RiggedPoly::update(double dt){
    for(int i = 0; i < this->numJoints; i++){
        // gives reference to poly, reference to vector of indices, and inital posistion of joint
        this->joints[i]->update(dt);
    }
}
void RiggedPoly::render(Screen* screen){
    this->poly.render(screen);
}