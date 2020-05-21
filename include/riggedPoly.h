#pragma once
#include "components.h"
#include "assets.h"
#include "collisionFuncs.h" // only used for smallestAngle function

class Joint{
    private:
        Polygon* poly;
        complex<double> posRelAsset;
    public:

        RigidBody rb;
        tuple<int,int,int> color;

        //used to change distance from join rb 
        double scale;
        // indices of vertexes puppeted by joint
        vector<int> attachedVerts;
        int numVerticies;

        void init(Polygon* poly, vector<int>* indices,complex<double> initPos);
        void update(double dt);

};

class RiggedPoly{
    public:
        // rigidbody is required for collision engine to test if poly is active
        RigidBody rb;
        Polygon poly;
        vector<Joint*> joints;
        int numJoints;

        void init(vector<complex<double>>* polyAsset, vector<jointInfo>* jointData, tuple<int,int,int> color, int ID);

        void update(double dt);
        void render(Screen* screen);
};

// joints must be fed in head to tail
// accomidates snakes of variable segment width and length
// requires fixed framerate
class Snake{
    private:
        vector<complex<double>> pathTraveled;
        int pathTraveledLen;

        int headIndex;
        //where on the path each vertex is relative to the head index
        //i th vertex index = headIndex + vertexIndexOffsets[i]
        vector<int> vertexIndexOffsets;

        vector<double> segmentLens;
        
        // will need setter for adjustable speed
        double speed;
        double turnSpeed;
    public:
        RiggedPoly rp;

        double rot;


        void init(vector<complex<double>>* polyAsset, vector<jointInfo>* jointData, 
                    tuple<int,int,int> color, double speed, double turnSpeed, int ID,double spf);

        void spawn(complex<double> pos, double rot,double spf);

        // turn in number between -1 and 1 for 
        void update(double turn, double dt);
        void render(Screen* screen);
        
};