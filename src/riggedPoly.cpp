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

void Skeleton::init(vector<complex<double>>* polyAsset, vector<jointInfo>* jointData, 
                    vector<linkInfo>* linkingInfo, tuple<int,int,int> color,int ID){

    this->rp.init(polyAsset,jointData,color,ID);
    this->rb = &this->rp.joints[0]->rb;

    // index queue system ensures this->links is sorted by number of links to root joint 0,
    // this way when skeleton is updated, its done outwards from the root joint.
    int headIndex;
    int tailIndex;
    int linkNum;
    queue<int> indexQueue;
    indexQueue.push(0);

    while (indexQueue.size()>0){
        headIndex = indexQueue.front();
        indexQueue.pop();

        for(int j = 0; j < (*linkingInfo)[headIndex].indices.size(); j++){
            tailIndex = (*linkingInfo)[headIndex].indices[j];
    
            indexQueue.push(tailIndex);

            this->links.push_back(new link());
            linkNum = this->links.size()-1;
            this->links[linkNum]->headIndex = headIndex;
            this->links[linkNum]->tailIndex = tailIndex;
            this->links[linkNum]->relPos = this->rp.joints[tailIndex]->posRelAsset - this->rp.joints[headIndex]->posRelAsset;
            this->links[linkNum]->relAngle = arg(this->links[linkNum]->relPos);
            this->links[linkNum]->len = abs(this->links[linkNum]->relPos);
            this->links[linkNum]->rigidity = (*linkingInfo)[headIndex].rigidity[j];
            
        }
        
    }
    this->numLinks = this->links.size();

    //jointLinks initalization
    for (int i = 0; i < this->rp.numJoints; i++){
        this->jointLinks.push_back(new jointLink());
    }
    for (int i = 0; i < this->numLinks; i++){
        tailIndex = this->links[i]->tailIndex;
        headIndex = this->links[i]->headIndex;
        this->jointLinks[tailIndex]->base = headIndex;

        this->jointLinks[tailIndex]->relAngle = arg(this->rp.joints[tailIndex]->posRelAsset - this->rp.joints[headIndex]->posRelAsset);
        this->jointLinks[headIndex]->branches.push_back(tailIndex);
    }
    for (int i = 0; i < this->rp.numJoints; i++){
        this->jointLinks[i]->numBranches = this->jointLinks[i]->branches.size();
    }
    // root joint doesnt have a head, so its just set to -1
    this->jointLinks[0]->base= -1;
    
}
void Skeleton::spawn(complex<double> pos, double rot){
    this->rb->pos = pos;
    this->rb->setRot(rot);

    //updating rb calcuates its rotation operator which is used to determine connected joint posisitons
    this->rb->update(0);

    complex<double> relPos;
    Joint* headJoint;
    Joint* tailJoint;
    for (int i = 0; i < this->numLinks; i++)
    {
        headJoint = this->rp.joints[this->links[i]->headIndex];
        tailJoint = this->rp.joints[this->links[i]->tailIndex];
        relPos =this->rb->rotOp * this->links[i]->relPos;
        //tailJoints rotaiton is set to face headJoint
        tailJoint->rb.setRot(arg(-relPos));

        tailJoint->rb.pos = headJoint->rb.pos + relPos;
    }
    
}
void Skeleton::update(double dt){
    //updating rb calcuates its rotation operator which is used to determine connected joint posisitons
    this->rp.joints[0]->update(0);

    complex<double> equilibrumPos;
    complex<double> relPos;
    Joint* headJoint;
    Joint* tailJoint;

    double theta;
    //joint placement
    for (int i = 0; i < this->numLinks; i++){
        headJoint = this->rp.joints[this->links[i]->headIndex];
        tailJoint = this->rp.joints[this->links[i]->tailIndex];
        equilibrumPos = headJoint->rb.rotOp * this->links[i]->relPos;
        relPos = tailJoint->rb.pos - headJoint->rb.pos;
        // scales to proper length
        relPos*= this->links[i]->len / abs(relPos);
        
        //moves towards proper angle
        theta = smallestAngle(arg(equilibrumPos),arg(relPos)) * dt * this->links[i]->rigidity;
        relPos*={cos(theta),sin(theta)};
        

        tailJoint->rb.pos = headJoint->rb.pos + relPos;
        tailJoint->rb.setRot(arg(relPos)-this->links[i]->relAngle);

        tailJoint->rb.update(0);
    }

    //joint rotation
    complex<double> outVec;
    complex<double> inVec;
    double phi;
    double deltaAngle;
    for (int i = 1; i < this->rp.numJoints; i++){
        //invec = this joints pos - base joints pos
        inVec = this->rp.joints[i]->rb.pos - this->rp.joints[this->jointLinks[i]->base]->rb.pos;
        //outvec is average of all the branch vectors (its len is irrelevent so we dont divide)
        outVec = 0;
        for (int j = 0; j < this->jointLinks[i]->numBranches; j++){
            // branch joints pos - this joint pos
            outVec += this->rp.joints[this->jointLinks[i]->branches[j]]->rb.pos - this->rp.joints[i]->rb.pos;
        }
        //if there are no branches, it defaults to inVec
        if(this->jointLinks[i]->numBranches==0){
            outVec = inVec;
        }

        theta = arg(outVec);
        phi = arg(inVec);
        
        deltaAngle = smallestAngle(theta,phi);
        
        //here we set the angle of the joint to be half way between outVec and inVec
        this->rp.joints[i]->rb.setRot(theta-0.5*deltaAngle - this->jointLinks[i]->relAngle);
        //segment must widen to maintain thickness while turning
        this->rp.joints[i]->scale = 1/cos(0.5*deltaAngle);
        
    }
    //update polygon now
    this->rp.update(dt);
}
void Skeleton::render(Screen* screen){
    this->rp.render(screen);



    #if RENDER_SPINE
    double thickness;
    int index;

    Joint* headJoint;
    Joint* tailJoint;
    tuple<double,double> coord1;
    tuple<double,double> coord2;
    
    glLineWidth(defaultLineThickness);
    glColor3ub(get<0>(white), get<1>(white), get<2>(white));

    for(int i = 0; i < this->numLinks; i++){

        headJoint = this->rp.joints[this->links[i]->headIndex];
        tailJoint = this->rp.joints[this->links[i]->tailIndex];

        coord1 = screen->worldToScreen(headJoint->rb.pos);
        coord2 = screen->worldToScreen(tailJoint->rb.pos);

        glBegin(GL_LINES);
        glVertex2d(get<0>(coord1), get<1>(coord1));
        glVertex2d(get<0>(coord2), get<1>(coord2));
        glEnd();
    }
    #endif
}