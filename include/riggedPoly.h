#pragma once
#include "components.h"
#include "assets.h"


class Joint{
    private:
        Polygon* poly;

    public:
        complex<double> posRelAsset;
        RigidBody rb;
        tuple<int,int,int> color;

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