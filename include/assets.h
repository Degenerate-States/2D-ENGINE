#pragma once

#include "components.h"
#include <complex>
#include <vector>

// used to serialize rigged poly joint data
struct jointInfo{
    vector<int> indices;
    complex<double> pos;

    vector<complex<double>> visualAsset;
    //member initalizer
    jointInfo(vector<int> indices,complex<double> pos,vector<complex<double>> visualAsset = {}) : 
                indices(indices), pos(pos), visualAsset(visualAsset){}
};

class Assets{
    private:
        int colliderID;
    public:
        // list of all game assets
        vector<complex<double>> plrAsset;
        vector<complex<double>> gunAsset;
        vector<complex<double>> flameAsset;
        vector<complex<double>> box;

        vector<complex<double>> swarmerAsset;

        vector<complex<double>> outerEngBall;
        vector<complex<double>> innerEngBall;

        vector<complex<double>> testPoly;
        vector<jointInfo> testJoints;

        vector<complex<double>> testSnakePoly;
        vector<jointInfo> testSnakeJoints;

        vector<complex<double>> testEye;
        //where all the assets are acutally defined
        void init();

        int getID();
};